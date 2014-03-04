#ifndef OSGEARTH_CEF_OECEFRENDERPROCESSHANDLER_H
#define OSGEARTH_CEF_OECEFRENDERPROCESSHANDLER_H

#include "include/cef_app.h"
#include "include/wrapper/cef_message_router.h"

namespace osgEarth { namespace Cef
{

class RenderProcessHandler : public CefRenderProcessHandler//, public CefV8Handler
{
public:
    RenderProcessHandler();

    // CefRenderProcessHandler methods
    virtual void OnWebKitInitialized() override;

    virtual void OnContextCreated(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) override;

    virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) override;

    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) override;


    // CefV8Handler methods
    //virtual bool Execute(
    //    const CefString& name,
    //    CefRefPtr<CefV8Value> object,
    //    const CefV8ValueList& arguments,
    //    CefRefPtr<CefV8Value>& retval,
    //    CefString& exception
    //);

private:
    //void SetListValue(CefRefPtr<CefListValue> list, int index, CefRefPtr<CefV8Value> value);
    //void SetList(CefRefPtr<CefV8Value> source, CefRefPtr<CefListValue> target);

    CefRefPtr<CefMessageRouterRendererSide> _messageRouter;

    IMPLEMENT_REFCOUNTING(RenderProcessHandler);
};

} }

#endif
