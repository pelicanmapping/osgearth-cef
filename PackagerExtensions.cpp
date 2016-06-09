#include "PackagerExtensions"
#include "osgEarthExtensions"

#include <osg/Notify>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarth/CompositeTileSource>
#include <osgEarth/CacheEstimator>
#include <osgEarthUtil/TMSPackager>
#include <osgEarth/Registry>

using namespace osgEarth::Cef;
using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Drivers;


class RunCallbackTask: public CefTask
{
public:
    CefRefPtr< CefV8Value > _callback;
    CefRefPtr< CefV8Context > _context;
    double _current;
    double _total;
    int _currentStage;
    int _totalStages;
    std::string _msg;

    RunCallbackTask(CefRefPtr< CefV8Context > context,
        CefRefPtr< CefV8Value > callback,
        double             current, 
        double             total, 
        unsigned           currentStage,
        unsigned           totalStages,
        const std::string& msg 
        ):
    _context(context),
    _callback(callback),
    _current( current ),
    _total( total ),
    _currentStage( currentStage ),
    _totalStages( totalStages ),
    _msg( msg )
    {
    }

    virtual void Execute() OVERRIDE
    {
        CefV8ValueList args;
        args.push_back(CefV8Value::CreateDouble(_current));
        args.push_back(CefV8Value::CreateDouble(_total));
        args.push_back(CefV8Value::CreateInt(_currentStage));
        args.push_back(CefV8Value::CreateInt(_totalStages));
        args.push_back(CefV8Value::CreateString(_msg));
        _callback->ExecuteFunctionWithContext(_context, 0, args);
    }

    // Provide the reference counting implementation for this class.
    IMPLEMENT_REFCOUNTING(RunCallbackTask);
};



class V8ProgressCallback : public ProgressCallback
{
public:

    V8ProgressCallback(CefRefPtr< CefV8Context > context,
                       CefRefPtr< CefV8Value > callback,
                       CefRefPtr< CefTaskRunner > taskRunner):
    _context( context ),
    _callback( callback ),
    _taskRunner( taskRunner )
    {
    }

    virtual bool reportProgress(
        double             current, 
        double             total, 
        unsigned           currentStage,
        unsigned           totalStages,
        const std::string& msg )
    {
        _taskRunner->PostTask( new RunCallbackTask( _context, _callback, current, total, currentStage, totalStages, msg ) );
        return false;
    }
                      
    CefRefPtr< CefV8Context > _context;
    CefRefPtr< CefV8Value > _callback;
    CefRefPtr< CefTaskRunner > _taskRunner;
};

/**
* User data for a package task
*/
class PackagerUserData : public CefBase, public OpenThreads::Thread
{
public:
    PackagerUserData(CefRefPtr< CefV8Value > opt )
    {
        // Get the list of filenames to process
        std::vector< std::string > filenames;
        if (opt->HasValue("filenames")) {
            CefRefPtr< CefV8Value > v8filenames = opt->GetValue("filenames");
            for (int i = 0; i < v8filenames->GetArrayLength(); i++)
            {
                std::string filename = v8filenames->GetValue(i)->GetStringValue().ToString();
                filenames.push_back( filename );
            }
        }

        // The list of TileSources to process
        std::vector< osg::ref_ptr< TileSource > > tileSources;
        if (opt->HasValue("tilesources"))
        {
            CefRefPtr< CefV8Value > v8tilesources = opt->GetValue("tilesources");
            for (int i = 0; i < v8tilesources->GetArrayLength(); i++)
            {
                osg::ref_ptr< TileSource > tileSource = dynamic_cast< TileSource*>(dynamic_cast< ReferencedUserData*>(v8tilesources->GetValue(i)->GetUserData().get())->_ref.get());
                if (tileSource.valid())
                {
                    tileSources.push_back( tileSource );
                }
            }
        }


        osg::ref_ptr< TileVisitor > visitor;

        int cores = 0;
        if (opt->HasValue("cores"))
        {
            cores = opt->GetValue("cores")->GetIntValue();
        }

        cores = max(1, cores);

        if (cores > 1)
        {
            MultithreadedTileVisitor* v = new MultithreadedTileVisitor();
            v->setNumThreads(cores);
            visitor = v;
        }
        else
        {
            visitor = new TileVisitor();
        }
         
        packager.setVisitor( visitor );


        // set the KeepEmpties flag to true
        packager.setKeepEmpties(true);

        std::string extension;
        // Get the extension
        if (opt->HasValue("extension"))
        {
            extension = opt->GetValue("extension")->GetStringValue().ToString();
        }

        // Setup the write options so that it will compress elevation using lzw
        osg::ref_ptr< osgDB::Options > options = new osgDB::Options("tiff_compression=lzw");
        packager.setWriteOptions( options.get() );

        bool elevation = false;
        if (extension == "tif16")
        {
            elevation = true;
            packager.setExtension("tif");
            packager.setElevationPixelDepth(16);
        }
        else if (extension == "tif32")
        {
            elevation = true;
            packager.setExtension("tif");
            packager.setElevationPixelDepth(32);
        }
        else
        {
            if (extension == "png")
            {
                packager.setApplyAlphaMask(true);
            }

            packager.setExtension( extension  );
        }

        // Get the min level
        if (opt->HasValue("min_level"))
        {
            visitor->setMinLevel( opt->GetValue("min_level")->GetIntValue() );
        }

        // Get the max level
        if (opt->HasValue("max_level"))
        {
            visitor->setMaxLevel( opt->GetValue("max_level")->GetIntValue() );
        }

        std::string layername = "layer";
        if (opt->HasValue("layer"))
        {
            layername = opt->GetValue("layer")->GetStringValue().ToString();
        }


        // Get the extents
        double minLon = -180.0;
        double minLat = -90.0;
        double maxLon = 180.0;
        double maxLat = 90.0;

        if (opt->HasValue("extents"))
        {
            CefRefPtr< CefV8Value > extents = opt->GetValue("extents");
            minLon = extents->GetValue(0)->GetIntValue();
            minLat = extents->GetValue(1)->GetIntValue();
            maxLon = extents->GetValue(2)->GetIntValue();
            maxLat = extents->GetValue(3)->GetIntValue();
        }

        std::string profileString = "global-geodetic";
        if (opt->HasValue("profile"))
        {
            profileString = opt->GetValue("profile")->GetStringValue().ToString();
        }

        ProfileOptions profileOptions(profileString);
        osg::ref_ptr< const Profile > profile = Profile::create( profileOptions );
        GeoExtent ext = profile->clampAndTransformExtent( GeoExtent(SpatialReference::create("epsg:4326"), minLon, minLat, maxLon, maxLat));
        visitor->addExtent( ext );

        std::string output;
        if (opt->HasValue("output"))
        {
            output = opt->GetValue("output")->GetStringValue().ToString();
            if (output == "mbtiles")
            {
                std::string destination = "package.mbtiles";
                if ( opt->HasValue("destination"))
                    destination = opt->GetValue("destination")->GetStringValue().ToString();

                if (osgDB::getFileExtension(destination) == "")
                    destination += ".mbtiles";

                Config outConf;
                outConf.set("driver", "mbtiles");
                outConf.set("format", packager.getExtension());
                outConf.set("filename", destination);

                // set the output profile.                
                outConf.add("profile", profileOptions.getConfig());

                TileSourceOptions outOptions(outConf);
                osg::ref_ptr<TileSource> output = TileSourceFactory::create(outOptions);
                if ( output.valid() )
                {
                    TileSource::Status outputStatus = output->open(
                        TileSource::MODE_WRITE | TileSource::MODE_CREATE );

                    if ( !outputStatus.isError() )
                    {
                        packager.setTileSource(output);
                    }
                    else
                    {
                        OE_WARN << "Failed to initialize output TileSource: " << outputStatus.message() << std::endl;
                    }
                }
                else
                {
                    OE_WARN << "Failed to create output TileSource" << std::endl;
                }
            }
            else
            {
                std::string destination = "tiles";
                if ( opt->HasValue("destination"))
                {
                    destination = opt->GetValue("destination")->GetStringValue().ToString();
                }
                packager.setDestination( destination );

                // create a folder for the output
                osgDB::makeDirectory( destination );
            }
        }


        CompositeTileSourceOptions compositeOpt;

        compositeOpt.profile() = profileOptions;

        for (unsigned int i = 0; i < filenames.size(); i++)
        {
            GDALOptions layerOpt;
            layerOpt.url() = filenames[i];

            if (elevation)
            {
                compositeOpt.add(ElevationLayerOptions(filenames[i], layerOpt)); 
            }
            else
            {
                compositeOpt.add(ImageLayerOptions(filenames[i], layerOpt));
            }
        }

        osgEarth::CompositeTileSource* compositeSource = static_cast<osgEarth::CompositeTileSource*>(TileSourceFactory::create( compositeOpt ));
        
        // Add any explicit TileSources
        for (unsigned int i = 0; i < tileSources.size(); i++)
        {
            if (elevation)
            {                
                compositeSource->add(new ElevationLayer(ImageLayerOptions(), tileSources[i].get()));             
            }
            else
            {
                compositeSource->add(new ImageLayer(ImageLayerOptions(), tileSources[i].get()));             
            }
        }

        if (elevation)
        {
            _layer = new osgEarth::ElevationLayer(ElevationLayerOptions(), compositeSource);
        }
        else
        {
            _layer = new osgEarth::ImageLayer( ImageLayerOptions(), compositeSource );
        }     
        _layer->setName(layername);

        MapOptions mapOpt;
        if (profile->getSRS()->isMercator())
        {
            mapOpt.coordSysType() = MapOptions::CSTYPE_PROJECTED;
        }
        mapOpt.profile() = profileOptions;
        _map = new Map(mapOpt);

        _context = CefV8Context::GetCurrentContext();
        _taskRunner = CefTaskRunner::GetForCurrentThread();


        if (opt->HasValue("progress")) {
            _progress = new V8ProgressCallback(_context,
                                               opt->GetValue("progress"),
                                               _taskRunner);
            visitor->setProgressCallback( _progress.get() );                                 
        }

        if (opt->HasValue("complete")) {
            _completeCallback = opt->GetValue("complete");
        }
    }

    bool getDone() { return _done;}
    void setDone( bool done) { _done = done; }

    virtual void run()
    {
        packager.run( _layer, _map );
        // TODO:  This is terrible but is the best way to distinguish between an MBtiles and TMS output.
        if (!packager.getTileSource())
        {
            packager.writeXML(_layer, _map);
        }
        if (_completeCallback)
        {
            _taskRunner->PostTask( new RunCallbackTask( _context, _completeCallback, 0, 0, 0, 0, ""));
        }

        _progress = 0L;
    }

    void cancelPackager()
    {
        if (_progress.valid())
            _progress->cancel();
    }

    volatile bool _done;

    TMSPackager packager;
    osg::ref_ptr< V8ProgressCallback > _progress;

    osg::ref_ptr< TerrainLayer > _layer;
    osg::ref_ptr< Map > _map;

    CefRefPtr< CefV8Context > _context;
    CefRefPtr< CefV8Value > _completeCallback;
    CefRefPtr< CefTaskRunner > _taskRunner;


    

    IMPLEMENT_REFCOUNTING(PackagerUserData);
};


PackagerV8Handler::PackagerV8Handler()
{
}

bool PackagerV8Handler::Execute(const CefString& name,
    CefRefPtr<CefV8Value> object,
    const CefV8ValueList& arguments,
    CefRefPtr<CefV8Value>& retval,
    CefString& exception)
{
    if (name == "Package")
    {
        CefRefPtr< CefV8Value > opt = arguments[0];

        CefRefPtr< PackagerUserData > userData = new PackagerUserData( opt );

        userData->startThread();

        retval = CefV8Value::CreateObject(0);
        retval->SetUserData(userData);
        
        //CefRefPtr<ProgressHandler> progressHandler = new ProgressHandler();
        //retval->SetValue("cancel", CefV8Value::CreateFunction("cancel", progressHandler), V8_PROPERTY_ATTRIBUTE_NONE);   
        return true;
    }
    else if (name == "CancelPackage")
    {
        CefRefPtr< CefV8Value > opt = arguments[0];
        CefRefPtr< PackagerUserData > userData = dynamic_cast< PackagerUserData* >(opt->GetUserData().get());
        if (userData.get())
        {
            if (!userData->getDone())
            {
                userData->cancelPackager();
            }

            return true;
        }

        return true;

    }
    else if (name == "Estimate")
    {
        CefRefPtr< CefV8Value > opt = arguments[0];

        CacheEstimator est;
        if (opt->HasValue("min_level"))
        {
            est.setMinLevel( opt->GetValue("min_level")->GetIntValue() );
        }

        if (opt->HasValue("max_level"))
        {
            est.setMaxLevel( opt->GetValue("max_level")->GetIntValue() );
        }

        double minLon = -180.0;
        double minLat = -90.0;
        double maxLon = 180.0;
        double maxLat = 90.0;

        if (opt->HasValue("extents"))
        {
            CefRefPtr< CefV8Value > extents = opt->GetValue("extents");
            minLon = extents->GetValue(0)->GetIntValue();
            minLat = extents->GetValue(1)->GetIntValue();
            maxLon = extents->GetValue(2)->GetIntValue();
            maxLat = extents->GetValue(3)->GetIntValue();
        }

        est.addExtent(GeoExtent(osgEarth::SpatialReference::create("epsg:4326"), minLon, minLat, maxLon, maxLat));

        retval = CefV8Value::CreateObject(0);
        retval->SetValue("tiles", CefV8Value::CreateUInt(est.getNumTiles()), V8_PROPERTY_ATTRIBUTE_NONE);
        retval->SetValue("size", CefV8Value::CreateDouble(est.getSizeInMB()), V8_PROPERTY_ATTRIBUTE_NONE);
        retval->SetValue("seconds", CefV8Value::CreateDouble(est.getTotalTimeInSeconds()), V8_PROPERTY_ATTRIBUTE_NONE);
        return true;
    }
    return false;
}

namespace
{
    std::string code =
        "var osgearth;"
        "if (!osgearth) {"
        "    osgearth = {};"
        "}"
        "(function() {"
        "    osgearth.package = function(options) {"
        "        native function Package();"
        "        return Package(options);"
        "    };"
        "    osgearth.estimate = function(options) {"
        "        native function Estimate();"
        "        return Estimate(options);"
        "    };"
        "    osgearth.cancelPackage = function(options) {"
        "        native function CancelPackage();"
        "        return CancelPackage(options);"
        "    };"
        "})();";
}


void PackagerAPI::AddExtensions(CefRefPtr<CefV8Value> global)
{
    CefRefPtr< PackagerV8Handler > handler = new PackagerV8Handler();
    CefRegisterExtension("packager", code, handler);
}
