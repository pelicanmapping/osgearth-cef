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
#include <osgEarth/Registry>
#include <osgEarthUtil/Controls>
#include <osgEarthDrivers/tms/TMSOptions>

#include <typeinfo>

#include "include/cef_app.h"

#include "CefHelper"
#include "BrowserClient"


#define LC "[osgearth_cef] "


using namespace osgEarth::Cef;
using namespace osgEarth::Util::Controls;

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

                osg::ref_ptr<osgEarth::MapNode> mapNode= _client->getMapNode(id);
                if (mapNode.valid())
                {
                    mapNode->getMap()->addImageLayer(layer);
                }
                else
                {
                    return new ExecuteCallback::ReturnVal("Failed to load layer, map node not found.", -1);
                }

                return new ExecuteCallback::ReturnVal();
            }

            OE_WARN << LC << "Execute: " << command << std::endl;
            return 0L;
        }
    };


    struct MyClickHandler : public ControlEventHandler
    {
        void onClick( Control* control, const osg::Vec2f& pos, int mouseButtonMask )
        {
            OE_NOTICE << "You clicked at (" << pos.x() << ", " << pos.y() << ") within the control."
                << std::endl;
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

        bool init = false;
        while (!browserClient->getViewer()->done())
        {
            browserClient->getViewer()->frame();
            CefDoMessageLoopWork();

            // Demo:
            // Once initialized, add a control to the default canvas
            if (!init)
            {
                // Have to wait until view is created
                osg::ref_ptr<osgViewer::View> view = browserClient->getMapView("map1");
                if (view.valid())
                {
                    // Get the default ControlCanvas
                    ControlCanvas* cs = ControlCanvas::get( view.get() );

                    HBox* box = new HBox();
                    box->setBorderColor( 1, 1, 1, 1 );
                    box->setBackColor( .6,.5,.4,0.5 );
                    box->setMargin( 40 );
                    box->setPadding( 10 );
                    box->setHorizAlign( Control::ALIGN_LEFT );
                    box->setVertAlign( Control::ALIGN_BOTTOM );

                    // Add a text label:
                    LabelControl* label = new LabelControl( "osgEarth Controls" );
                    label->setFont( osgEarth::Registry::instance()->getDefaultFont() );
                    label->setFontSize( 24.0f );
                    label->setHorizAlign( Control::ALIGN_LEFT );
                    label->setMargin( 5 );
                    box->addControl( label );

                    label = new LabelControl("Hover Me");
                    label->setMargin( 10 );
                    label->setBackColor( 1,1,1,0.4 );
                    box->addControl( label );
                    label->setActiveColor(1,.3,.3,1);
                    label->addEventHandler( new MyClickHandler );

                    // Add some checkboxes
                    HBox* c4 = new HBox();
                    c4->setChildSpacing( 5 );
                    {
                        c4->addControl( new CheckBoxControl( true ) );
                        c4->addControl( new LabelControl( "Checkbox" ) );
                    }
                    box->addControl( c4 );

                    cs->addControl(box);
                    
                    init = true;
                }
            }
        }
    }

    // Shutdown the CEF processes
    CefShutdown();

    return 0; 
}

