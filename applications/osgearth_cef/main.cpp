/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
* Copyright 2008-2013 Pelican Mapping
* http://osgearth.org
*
* osgEarth is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include <osg/Notify>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>

#include <osgEarth/ImageLayer>
#include <osgEarthDrivers/tms/TMSOptions>

#include "include/cef_app.h"

#include "CefHelper"
#include "BrowserClient"


#define LC "[osgearth_cef] "


using namespace osgEarth::Cef;


namespace
{
    struct MyExecCallback : public ExecuteCallback
    {
        CefRefPtr<BrowserClient> _client;

        MyExecCallback(BrowserClient* client) : _client(client) { }

        ExecuteCallback::ReturnVal* execute( int64 query_id, const std::string& command, const JsonArguments &args, CefRefPtr<CefMessageRouterBrowserSide::Callback> persistentCallback )
        {
            if (command == "says")
            {
              std::string animal = args["animal"];
              std::string sound = args["sound"];

              return new ExecuteCallback::ReturnVal("The " + animal + " says " + sound);
            }
            else if (command == "addTestLayer")
            {
                std::string id = args["id"];

                osgEarth::Drivers::TMSOptions tms;
                tms.url() = "http://readymap.org:8080/readymap/tiles/1.0.0/76/";
                osgEarth::ImageLayer* layer = new osgEarth::ImageLayer( "Test Layer", tms );

                _client->getMapNode(id)->getMap()->addImageLayer(layer);

                return new ExecuteCallback::ReturnVal();
            }

            OE_WARN << LC << "Execute: " << command << std::endl;
            return 0L;
        }
    };
}


int main(int argc, char** argv)
{
    osg::ArgumentParser arguments(&argc,argv);

    // Call load to initialize CEF and create the BrowserClient
    CefRefPtr<BrowserClient> browserClient = CefHelper().load(arguments);

    // Start main loop
    if (browserClient)
    {
        //TEST
        browserClient->addExecuteCallback(new MyExecCallback(browserClient.get()));

        while (!browserClient->getViewer()->done())
        {
            browserClient->getViewer()->frame();
            CefDoMessageLoopWork();
        }
    }

    // Shutdown the CEF processes
    CefShutdown();

    return 0; 
}
