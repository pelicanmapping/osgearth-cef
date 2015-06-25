#include "PackagerExtensions"

#include <osg/Notify>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarthUtil/TMSPackager>

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
            for (unsigned int i = 0; i < v8filenames->GetArrayLength(); i++)
            {
                std::string filename = v8filenames->GetValue(i)->GetStringValue().ToString();
                filenames.push_back( filename );
            }
        }


        osg::ref_ptr< TileVisitor > visitor = new TileVisitor(); 
        packager.setVisitor( visitor );


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

        std::string destination = "tiles";
        if ( opt->HasValue("destination"))
        {
            destination = opt->GetValue("destination")->GetStringValue().ToString();
        }
        packager.setDestination( destination );

        // create a folder for the output
        osgDB::makeDirectory( destination );

        // Create an image layer from just the first image for now and tile it.  Need to make a composite and reproject them, etc.
        GDALOptions layerOpt;
        layerOpt.url() = filenames[0];

        if (elevation)
        {
             _layer = new osgEarth::ElevationLayer( ElevationLayerOptions("layer", layerOpt) );
        }
        else
        {
            _layer = new osgEarth::ImageLayer( ImageLayerOptions("layer", layerOpt) );
        }
       

        _map = new Map();

        _context = CefV8Context::GetCurrentContext();
        _taskRunner = CefTaskRunner::GetForCurrentThread();


        if (opt->HasValue("progress")) {
            visitor->setProgressCallback( new V8ProgressCallback(_context,
                                                                 opt->GetValue("progress"),
                                                                 _taskRunner));                                 
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
        if (_completeCallback)
        {
            _taskRunner->PostTask( new RunCallbackTask( _context, _completeCallback, 0, 0, 0, 0, ""));
        }
    }

    volatile bool _done;

    TMSPackager packager;

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
        "})();";
}


void PackagerAPI::AddExtensions(CefRefPtr<CefV8Value> global)
{
    CefRefPtr< PackagerV8Handler > handler = new PackagerV8Handler();
    CefRegisterExtension("packager", code, handler);
}
