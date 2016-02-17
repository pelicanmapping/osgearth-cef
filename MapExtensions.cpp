#include "MapExtensions"

#include <osg/Notify>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

using namespace osgEarth::Cef;
using namespace osgEarth;


MapV8Handler::MapV8Handler()
{
}

bool MapV8Handler::Execute(const CefString& name,
    CefRefPtr<CefV8Value> object,
    const CefV8ValueList& arguments,
    CefRefPtr<CefV8Value>& retval,
    CefString& exception)
{
    if (name == "ScreenCapture")
    {
        CefRefPtr< CefV8Value > opt = arguments[0];

        /*int x = 0;
        int y = 0;
        int width = -1;
        int height = -1;

        if (opt->HasValue("x"))
        {
            x = opt->GetValue("min_level")->GetIntValue();
        }

        if (opt->HasValue("y"))
        {
            y = opt->GetValue("y")->GetIntValue();
        }

        if (opt->HasValue("width"))
        {
            width = opt->GetValue("width")->GetIntValue();
        }

        if (opt->HasValue("height"))
        {
            y = opt->GetValue("height")->GetIntValue();
        }*/

        std::string filename = "screen.png";
        if (opt->HasValue("filename"))
            filename = opt->GetValue("filename")->GetStringValue().ToString();

        std::string mapId = "";
        if (opt->HasValue("id"))
          mapId = opt->GetValue("id")->GetStringValue().ToString();

        //CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
        //context->Enter();

        CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();

        CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("capture_screen");
        
        CefRefPtr<CefListValue> args = message->GetArgumentList();
        //args->SetSize(5);
        //args->SetInt(0, x);
        //args->SetInt(1, y);
        //args->SetInt(2, width);
        //args->SetInt(3, height);
        //args->SetString(4, filename.c_str());

        if (mapId.size() > 0)
        {
        args->SetSize(2);
        args->SetString(0, filename.c_str());
        args->SetString(1, mapId.c_str());
        }
        else
        {
          args->SetSize(1);
          args->SetString(0, filename.c_str());
        }

        browser->SendProcessMessage(PID_BROWSER, message);

        //context->Exit();


        //retval = CefV8Value::CreateObject(0);
        //retval->SetValue("tiles", CefV8Value::CreateUInt(est.getNumTiles()), V8_PROPERTY_ATTRIBUTE_NONE);
        //retval->SetValue("size", CefV8Value::CreateDouble(est.getSizeInMB()), V8_PROPERTY_ATTRIBUTE_NONE);
        //retval->SetValue("seconds", CefV8Value::CreateDouble(est.getTotalTimeInSeconds()), V8_PROPERTY_ATTRIBUTE_NONE);

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
        "    osgearth.captureScreen = function(options) {"
        "        native function ScreenCapture();"
        "        return ScreenCapture(options);"
        "    };"
        "})();";
}


void MapAPI::AddExtensions(CefRefPtr<CefV8Value> global)
{
    CefRefPtr< MapV8Handler > handler = new MapV8Handler();
    CefRegisterExtension("map", code, handler);
}
