#ifndef OSGEARTH_CEF_OECEFAPP_H
#define OSGEARTH_CEF_OECEFAPP_H

#include "include/cef_app.h"

#include "RenderProcessHandler.h"

namespace osgEarth { namespace Cef
{

class OECefApp : public CefApp
{
public:
  OECefApp() { _renderProcHandler = new RenderProcessHandler(); }

public:
  CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() { return _renderProcHandler; }

private:
  CefRefPtr<CefRenderProcessHandler> _renderProcHandler;

  IMPLEMENT_REFCOUNTING(OECefApp);
};

} }

#endif
