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

        std::string filename = "screen.png";
        if (opt->HasValue("filename"))
            filename = opt->GetValue("filename")->GetStringValue().ToString();

        int width = -1;
        int height = -1;

        std::string mapId = "";
        if (opt->HasValue("id"))
        {
            mapId = opt->GetValue("id")->GetStringValue().ToString();

            if (opt->HasValue("width"))
                width = opt->GetValue("width")->GetIntValue();

            if (opt->HasValue("height"))
                height = opt->GetValue("height")->GetIntValue();
        }

        CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();

        CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("capture_screen");
        
        CefRefPtr<CefListValue> args = message->GetArgumentList();

        if (mapId.size() > 0)
        {
            if (width > 0 && height > 0)
            {
                args->SetSize(4);
                args->SetInt(2, width);
                args->SetInt(3, height);
            }
            else
            {
                args->SetSize(2);
            }

            args->SetString(0, filename.c_str());
            args->SetString(1, mapId.c_str());
        }
        else
        {
            args->SetSize(1);
            args->SetString(0, filename.c_str());
        }

        browser->SendProcessMessage(PID_BROWSER, message);

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
