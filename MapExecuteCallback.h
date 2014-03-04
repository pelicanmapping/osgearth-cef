#ifndef OSGEARTH_CEF_MAPEXECUTECALLBACK_H
#define OSGEARTH_CEF_MAPEXECUTECALLBACK_H

#include "ExecuteCallback.h"
#include "BrowserClient.h"

#include <osgearth/MapCallback>
#include <osgEarth/MapModelChange>

namespace osgEarth { namespace Cef
{

class MapExecuteCallback : public ExecuteCallback
{
public:
    MapExecuteCallback(BrowserClient* client) : _client(client) { }
    ReturnVal* execute( int64 queryId, const std::string& command, const JsonArguments &args, CefRefPtr<CefMessageRouterBrowserSide::Callback> persistentCallback );

private:
    CefRefPtr<BrowserClient> _client;

    struct MapQueryCallback: public osgEarth::MapCallback
    {
      std::string mapId;
      CefRefPtr<CefMessageRouterBrowserSide::Callback> callback;

      MapQueryCallback(const std::string& map_id, CefRefPtr<CefMessageRouterBrowserSide::Callback> queryCallback): mapId(map_id), callback(queryCallback) { }

      void onMapModelChanged( const osgEarth::MapModelChange& change );
    };

    std::map< int64, osg::ref_ptr<MapQueryCallback> > _mapCallbacks;
};

} }

#endif