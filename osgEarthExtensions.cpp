#include "osgEarthExtensions"

#include <osgEarth/Notify>
#include <osgEarth/TileSource>
#include <osgEarth/ImageLayer>
#include <osgEarth/ElevationLayer>


using namespace osgEarth::Cef;
using namespace osgEarth;

TileSourceV8Handler::TileSourceV8Handler()
{
}

/**
 * Converts a V8 object to a config.
 */
bool V8ObjectToConfig(CefRefPtr<CefV8Value> object, Config& config)
{
    if (!object->IsObject())
    {
        OE_WARN << "V8ObjectToConfig object is not an object" << std::endl;
        return false;
    }

    std::vector< CefString > keys;
    object->GetKeys(keys);
    for (unsigned int i = 0; i < keys.size(); i++)
    {
        CefRefPtr< CefV8Value> value = object->GetValue(keys[i]);
        
        if (value->IsObject())
        {
            Config child(keys[i].ToString());
            V8ObjectToConfig(value, child);
            config.add(child);
        }
        else
        {
            if (value->IsBool())
            {
                config.set<bool>(keys[i].ToString(), value->GetBoolValue());
            }
            else if (value->IsDouble())
            {
                config.set<double>(keys[i].ToString(), value->GetDoubleValue());
            }
            else if (value->IsInt())
            {
                config.set(keys[i].ToString(), value->GetIntValue());
            }
            else if (value->IsString())
            {
                config.set<std::string>(keys[i].ToString(), value->GetStringValue().ToString());
            }
            else if (value->IsUInt())
            {
                config.set<unsigned int>(keys[i].ToString(), value->GetUIntValue());
            }            
        }
    }
    return true;
}



bool TileSourceV8Handler::Execute(const CefString& name,
    CefRefPtr<CefV8Value> object,
    const CefV8ValueList& arguments,
    CefRefPtr<CefV8Value>& retval,
    CefString& exception)
{
    if (name == "TileSource")
    {        
        std::string driver = arguments[0]->GetStringValue().ToString();
        CefRefPtr< CefV8Value > options = arguments[1];
        Config driverOpt;
        driverOpt.set("driver", driver);
        V8ObjectToConfig(options, driverOpt);

        TileSourceOptions tileSourceOpt( driverOpt );

        osg::ref_ptr< TileSource > tileSource = TileSourceFactory::create( tileSourceOpt );
        if (tileSource.valid())
        {
            TileSource::Status status = tileSource->open( osgEarth::TileSource::MODE_READ );        
            if (!status.isOK())
            {
                exception = "Failed to open TileSource";
                return false;
            }         
        }
        else
        {
            exception = "Failed to create tilesource";
            return false;
        }

        retval = CefV8Value::CreateObject(0);   

        CefRefPtr< ReferencedUserData > userData = new ReferencedUserData( tileSource.get() );
        retval->SetUserData( userData );

        return true;
    }
    return false;
}

LayerV8Handler::LayerV8Handler()
{
}


bool LayerV8Handler::Execute(const CefString& name,
    CefRefPtr<CefV8Value> object,
    const CefV8ValueList& arguments,
    CefRefPtr<CefV8Value>& retval,
    CefString& exception)
{
    if (name == "ImageLayer")
    {        
        osg::ref_ptr< TileSource > tileSource = dynamic_cast< TileSource*>(dynamic_cast< ReferencedUserData*>(arguments[0]->GetUserData().get())->_ref.get());

        CefRefPtr< CefV8Value > optionsObj = arguments[1];
        Config options;
        V8ObjectToConfig(optionsObj, options);

        osgEarth::ImageLayerOptions layerOptions( options );
        osg::ref_ptr< ImageLayer > layer = new ImageLayer(layerOptions, tileSource.get());
        retval = CefV8Value::CreateObject(0);   

        CefRefPtr< ReferencedUserData > userData = new ReferencedUserData( layer.get() );
        retval->SetUserData( userData );

        return true;
    }
    else if (name == "ElevationLayer")
    {
        osg::ref_ptr< TileSource > tileSource = dynamic_cast< TileSource*>(dynamic_cast< ReferencedUserData*>(arguments[0]->GetUserData().get())->_ref.get());

        CefRefPtr< CefV8Value > optionsObj = arguments[1];
        Config options;
        V8ObjectToConfig(optionsObj, options);

        osgEarth::ElevationLayerOptions layerOptions( options );
        osg::ref_ptr< ElevationLayer > layer = new ElevationLayer(layerOptions, tileSource.get());
        retval = CefV8Value::CreateObject(0);   
        CefRefPtr< ReferencedUserData > userData = new ReferencedUserData( layer.get() );
        retval->SetUserData( userData );

        return true;
    }
    else if (name == "setOpacity")
    {
        osg::ref_ptr< ImageLayer > imageLayer = dynamic_cast< ImageLayer*>(dynamic_cast< ReferencedUserData*>(object->GetUserData().get())->_ref.get());
        imageLayer->setOpacity( arguments[0]->GetDoubleValue() );        
        return true;
    }
    else if (name == "getOpacity")
    {
        osg::ref_ptr< ImageLayer > imageLayer = dynamic_cast< ImageLayer*>(dynamic_cast< ReferencedUserData*>(object->GetUserData().get())->_ref.get());
        retval = CefV8Value::CreateDouble(imageLayer->getOpacity());               
        return true;
    }
    else if (name == "getVisible")
    {
        osg::ref_ptr< TerrainLayer > layer = dynamic_cast< TerrainLayer*>(dynamic_cast< ReferencedUserData*>(object->GetUserData().get())->_ref.get());
        retval = CefV8Value::CreateBool(layer->getVisible());
        return true;
    }
    else if (name == "setVisible")
    {
        osg::ref_ptr< TerrainLayer > layer = dynamic_cast< TerrainLayer*>(dynamic_cast< ReferencedUserData*>(object->GetUserData().get())->_ref.get());
        layer->setVisible(arguments[0]->GetBoolValue());
        return true;
    }
    return false;
}

namespace
{
    std::string osgEarthTileSourceExtension =
        "var osgearth;"
        "if (!osgearth) {"
        "    osgearth = {};"
        "}"
        "(function() {"        
        "    osgearth.TileSource = function(driver, options) {"
        "        native function TileSource();"
        "        return TileSource(driver, options);"
        "    };"
        "})();";

    std::string osgEarthLayerExtension =
        "var osgearth;"
        "if (!osgearth) {"
        "    osgearth = {};"
        "}"
        "(function() {"        
        "    native function getVisible();"
        "    native function setVisible();"

        "    osgearth.ImageLayer = function(tilesource, options) {"
        "        native function ImageLayer();\n"
        "        native function getOpacity();"
        "        native function setOpacity();"

        "        var ret = ImageLayer(tilesource, options);"
        "        ret.getOpacity = getOpacity;\n"
        "        ret.setOpacity = setOpacity;\n"        
        "        ret.getVisible = getVisible;\n"
        "        ret.setVisible = setVisible;\n"
        "        return ret;\n"
        "    };"
        "    osgearth.ElevationLayer = function(tilesource, options) {"
        "        var ret = ElevationLayer(tilesource, options);"
        "        ret.getVisible = getVisible;\n"
        "        ret.setVisible = setVisible;\n"        
        "        return ret;\n"
        "    };"
        "})();";

}


void osgEarthAPI::AddExtensions(CefRefPtr<CefV8Value> global)
{
    CefRefPtr< TileSourceV8Handler > tileSourceHandler = new TileSourceV8Handler();
    CefRegisterExtension("osgEarth/TileSource", osgEarthTileSourceExtension, tileSourceHandler);

    CefRefPtr< LayerV8Handler > layerHandler = new LayerV8Handler();
    CefRegisterExtension("osgEarth/Layer", osgEarthLayerExtension, layerHandler);
}
