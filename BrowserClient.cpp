#include "BrowserClient"
#include "KeyboardEventAdapter"
#include "MapExecuteCallback"

#include <osg/Camera>
#include <osg/TexMat>
#include <osg/TextureRectangle>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>

#include <osgearth/ImageUtils>
#include <osgearth/Notify>
#include <osgearth/StringUtils>
#include <osgEarthUtil/ExampleResources>

using namespace osgEarth;
using namespace osgEarth::Cef;


#define LC "[BrowserClient] "


namespace
{
    struct BrowserEventHandler : public osgGA::GUIEventHandler 
    {
        BrowserEventHandler( osgViewer::View* view, BrowserClient* browserClient, CefBrowser* browser )
          : _view(view), _browserClient(browserClient), _browser(browser)
        {
            //initialize _scrollFactor
            const char* sf = ::getenv("CEF_SCROLL_FACTOR");
            if ( sf )
                _scrollFactor = as<float>(sf, 0.0);
            else
                _scrollFactor = 70.0f;
        }

        CefBrowserHost::MouseButtonType getCefMouseButton(int button)
        {
          return button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ? MBT_LEFT : button == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON ? MBT_RIGHT : MBT_MIDDLE;
        }

        bool getScrollDeltas(const osgGA::GUIEventAdapter& ea, float &deltaX, float &deltaY)
        {
          if (ea.getScrollingDeltaX() != 0 || ea.getScrollingDeltaY() != 0)
          {
              deltaX = ea.getScrollingDeltaX();
              deltaY = ea.getScrollingDeltaY();
              return true;
          }

          deltaX = 0.0f;
          deltaY = 0.0f;

          switch (ea.getScrollingMotion())
          {
              case osgGA::GUIEventAdapter::SCROLL_UP:
                  deltaY = _scrollFactor;
                  break;

              case osgGA::GUIEventAdapter::SCROLL_DOWN:
                  deltaY = -_scrollFactor;
                  break;

              case osgGA::GUIEventAdapter::SCROLL_LEFT:
                  deltaX = -_scrollFactor;
                  break;

              case osgGA::GUIEventAdapter::SCROLL_RIGHT:
                  deltaX = _scrollFactor;
                  break;
          }

          return deltaX != 0.0f || deltaY != 0.0f;
        }

        bool transparentPixel(osgViewer::View* view, const osgGA::GUIEventAdapter& ea) const
        {
            osgUtil::LineSegmentIntersector::Intersections intersections;
            bool foundIntersection = view==0 ? false : view->computeIntersections(ea.getX(), ea.getY(), intersections);

            if (foundIntersection)
            {
                osg::Vec2 tc(0.5f,0.5f);

                // use the nearest intersection
                const osgUtil::LineSegmentIntersector::Intersection& intersection = *(intersections.begin());
                osg::Drawable* drawable = intersection.drawable.get();
                osg::Geometry* geometry = drawable ? drawable->asGeometry() : 0;
                osg::Vec3Array* vertices = geometry ? dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray()) : 0;
                if (vertices)
                {
                    // get the vertex indices.
                    const osgUtil::LineSegmentIntersector::Intersection::IndexList& indices = intersection.indexList;
                    const osgUtil::LineSegmentIntersector::Intersection::RatioList& ratios = intersection.ratioList;

                    if (indices.size()==3 && ratios.size()==3)
                    {
                        unsigned int i1 = indices[0];
                        unsigned int i2 = indices[1];
                        unsigned int i3 = indices[2];

                        float r1 = ratios[0];
                        float r2 = ratios[1];
                        float r3 = ratios[2];

                        osg::Array* texcoords = (geometry->getNumTexCoordArrays()>0) ? geometry->getTexCoordArray(0) : 0;
                        osg::Vec2Array* texcoords_Vec2Array = dynamic_cast<osg::Vec2Array*>(texcoords);
                        if (texcoords_Vec2Array)
                        {
                            // we have tex coord array so now we can compute the final tex coord at the point of intersection.
                            osg::Vec2 tc1 = (*texcoords_Vec2Array)[i1];
                            osg::Vec2 tc2 = (*texcoords_Vec2Array)[i2];
                            osg::Vec2 tc3 = (*texcoords_Vec2Array)[i3];
                            tc = tc1*r1 + tc2*r2 + tc3*r3;
                        }
                    }

                    osg::TexMat* activeTexMat = 0;
                    osg::Texture* activeTexture = 0;

                    if (drawable->getStateSet())
                    {
                        osg::TexMat* texMat = dynamic_cast<osg::TexMat*>(drawable->getStateSet()->getTextureAttribute(0,osg::StateAttribute::TEXMAT));
                        if (texMat) activeTexMat = texMat;

                        osg::Texture* texture = dynamic_cast<osg::Texture*>(drawable->getStateSet()->getTextureAttribute(0,osg::StateAttribute::TEXTURE));
                        if (texture) activeTexture = texture;
                    }

                    if (activeTexMat)
                    {
                        osg::Vec4 tc_transformed = osg::Vec4(tc.x(), tc.y(), 0.0f,0.0f) * activeTexMat->getMatrix();
                        tc.x() = tc_transformed.x();
                        tc.y() = tc_transformed.y();
                    }

                    osg::Image* image = _browserClient->getImage();
                    if (image)
                    {
                        int x = int( float(image->s()) * tc.x() );
                        int y = int( float(image->t()) * tc.y() );

                        ImageUtils::PixelReader ia(image);
                        osg::Vec4 color = ia(x, y);

                        return color.a() == 0.0;
                    }
                }
            }

            return false;
        }

        bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
        {
            
            switch(ea.getEventType())
            {
                case(osgGA::GUIEventAdapter::MOVE):
                case(osgGA::GUIEventAdapter::DRAG):
                case(osgGA::GUIEventAdapter::PUSH):
                case(osgGA::GUIEventAdapter::RELEASE):
                case(osgGA::GUIEventAdapter::DOUBLECLICK):
                case(osgGA::GUIEventAdapter::SCROLL):
                {
                    if (transparentPixel(_view, ea))
                    {
                        if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE || ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK)
                        {
                            _browser->GetHost()->SendFocusEvent(false);
                            _browserClient->setInFocus(false);
                        }

                        return false;
                    }
                    break;
                }
            }

            switch (ea.getEventType())
            {
                case(osgGA::GUIEventAdapter::DOUBLECLICK):
                    return true;

                case osgGA::GUIEventAdapter::PUSH:
                    {
                        CefMouseEvent mouse_event;
                        mouse_event.x = (int)ea.getX();
                        mouse_event.y = ea.getWindowHeight() - (int)ea.getY();
                        mouse_event.modifiers = _keyAdapter.getCefModifiers(ea.getModKeyMask());
                        _browser->GetHost()->SendMouseClickEvent(mouse_event, getCefMouseButton(ea.getButton()), false, 1);
                        return true;
                    }

                case osgGA::GUIEventAdapter::RELEASE:
                    {
                        CefMouseEvent mouse_event;
                        mouse_event.x = (int)ea.getX();
                        mouse_event.y = ea.getWindowHeight() - (int)ea.getY();
                        mouse_event.modifiers = _keyAdapter.getCefModifiers(ea.getModKeyMask());
                        _browser->GetHost()->SendMouseClickEvent(mouse_event, getCefMouseButton(ea.getButton()), true, 1);

                        _browser->GetHost()->SendFocusEvent(true);
                        _browserClient->setInFocus(true);
                    }
                    break;

                case osgGA::GUIEventAdapter::KEYDOWN:
                    if (ea.getKey() == osgGA::GUIEventAdapter::KEY_F12)
                    {
                        CefWindowInfo windowInfo;
                        windowInfo.SetAsPopup(0L, "DevTools");
                        CefBrowserSettings browserSettings;
                        _browser->GetHost()->ShowDevTools(windowInfo, _browserClient.get(), browserSettings);
                    }

                    {
                        // Down process keys if not in focus
                        if (!_browserClient->getInFocus())
                            return false;

                        CefKeyEvent key_event;

#ifdef WIN32
                        key_event.windows_key_code = _keyAdapter.remapKey(ea.getUnmodifiedKey());

                        OE_DEBUG << LC << "KEYDOWN: unmodified(" << ea.getUnmodifiedKey() << ")  modified(" << ea.getKey() << ")  remapped(" << key_event.windows_key_code << ")" << std::endl;
#else
                        key_event.native_key_code = _keyAdapter.remapKey(ea.getUnmodifiedKey());

                        OE_DEBUG << LC << "KEYDOWN: unmodified(" << ea.getUnmodifiedKey() << ")  modified(" << ea.getKey() << ")  remapped(" << key_event.native_key_code << ")" << std::endl;
#endif

                        key_event.is_system_key = 0;
                        key_event.modifiers = _keyAdapter.getCefModifiers(ea.getModKeyMask());

                        key_event.type = KEYEVENT_RAWKEYDOWN;
                        _browser->GetHost()->SendKeyEvent(key_event);

                        if (_keyAdapter.confirmCharKey(ea.getUnmodifiedKey()))
                        {
                            key_event.type = KEYEVENT_CHAR;
                            key_event.windows_key_code = ea.getKey();

                            _browser->GetHost()->SendKeyEvent(key_event);
                        }

                        return true;
                    }
                    break;

                case osgGA::GUIEventAdapter::KEYUP:
                    {
                        // Down process keys if not in focus
                        if (!_browserClient->getInFocus())
                            return false;

                        CefKeyEvent key_event;
                        
#ifdef WIN32
                        key_event.windows_key_code = _keyAdapter.remapKey(ea.getUnmodifiedKey());
#else
                        key_event.native_key_code = _keyAdapter.remapKey(ea.getUnmodifiedKey());
#endif

                        key_event.is_system_key = 0;
                        key_event.modifiers = _keyAdapter.getCefModifiers(ea.getModKeyMask());

                        key_event.type = KEYEVENT_KEYUP;
                        _browser->GetHost()->SendKeyEvent(key_event);

                        return true;
                    }
                    break;

                case osgGA::GUIEventAdapter::MOVE:
                case osgGA::GUIEventAdapter::DRAG:
                    {
                        CefMouseEvent mouse_event;
                        mouse_event.x = (int)ea.getX();
                        mouse_event.y = ea.getWindowHeight() - (int)ea.getY();
                        mouse_event.modifiers = _keyAdapter.getCefModifiers(ea.getModKeyMask());
                        _browser->GetHost()->SendMouseMoveEvent(mouse_event, false);
                        return true;
                    }
                    break;

                case osgGA::GUIEventAdapter::SCROLL:
                    {
                        float deltaX, deltaY;
                        if (getScrollDeltas(ea, deltaX, deltaY))
                        {
                            CefMouseEvent mouse_event;
                            mouse_event.x = (int)ea.getX();
                            mouse_event.y = ea.getWindowHeight() - (int)ea.getY();
                            mouse_event.modifiers = _keyAdapter.getCefModifiers(ea.getModKeyMask());
                            _browser->GetHost()->SendMouseWheelEvent(mouse_event, deltaX, deltaY);
                            return true;
                        }
                    }
                    break;

                case osgGA::GUIEventAdapter::RESIZE:
                    _browserClient->setSize(ea.getWindowWidth(), ea.getWindowHeight());
                    _browser->GetHost()->WasResized();
                    break;
            }

            return false;
        }

        osg::ref_ptr<osgViewer::View> _view;
        CefRefPtr<CefBrowser> _browser;
        CefRefPtr<BrowserClient> _browserClient;
        float _scrollFactor;
        KeyboardEventAdapter _keyAdapter;
    };


    struct MapProxyHandler : public osgGA::GUIEventHandler 
    {
        MapProxyHandler(osgGA::GUIEventHandler* handler)
        {
            _handler = handler;
        }

        bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
        {
            if (_handler.valid())
                return _handler->handle(ea, aa);
        
            return false;
        }

        osg::ref_ptr<osgGA::GUIEventHandler> _handler;
    };
}


BrowserClient::BrowserClient(osgViewer::CompositeViewer* viewer, const std::string& url, int width, int height)
: _viewer(viewer), _width(width), _height(height), _inFocus(true)
{
    initBrowser(url);
}

void BrowserClient::initBrowser(const std::string& url)
{
    //Setup our main view that will show the cef ui
    setupMainView( _width, _height );
    _viewer->addView( _mainView );

    {
        CefWindowInfo windowInfo;
        CefBrowserSettings browserSettings;

        // in linux set a gtk widget, in windows a hwnd. If not available set nullptr - may cause some render errors, in context-menu and plugins.
        windowInfo.SetAsOffScreen(0L);
        windowInfo.SetTransparentPainting(true);
        //windowInfo.SetAsWindowless(0L, true);

        _browser = CefBrowserHost::CreateBrowserSync(windowInfo, this, url.c_str(), browserSettings, 0L);
        _browser->GetHost()->SendFocusEvent(true);
    }

    addExecuteCallback(new MapExecuteCallback(this));

    _mainEventHandler = new BrowserEventHandler(_mainView.get(), this, _browser);
    _mainView->addEventHandler(_mainEventHandler);
}

void BrowserClient::setupMainView(unsigned int width, unsigned int height)
{
    //setup view
    _mainView = new osgViewer::View();
    _mainView->setUpViewInWindow( 50, 50, width, height );
    _mainView->getCamera()->setRenderOrder(osg::Camera::POST_RENDER);
    _mainView->getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT);
    _mainView->getCamera()->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
    _mainView->getCamera()->setProjectionResizePolicy(osg::Camera::FIXED);
    _mainView->getCamera()->setViewMatrix(osg::Matrix::identity());
    _mainView->getCamera()->setClearColor( osg::Vec4( 1., 0., 1., 0. ) );

    //create image and texture to render to
    _image = new osg::Image();
    osg::PixelBufferObject* pbo = new osg::PixelBufferObject(_image.get());
    pbo->setCopyDataAndReleaseGLBufferObject(true);
    pbo->setUsage(GL_DYNAMIC_DRAW_ARB);
    _image->setPixelBufferObject(pbo);

    osg::Texture2D* tex = new osg::Texture2D(_image);
    tex->setResizeNonPowerOfTwoHint(false);
    tex->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
    tex->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
    tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

    //setup quad to render on
    osg::Geometry* geometry = new osg::Geometry();

    osg::Vec3Array* verts = new osg::Vec3Array();
    verts->reserve( 4 );
    verts->push_back( osg::Vec3(0,0,0));
    verts->push_back( osg::Vec3(1,0,0));
    verts->push_back( osg::Vec3(1,1,0));
    verts->push_back( osg::Vec3(0,1,0));
    geometry->setVertexArray( verts );

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
    geometry->setColorArray(colors);
    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    osg::Vec2Array* tcoords = new osg::Vec2Array(4);
    (*tcoords)[3].set(0.0f,0.0f);
    (*tcoords)[2].set(1.0f,0.0f);
    (*tcoords)[1].set(1.0f,1.0f);
    (*tcoords)[0].set(0.0f,1.0f);
    geometry->setTexCoordArray(0,tcoords);

    geometry->addPrimitiveSet( new osg::DrawArrays( GL_QUADS, 0, 4 ) );

    geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
    geometry->setDataVariance(osg::Object::DYNAMIC);
    geometry->setName("CEFGeometry");

    osg::Geode* geode = new osg::Geode;
    geode->addDrawable(geometry);
    geode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    geode->getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    geode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    geode->setDataVariance(osg::Object::DYNAMIC);

    osg::MatrixTransform* modelViewMat = new osg::MatrixTransform;
    //modelViewMat->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    modelViewMat->setMatrix(osg::Matrix::identity());
    modelViewMat->addChild(geode);

    
    // setup popup
    _popupImage = new osg::Image();
    pbo = new osg::PixelBufferObject(_popupImage.get());
    pbo->setCopyDataAndReleaseGLBufferObject(true);
    pbo->setUsage(GL_DYNAMIC_DRAW_ARB);
    _popupImage->setPixelBufferObject(pbo);

    tex = new osg::Texture2D(_popupImage);
    tex->setResizeNonPowerOfTwoHint(false);
    tex->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
    tex->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
    tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

    //setup quad to render on
    _popupGeom = new osg::Geometry();

    _popupVerts = new osg::Vec3Array();
    _popupVerts->reserve( 4 );
    _popupVerts->push_back( osg::Vec3(0,0,0));
    _popupVerts->push_back( osg::Vec3(1,0,0));
    _popupVerts->push_back( osg::Vec3(1,1,0));
    _popupVerts->push_back( osg::Vec3(0,1,0));
    _popupGeom->setVertexArray( _popupVerts );

    colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
    _popupGeom->setColorArray(colors);
    _popupGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

    tcoords = new osg::Vec2Array(4);
    (*tcoords)[3].set(0.0f,0.0f);
    (*tcoords)[2].set(1.0f,0.0f);
    (*tcoords)[1].set(1.0f,1.0f);
    (*tcoords)[0].set(0.0f,1.0f);
    _popupGeom->setTexCoordArray(0,tcoords);

    _popupGeom->addPrimitiveSet( new osg::DrawArrays( GL_QUADS, 0, 4 ) );

    _popupGeom->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
    _popupGeom->setDataVariance(osg::Object::DYNAMIC);

    _popupNode = new osg::Geode();
    _popupNode->addDrawable(_popupGeom);
    _popupNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    _popupNode->getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    _popupNode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    _popupNode->setDataVariance(osg::Object::DYNAMIC);
    _popupNode->setNodeMask(0);

    modelViewMat->addChild(_popupNode);


    _mainView->setSceneData(modelViewMat);
}

void BrowserClient::addMapView(const std::string &id, osgViewer::View *mapView)
{
    std::map<std::string, osg::ref_ptr<osgViewer::View>>::iterator it = _mapViews.find(id);
    if (it != _mapViews.end())
    {
        // Remove event handlers if replacing a view
        it->second->getEventHandlers().clear();
    }

    mapView->addEventHandler(new MapProxyHandler(_mainEventHandler));
    _mapViews[id] = mapView;
    _viewer->addView( mapView );
}

osgViewer::View* BrowserClient::getMapView(const std::string& name)
{
  std::map<std::string, osg::ref_ptr<osgViewer::View>>::iterator it = _mapViews.find(name);
  if (it != _mapViews.end())
      return it->second;
  else
      return 0L;
}

MapNode* BrowserClient::getMapNode(const std::string& name)
{
    std::map<std::string, osg::ref_ptr<MapNode>>::iterator it = _mapNodes.find(name);
    if (it != _mapNodes.end())
        return it->second;
    else
        return 0L;
}

void BrowserClient::setSize(unsigned int width, unsigned int height)
{
    _width = osg::maximum(0U, width);
    _height = osg::maximum(0U, height);
}

void BrowserClient::addExecuteCallback( ExecuteCallback* callback )
{
    if ( callback )
        _callbacks.push_back( callback );
}

bool BrowserClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                             CefProcessId source_process,
                                             CefRefPtr<CefProcessMessage> message)
{
    if (_messageRouter->OnProcessMessageReceived(browser, source_process, message))
        return true;

    //std::string name = message->GetName();
    //CefRefPtr<CefListValue> arguments = message->GetArgumentList();

    //OE_DEBUG << LC << "Process message received on browser client: " << name << std::endl;

    //if (name == "createMap")  // osgearth.createMap(string id, int x, int y, int width, int height, (optional)string earthfile)
    //{
    //    if (arguments->GetSize() < 5)
    //    {
    //        //TODO: error msg
    //        return false;
    //    }
    //    else
    //    {
    //        std::string name = arguments->GetString(0).ToString();

    //        if (name.length() == 0)
    //        {
    //            //TODO: error msg
    //            return false;
    //        }

    //        if (_mapViews.find(name) != _mapViews.end())
    //        {
    //            //TODO: error msg
    //            return false;
    //        }

    //        int x = arguments->GetInt(1);
    //        int y = arguments->GetInt(2);
    //        int width = arguments->GetInt(3);
    //        int height = arguments->GetInt(4);

    //        OE_DEBUG << LC << "createMap(" << name << "): at " << x << ", " << y <<  "  size " << width << " x " << height << std::endl;

    //        osgViewer::View* mapView = new osgViewer::View();
    //        mapView->getCamera()->setGraphicsContext(_viewer->getView(0)->getCamera()->getGraphicsContext());
    //        mapView->getCamera()->setNearFarRatio(0.00002);
    //        mapView->getCamera()->setViewport( x, y, width, height);
    //        mapView->getCamera()->setRenderOrder(osg::Camera::PRE_RENDER);
    //        mapView->setCameraManipulator( new osgEarth::Util::EarthManipulator() );    
    //        mapView->getCamera()->setProjectionMatrixAsPerspective(30.0, double(width) / double(height), 1.0, 1000.0);

    //        _mapViews[name] = mapView;

    //        _viewer->addView( mapView );

    //        if (arguments->GetSize() >= 6)
    //        {
    //            std::string mapFile = arguments->GetString(5).ToString();

    //            osg::Node* node = osgDB::readNodeFile(mapFile);
    //            if (node)
    //            {
    //              mapView->setSceneData(node);
    //              _mapNodes[name] = MapNode::findMapNode(node);
    //            }
    //            else
    //            {
    //                _mapNodes[name] = 0L;
    //            }
    //        }
    //        else
    //        {
    //            _mapNodes[name] = 0L;
    //        }

    //        return true;
    //    }
    //}
    //else if (name == "setMap")  // osgearth.setMap(string id, int x, int y, int width, int height)
    //{
    //    if (arguments->GetSize() < 4)
    //    {
    //        //TODO: warning
    //        return false;
    //    }
    //    else
    //    {
    //        std::string name = arguments->GetString(0).ToString();

    //        if (name.length() == 0 || _mapViews.find(name) == _mapViews.end())
    //        {
    //            //TODO: error msg
    //            return false;
    //        }

    //        int x = arguments->GetInt(1);
    //        int y = arguments->GetInt(2);
    //        int width = arguments->GetInt(3);
    //        int height = arguments->GetInt(4);

    //        OE_DEBUG << LC << "setMap(" << name << "): at " << x << ", " << y <<  "  size " << width << " x " << height << std::endl;

    //        osg::ref_ptr<osgViewer::View> mapView = _mapViews[name];

    //        if (mapView.valid())
    //        {
    //            mapView->getCamera()->setViewport( x, y, width, height);
    //            mapView->getCamera()->setProjectionMatrixAsPerspective(30.0, double(width) / double(height), 1.0, 1000.0);
    //        }

    //        return true;
    //    }
    //}
    ///*else if (name == "execute")
    //{
    //    bool handled = false;

    //    if (arguments->GetSize() > 0)
    //    {
    //        std::string execName = arguments->GetString(0).ToString();

    //        for( ExecuteCallbacks::iterator i = _callbacks.begin(); i != _callbacks.end(); )
    //        {
    //            if ( i->valid() )
    //            {
    //                if (i->get()->execute(execName, arguments))
    //                {
    //                    handled = true;
    //                    break;
    //                }

    //                ++i;
    //            }
    //            else
    //            {
    //                i = _callbacks.erase( i );
    //            }
    //        }

    //        if (!handled)
    //            OE_DEBUG << LC << "Unhanlded execute command:  " << execName << std::endl;
    //    }

    //    return handled;
    //}*/
    ////else
    ////{
    ////}

    return false;
}


bool BrowserClient::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
  osgViewer::Viewer::Windows windows;
  _viewer->getWindows(windows);
  if (windows.size() > 0)
  {
      int x, y, width, height;
      windows[0]->getWindowRectangle(x, y, width, height);
      rect = CefRect(x, y, width, height);

      return true;
  }

  return false;
}

bool BrowserClient::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
    rect = CefRect(0, 0, _width, _height);
    return true;
}

bool BrowserClient::GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY)
{
    const osg::GraphicsContext::Traits* traits = _mainView->getCamera()->getGraphicsContext()->getTraits();
    if (traits)
    {
        screenX = traits->x + viewX;
        screenY = traits->y + viewY;
        return true;
    }

    return false;
}

//bool BrowserClient::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
//{
//    return false;
//}

void BrowserClient::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
    _popupNode->setNodeMask(show ? ~0 : 0);
}

void BrowserClient::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
    if (rect.width <= 0 || rect.height <= 0)
    {
        return;
    }

    float x1 = (float)rect.x / (float)_width;
    float y1 = ((float)_height - (float)rect.y - (float)rect.height) / (float)_height;
    float x2 = ((float)rect.x + (float)rect.width) / (float)_width;
    float y2 = ((float)_height - (float)rect.y) / (float)_height;

    (*_popupVerts.get())[0].set(x1, y1, 0.1);
    (*_popupVerts.get())[1].set(x2, y1, 0.1);
    (*_popupVerts.get())[2].set(x2, y2, 0.1);
    (*_popupVerts.get())[3].set(x1, y2, 0.1);

    _popupGeom->dirtyDisplayList();
}  

void BrowserClient::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
{
    if (type == PET_VIEW && dirtyRects.size() > 0)
    {
        _image->setImage( width, height, 1, 4, GL_BGRA, GL_UNSIGNED_BYTE, (unsigned char*)(buffer), osg::Image::NO_DELETE );
    }
    else if (type == PET_POPUP)
    {
        _popupImage->setImage( width, height, 1, 4, GL_BGRA, GL_UNSIGNED_BYTE, (unsigned char*)(buffer), osg::Image::NO_DELETE );
    }
}

//void BrowserClient::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor)
//{
//}


bool BrowserClient::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefRequest> request,
                                      bool is_redirect)
{
    _messageRouter->OnBeforeBrowse(browser, frame);
    return false;
}

void BrowserClient::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status)
{
    _messageRouter->OnRenderProcessTerminated(browser);
}


bool BrowserClient::OnBeforePopup(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  const CefString& target_url,
                                  const CefString& target_frame_name,
                                  const CefPopupFeatures& popupFeatures,
                                  CefWindowInfo& windowInfo,
                                  CefRefPtr<CefClient>& client,
                                  CefBrowserSettings& settings,
                                  bool* no_javascript_access) {
  //if (browser->GetHost()->IsWindowRenderingDisabled()) {
  //  // Cancel popups in off-screen rendering mode.
  //  return true;
  //}
  return false;
}

void BrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  //REQUIRE_UI_THREAD();

  if (!_messageRouter) {
    // Create the browser-side router for query handling.
    CefMessageRouterConfig config;
    _messageRouter = CefMessageRouterBrowserSide::Create(config);
    _messageRouter->AddHandler(this, true);

    // Register handlers with the router.
    //CreateMessageHandlers(message_handler_set_);
    //MessageHandlerSet::const_iterator it = message_handler_set_.begin();
    //for (; it != message_handler_set_.end(); ++it)
    //  message_router_->AddHandler(*(it), false);
  }

  //// Disable mouse cursor change if requested via the command-line flag.
  //if (m_bMouseCursorChangeDisabled)
  //  browser->GetHost()->SetMouseCursorChangeDisabled(true);

  //AutoLock lock_scope(this);
  //if (!m_Browser.get())   {
  //  // We need to keep the main child window, but not popup windows
  //  m_Browser = browser;
  //  m_BrowserId = browser->GetIdentifier();
  //} else if (browser->IsPopup()) {
  //  // Add to the list of popup browsers.
  //  m_PopupBrowsers.push_back(browser);
  //}

  //m_BrowserCount++;
}

bool BrowserClient::DoClose(CefRefPtr<CefBrowser> browser) {
  //REQUIRE_UI_THREAD();

  // Closing the main window requires special handling. See the DoClose()
  // documentation in the CEF header for a detailed destription of this
  // process.
  if (_browser->GetIdentifier() == browser->GetIdentifier()) {
    // Notify the browser that the parent window is about to close.
    browser->GetHost()->ParentWindowWillClose();

    // Set a flag to indicate that the window close should be allowed.
    //m_bIsClosing = true;
  }

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void BrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  //REQUIRE_UI_THREAD();

  _messageRouter->OnBeforeClose(browser);

  if (_browser->GetIdentifier() == browser->GetIdentifier()) {
    // Free the browser pointer so that the browser can be destroyed
    _browser = NULL;
    _messageRouter = NULL;

    //if (m_OSRHandler.get()) {
    //  m_OSRHandler->OnBeforeClose(browser);
    //  m_OSRHandler = NULL;
    //}
  //} else if (browser->IsPopup()) {
  //  // Remove from the browser popup list.
  //  BrowserList::iterator bit = m_PopupBrowsers.begin();
  //  for (; bit != m_PopupBrowsers.end(); ++bit) {
  //    if ((*bit)->IsSame(browser)) {
  //      m_PopupBrowsers.erase(bit);
  //      break;
  //    }
  //  }
  }

  //if (--m_BrowserCount == 0) {
    // All browser windows have closed.
    // Remove and delete message router handlers.
    //MessageHandlerSet::const_iterator it = message_handler_set_.begin();
    //for (; it != message_handler_set_.end(); ++it) {
    //  message_router_->RemoveHandler(*(it));
    //  delete *(it);
    //}
    //message_handler_set_.clear();
    //_messageRouter = NULL;
  //}
}


bool BrowserClient::OnQuery(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               int64 query_id,
                               const CefString& request,
                               bool persistent,
                               CefRefPtr<Callback> callback)
{
  // Parse JSON args
  std::string requestStr = request;
  OE_DEBUG << LC << "Query received: " << requestStr << std::endl;

  JsonArguments args(request);
  std::string command = args["command"];


  // Pass to callbacks to handle
  bool handled = false;
  if (!command.empty())
  {
    for( ExecuteCallbacks::iterator i = _callbacks.begin(); i != _callbacks.end(); )
    {
        if ( i->valid() )
        {
            osg::ref_ptr<ExecuteCallback::ReturnVal> ret = i->get()->execute(query_id, command, args, persistent ? callback : 0L);
            if (ret.valid())
            {
                handled = true;

                if (ret->errorCode == 0)
                {
                    callback->Success(ret->value);
                }
                else
                {
                    callback->Failure(ret->errorCode, ret->value);
                }

                break;
            }

            ++i;
        }
        else
        {
            i = _callbacks.erase( i );
        }
    }
  }


  // If unhandled call Failure callback
  if (!handled)
  {
      std::string result = "No handler found for query.";
      callback->Failure(404, result);
  }

  return true;
}

void BrowserClient::OnQueryCanceled(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id)
{
    for( ExecuteCallbacks::iterator i = _callbacks.begin(); i != _callbacks.end(); )
    {
        if ( i->valid() )
        {
            if (i->get()->cancel(query_id))
                break;

            ++i;
        }
        else
        {
            i = _callbacks.erase( i );
        }
    }
}