#include "RenderProcessHandler"
#include "GDALLayer"
#include "PackagerExtensions"
#include "MapExtensions"

#include <osg/Camera>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>

#include <osgearth/Notify>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/ExampleResources>

using namespace osgEarth::Cef;


#define LC "[RenderProcessHandler] "


RenderProcessHandler::RenderProcessHandler()
{
    //nop
}

void RenderProcessHandler::OnWebKitInitialized()
{
    // Create the renderer-side router for query handling.
    CefMessageRouterConfig config;
    _messageRouter = CefMessageRouterRendererSide::Create(config);

    GDALAPI::AddGDALExtensions(0);
    PackagerAPI::AddExtensions(0);
    MapAPI::AddExtensions(0);
}

void RenderProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
    _messageRouter->OnContextCreated(browser,  frame, context);

    CefRefPtr<CefV8Value> global = context->GetGlobal();

    GDALAPI::AddGDALExtensions( global );

    // Create a basic osgearth object in the context if one does not already exist (via the inclusion of our osgearth.js library)
    //if (!global->HasValue("osgearth"))
    //{
    //    //OE_WARN << LC << "osgearth value not found, creating default object." << std::endl;

    //    CefRefPtr<CefV8Value> osgearthObj = CefV8Value::CreateObject(0);

    //    CefRefPtr<CefV8Value> createMap = CefV8Value::CreateFunction("createMap", this);
    //    osgearthObj->SetValue("createMap", createMap, V8_PROPERTY_ATTRIBUTE_READONLY);

    //    CefRefPtr<CefV8Value> setMap = CefV8Value::CreateFunction("setMap", this);
    //    osgearthObj->SetValue("setMap", setMap, V8_PROPERTY_ATTRIBUTE_READONLY);

    //    CefRefPtr<CefV8Value> execute = CefV8Value::CreateFunction("execute", this);
    //    osgearthObj->SetValue("execute", execute, V8_PROPERTY_ATTRIBUTE_READONLY);

    //    global->SetValue("osgearth", osgearthObj, V8_PROPERTY_ATTRIBUTE_READONLY);
    //}
}

void RenderProcessHandler::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
    _messageRouter->OnContextReleased(browser, frame, context);
}

bool RenderProcessHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    return _messageRouter->OnProcessMessageReceived(browser, source_process, message);
}

//bool RenderProcessHandler::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)
//{
//    // Create an inter-process message and fill it's arguments list
//    CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(name);
//
//    CefRefPtr<CefListValue> messageArgs = message->GetArgumentList();
//	  for (unsigned int i = 0; i < arguments.size(); i++)
//	      SetListValue(messageArgs, i, arguments[i]);
//
//
//    // Send the message to the browser
//    CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();
//    if (!browser.get())
//        return false;
//
//	  browser->SendProcessMessage(PID_BROWSER, message);
//
//
//    return true;
//}

//void RenderProcessHandler::SetListValue(CefRefPtr<CefListValue> list, int index, CefRefPtr<CefV8Value> value)
//{
//  if (value->IsArray()) {
//    CefRefPtr<CefListValue> new_list = CefListValue::Create();
//    SetList(value, new_list);
//    list->SetList(index, new_list);
//  } else if (value->IsString()) {
//    list->SetString(index, value->GetStringValue());
//  } else if (value->IsBool()) {
//    list->SetBool(index, value->GetBoolValue());
//  } else if (value->IsInt()) {
//    list->SetInt(index, value->GetIntValue());
//  } else if (value->IsDouble()) {
//    list->SetDouble(index, value->GetDoubleValue());
//  }
//}
//
//void RenderProcessHandler::SetList(CefRefPtr<CefV8Value> source, CefRefPtr<CefListValue> target)
//{
//  if (!source->IsArray())
//    return;
//
//  int arg_length = source->GetArrayLength();
//  if (arg_length == 0)
//    return;
//
//  // Start with null types in all spaces.
//  target->SetSize(arg_length);
//
//  for (int i = 0; i < arg_length; ++i)
//    SetListValue(target, i, source->GetValue(i));
//}
