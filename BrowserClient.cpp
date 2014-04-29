#include "BrowserClient"
#include "KeyboardEventAdapter"
#include "MapExecuteCallback"

#include <osg/Camera>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>

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

        unsigned int getCefModifiers(int modKeyMask)
        {
            int modifiers = 0;
            if (modKeyMask & osgGA::GUIEventAdapter::MODKEY_CTRL)
              modifiers |= EVENTFLAG_CONTROL_DOWN;
            if (modKeyMask & osgGA::GUIEventAdapter::MODKEY_SHIFT)
              modifiers |= EVENTFLAG_SHIFT_DOWN;
            if (modKeyMask & osgGA::GUIEventAdapter::MODKEY_ALT)
              modifiers |= EVENTFLAG_ALT_DOWN;
            if (modKeyMask & osgGA::GUIEventAdapter::MODKEY_CAPS_LOCK)
              modifiers |= EVENTFLAG_CAPS_LOCK_ON;
            if (modKeyMask & osgGA::GUIEventAdapter::MODKEY_NUM_LOCK)
              modifiers |= EVENTFLAG_NUM_LOCK_ON;

            //if (modKeyMask & osgGA::GUIEventAdapter:)
            //  modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
            //if (modKeyMask & osgGA::GUIEventAdapter::MODKEY_CTRL)
            //  modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
            //if (modKeyMask & osgGA::GUIEventAdapter::MODKEY_CTRL)
            //  modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

            return modifiers;
        }

        bool confirmCharKey(int key)
        {
            switch (key)
            {
                case osgGA::GUIEventAdapter::KEY_Space:
                case osgGA::GUIEventAdapter::KEY_0:
                case osgGA::GUIEventAdapter::KEY_1:
                case osgGA::GUIEventAdapter::KEY_2:
                case osgGA::GUIEventAdapter::KEY_3:
                case osgGA::GUIEventAdapter::KEY_4:
                case osgGA::GUIEventAdapter::KEY_5:
                case osgGA::GUIEventAdapter::KEY_6:
                case osgGA::GUIEventAdapter::KEY_7:
                case osgGA::GUIEventAdapter::KEY_8:
                case osgGA::GUIEventAdapter::KEY_9:
                case osgGA::GUIEventAdapter::KEY_A:
                case osgGA::GUIEventAdapter::KEY_B:
                case osgGA::GUIEventAdapter::KEY_C:
                case osgGA::GUIEventAdapter::KEY_D:
                case osgGA::GUIEventAdapter::KEY_E:
                case osgGA::GUIEventAdapter::KEY_F:
                case osgGA::GUIEventAdapter::KEY_G:
                case osgGA::GUIEventAdapter::KEY_H:
                case osgGA::GUIEventAdapter::KEY_I:
                case osgGA::GUIEventAdapter::KEY_J:
                case osgGA::GUIEventAdapter::KEY_K:
                case osgGA::GUIEventAdapter::KEY_L:
                case osgGA::GUIEventAdapter::KEY_M:
                case osgGA::GUIEventAdapter::KEY_N:
                case osgGA::GUIEventAdapter::KEY_O:
                case osgGA::GUIEventAdapter::KEY_P:
                case osgGA::GUIEventAdapter::KEY_Q:
                case osgGA::GUIEventAdapter::KEY_R:
                case osgGA::GUIEventAdapter::KEY_S:
                case osgGA::GUIEventAdapter::KEY_T:
                case osgGA::GUIEventAdapter::KEY_U:
                case osgGA::GUIEventAdapter::KEY_V:
                case osgGA::GUIEventAdapter::KEY_W:
                case osgGA::GUIEventAdapter::KEY_X:
                case osgGA::GUIEventAdapter::KEY_Y:
                case osgGA::GUIEventAdapter::KEY_Z:

                case osgGA::GUIEventAdapter::KEY_Exclaim:
                case osgGA::GUIEventAdapter::KEY_Quotedbl:
                case osgGA::GUIEventAdapter::KEY_Hash:
                case osgGA::GUIEventAdapter::KEY_Dollar:
                case osgGA::GUIEventAdapter::KEY_Ampersand:
                case osgGA::GUIEventAdapter::KEY_Quote:
                case osgGA::GUIEventAdapter::KEY_Leftparen:
                case osgGA::GUIEventAdapter::KEY_Rightparen:
                case osgGA::GUIEventAdapter::KEY_Asterisk:
                case osgGA::GUIEventAdapter::KEY_Plus:
                case osgGA::GUIEventAdapter::KEY_Comma:
                case osgGA::GUIEventAdapter::KEY_Minus:
                case osgGA::GUIEventAdapter::KEY_Period:
                case osgGA::GUIEventAdapter::KEY_Slash:
                case osgGA::GUIEventAdapter::KEY_Colon:
                case osgGA::GUIEventAdapter::KEY_Semicolon:
                case osgGA::GUIEventAdapter::KEY_Less:
                case osgGA::GUIEventAdapter::KEY_Equals:
                case osgGA::GUIEventAdapter::KEY_Greater:
                case osgGA::GUIEventAdapter::KEY_Question:
                case osgGA::GUIEventAdapter::KEY_At:
                case osgGA::GUIEventAdapter::KEY_Leftbracket:
                case osgGA::GUIEventAdapter::KEY_Backslash:
                case osgGA::GUIEventAdapter::KEY_Rightbracket:
                case osgGA::GUIEventAdapter::KEY_Caret:
                case osgGA::GUIEventAdapter::KEY_Underscore:
                case osgGA::GUIEventAdapter::KEY_Backquote:

                //case osgGA::GUIEventAdapter::KEY_BackSpace:
                //case osgGA::GUIEventAdapter::KEY_Tab:
                //case osgGA::GUIEventAdapter::KEY_Linefeed:
                //case osgGA::GUIEventAdapter::KEY_Clear:
                //case osgGA::GUIEventAdapter::KEY_Return:
                //case osgGA::GUIEventAdapter::KEY_Pause:
                //case osgGA::GUIEventAdapter::KEY_Scroll_Lock:
                //case osgGA::GUIEventAdapter::KEY_Sys_Req:
                //case osgGA::GUIEventAdapter::KEY_Escape:
                //case osgGA::GUIEventAdapter::KEY_Delete:

                //case osgGA::GUIEventAdapter::KEY_Home:
                //case osgGA::GUIEventAdapter::KEY_Left:
                //case osgGA::GUIEventAdapter::KEY_Up:
                //case osgGA::GUIEventAdapter::KEY_Right:
                //case osgGA::GUIEventAdapter::KEY_Down:
                //case osgGA::GUIEventAdapter::KEY_Prior:
                //case osgGA::GUIEventAdapter::KEY_Page_Up:
                //case osgGA::GUIEventAdapter::KEY_Next:
                //case osgGA::GUIEventAdapter::KEY_Page_Down:
                //case osgGA::GUIEventAdapter::KEY_End:
                //case osgGA::GUIEventAdapter::KEY_Begin:

                //case osgGA::GUIEventAdapter::KEY_Select:
                //case osgGA::GUIEventAdapter::KEY_Print:
                //case osgGA::GUIEventAdapter::KEY_Execute:
                //case osgGA::GUIEventAdapter::KEY_Insert:
                //case osgGA::GUIEventAdapter::KEY_Undo:
                //case osgGA::GUIEventAdapter::KEY_Redo:
                //case osgGA::GUIEventAdapter::KEY_Menu:
                //case osgGA::GUIEventAdapter::KEY_Find:
                //case osgGA::GUIEventAdapter::KEY_Cancel:
                //case osgGA::GUIEventAdapter::KEY_Help:
                //case osgGA::GUIEventAdapter::KEY_Break:
                //case osgGA::GUIEventAdapter::KEY_Mode_switch:
                //case osgGA::GUIEventAdapter::KEY_Script_switch:
                //case osgGA::GUIEventAdapter::KEY_Num_Lock:

                case osgGA::GUIEventAdapter::KEY_KP_Space:
                //case osgGA::GUIEventAdapter::KEY_KP_Tab:
                //case osgGA::GUIEventAdapter::KEY_KP_Enter:
                //case osgGA::GUIEventAdapter::KEY_KP_F1:
                //case osgGA::GUIEventAdapter::KEY_KP_F2:
                //case osgGA::GUIEventAdapter::KEY_KP_F3:
                //case osgGA::GUIEventAdapter::KEY_KP_F4:
                //case osgGA::GUIEventAdapter::KEY_KP_Home:
                //case osgGA::GUIEventAdapter::KEY_KP_Left:
                //case osgGA::GUIEventAdapter::KEY_KP_Up:
                //case osgGA::GUIEventAdapter::KEY_KP_Right:
                //case osgGA::GUIEventAdapter::KEY_KP_Down:
                //case osgGA::GUIEventAdapter::KEY_KP_Prior:
                //case osgGA::GUIEventAdapter::KEY_KP_Page_Up:
                //case osgGA::GUIEventAdapter::KEY_KP_Next:
                //case osgGA::GUIEventAdapter::KEY_KP_Page_Down:
                //case osgGA::GUIEventAdapter::KEY_KP_End:
                //case osgGA::GUIEventAdapter::KEY_KP_Begin:
                //case osgGA::GUIEventAdapter::KEY_KP_Insert:
                //case osgGA::GUIEventAdapter::KEY_KP_Delete:
                case osgGA::GUIEventAdapter::KEY_KP_Equal:
                case osgGA::GUIEventAdapter::KEY_KP_Multiply:
                case osgGA::GUIEventAdapter::KEY_KP_Add:
                case osgGA::GUIEventAdapter::KEY_KP_Separator:
                case osgGA::GUIEventAdapter::KEY_KP_Subtract:
                case osgGA::GUIEventAdapter::KEY_KP_Decimal:
                case osgGA::GUIEventAdapter::KEY_KP_Divide:

                case osgGA::GUIEventAdapter::KEY_KP_0:
                case osgGA::GUIEventAdapter::KEY_KP_1:
                case osgGA::GUIEventAdapter::KEY_KP_2:
                case osgGA::GUIEventAdapter::KEY_KP_3:
                case osgGA::GUIEventAdapter::KEY_KP_4:
                case osgGA::GUIEventAdapter::KEY_KP_5:
                case osgGA::GUIEventAdapter::KEY_KP_6:
                case osgGA::GUIEventAdapter::KEY_KP_7:
                case osgGA::GUIEventAdapter::KEY_KP_8:
                case osgGA::GUIEventAdapter::KEY_KP_9:

                //case osgGA::GUIEventAdapter::KEY_F1:
                //case osgGA::GUIEventAdapter::KEY_F2:
                //case osgGA::GUIEventAdapter::KEY_F3:
                //case osgGA::GUIEventAdapter::KEY_F4:
                //case osgGA::GUIEventAdapter::KEY_F5:
                //case osgGA::GUIEventAdapter::KEY_F6:
                //case osgGA::GUIEventAdapter::KEY_F7:
                //case osgGA::GUIEventAdapter::KEY_F8:
                //case osgGA::GUIEventAdapter::KEY_F9:
                //case osgGA::GUIEventAdapter::KEY_F10:
                //case osgGA::GUIEventAdapter::KEY_F11:
                //case osgGA::GUIEventAdapter::KEY_F12:
                //case osgGA::GUIEventAdapter::KEY_F13:
                //case osgGA::GUIEventAdapter::KEY_F14:
                //case osgGA::GUIEventAdapter::KEY_F15:
                //case osgGA::GUIEventAdapter::KEY_F16:
                //case osgGA::GUIEventAdapter::KEY_F17:
                //case osgGA::GUIEventAdapter::KEY_F18:
                //case osgGA::GUIEventAdapter::KEY_F19:
                //case osgGA::GUIEventAdapter::KEY_F20:
                //case osgGA::GUIEventAdapter::KEY_F21:
                //case osgGA::GUIEventAdapter::KEY_F22:
                //case osgGA::GUIEventAdapter::KEY_F23:
                //case osgGA::GUIEventAdapter::KEY_F24:
                //case osgGA::GUIEventAdapter::KEY_F25:
                //case osgGA::GUIEventAdapter::KEY_F26:
                //case osgGA::GUIEventAdapter::KEY_F27:
                //case osgGA::GUIEventAdapter::KEY_F28:
                //case osgGA::GUIEventAdapter::KEY_F29:
                //case osgGA::GUIEventAdapter::KEY_F30:
                //case osgGA::GUIEventAdapter::KEY_F31:
                //case osgGA::GUIEventAdapter::KEY_F32:
                //case osgGA::GUIEventAdapter::KEY_F33:
                //case osgGA::GUIEventAdapter::KEY_F34:
                //case osgGA::GUIEventAdapter::KEY_F35:

                //case osgGA::GUIEventAdapter::KEY_Shift_L:
                //case osgGA::GUIEventAdapter::KEY_Shift_R:
                //case osgGA::GUIEventAdapter::KEY_Control_L:
                //case osgGA::GUIEventAdapter::KEY_Control_R:
                //case osgGA::GUIEventAdapter::KEY_Caps_Lock:
                //case osgGA::GUIEventAdapter::KEY_Shift_Lock:

                //case osgGA::GUIEventAdapter::KEY_Meta_L:
                //case osgGA::GUIEventAdapter::KEY_Meta_R:
                //case osgGA::GUIEventAdapter::KEY_Alt_L:
                //case osgGA::GUIEventAdapter::KEY_Alt_R:
                //case osgGA::GUIEventAdapter::KEY_Super_L:
                //case osgGA::GUIEventAdapter::KEY_Super_R:
                //case osgGA::GUIEventAdapter::KEY_Hyper_L:
                //case osgGA::GUIEventAdapter::KEY_Hyper_R:

                  return true;

                default:
                  return false;
            }
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

        bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
        {
            switch (ea.getEventType())
            {
                case osgGA::GUIEventAdapter::PUSH:
                    {
                        CefMouseEvent mouse_event;
                        mouse_event.x = (int)ea.getX();
                        mouse_event.y = ea.getWindowHeight() - (int)ea.getY();
                        mouse_event.modifiers = getCefModifiers(ea.getModKeyMask());
                        _browser->GetHost()->SendMouseClickEvent(mouse_event, getCefMouseButton(ea.getButton()), false, 1);
                    }
                    break;

                case osgGA::GUIEventAdapter::RELEASE:
                    {
                        CefMouseEvent mouse_event;
                        mouse_event.x = (int)ea.getX();
                        mouse_event.y = ea.getWindowHeight() - (int)ea.getY();
                        mouse_event.modifiers = getCefModifiers(ea.getModKeyMask());
                        _browser->GetHost()->SendMouseClickEvent(mouse_event, getCefMouseButton(ea.getButton()), true, 1);

                        _browser->GetHost()->SendFocusEvent(true);
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
                        CefKeyEvent key_event;

#ifdef WIN32
                        key_event.windows_key_code = _keyAdapter.remapKey(ea.getUnmodifiedKey());
#else
                        key_event.native_key_code = _keyAdapter.remapKey(ea.getUnmodifiedKey());
#endif

                        key_event.is_system_key = 0;
                        key_event.modifiers = getCefModifiers(ea.getModKeyMask());

                        key_event.type = KEYEVENT_RAWKEYDOWN;
                        _browser->GetHost()->SendKeyEvent(key_event);

                        if (confirmCharKey(ea.getUnmodifiedKey()))
                        {
                            key_event.type = KEYEVENT_CHAR;
                            key_event.windows_key_code = ea.getKey();

                            _browser->GetHost()->SendKeyEvent(key_event);
                        }
                    }
                    break;

                case osgGA::GUIEventAdapter::KEYUP:
                    {
                        CefKeyEvent key_event;
                        
#ifdef WIN32
                        key_event.windows_key_code = _keyAdapter.remapKey(ea.getUnmodifiedKey());
#else
                        key_event.native_key_code = _keyAdapter.remapKey(ea.getUnmodifiedKey());
#endif

                        key_event.is_system_key = 0;
                        key_event.modifiers = getCefModifiers(ea.getModKeyMask());

                        key_event.type = KEYEVENT_KEYUP;
                        _browser->GetHost()->SendKeyEvent(key_event);
                    }
                    break;

                case osgGA::GUIEventAdapter::MOVE:
                case osgGA::GUIEventAdapter::DRAG:
                    {
                        CefMouseEvent mouse_event;
                        mouse_event.x = (int)ea.getX();
                        mouse_event.y = ea.getWindowHeight() - (int)ea.getY();
                        mouse_event.modifiers = getCefModifiers(ea.getModKeyMask());
                        _browser->GetHost()->SendMouseMoveEvent(mouse_event, false);
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
                            mouse_event.modifiers = getCefModifiers(ea.getModKeyMask());
                            _browser->GetHost()->SendMouseWheelEvent(mouse_event, deltaX, deltaY);
                        }
                    }
                    break;

                case osgGA::GUIEventAdapter::RESIZE:
                    _browserClient->setSize(ea.getWindowWidth(), ea.getWindowHeight());
                    _browser->GetHost()->WasResized();
                    break;
            }

            //TODO: Return true if event occurred over non-transparent portion of main view
            return false;
        }

        osg::ref_ptr<osgViewer::View> _view;
        CefRefPtr<CefBrowser> _browser;
        CefRefPtr<BrowserClient> _browserClient;
        float _scrollFactor;
        KeyboardEventAdapter _keyAdapter;
    };
}


BrowserClient::BrowserClient(osgViewer::CompositeViewer* viewer, const std::string& url, int width, int height)
: _viewer(viewer), _width(width), _height(height)
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
    _mainView->addEventHandler(new BrowserEventHandler(_mainView.get(), this, _browser));
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
    geometry = new osg::Geometry();

    _popupVerts = new osg::Vec3Array();
    _popupVerts->reserve( 4 );
    _popupVerts->push_back( osg::Vec3(0,0,0));
    _popupVerts->push_back( osg::Vec3(1,0,0));
    _popupVerts->push_back( osg::Vec3(1,1,0));
    _popupVerts->push_back( osg::Vec3(0,1,0));
    geometry->setVertexArray( _popupVerts );

    colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
    geometry->setColorArray(colors);
    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    tcoords = new osg::Vec2Array(4);
    (*tcoords)[3].set(0.0f,0.0f);
    (*tcoords)[2].set(1.0f,0.0f);
    (*tcoords)[1].set(1.0f,1.0f);
    (*tcoords)[0].set(0.0f,1.0f);
    geometry->setTexCoordArray(0,tcoords);

    geometry->addPrimitiveSet( new osg::DrawArrays( GL_QUADS, 0, 4 ) );

    geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
    geometry->setDataVariance(osg::Object::DYNAMIC);

    _popupNode = new osg::Geode();
    _popupNode->addDrawable(geometry);
    _popupNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    _popupNode->getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    _popupNode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    _popupNode->setDataVariance(osg::Object::DYNAMIC);
    _popupNode->setNodeMask(0);

    modelViewMat->addChild(_popupNode);


    _mainView->setSceneData(modelViewMat);
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


//bool BrowserClient::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
//{
//  const osg::GraphicsContext::Traits* traits = _mainView->getCamera()->getGraphicsContext()->getTraits();
//  if (traits)
//  {
//      rect = CefRect(traits->x, traits->y, traits->width, traits->height);
//      return true;
//  }
//
//  return false;
//}

bool BrowserClient::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
    rect = CefRect(0, 0, _width, _height);
    return true;
}

//bool BrowserClient::GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY)
//{
//    CefRect rect;
//    if (GetRootScreenRect(browser, rect))
//    {
//        screenX = rect.x + viewX;
//        screenY = rect.y + viewY;
//        return true;
//    }
//
//    return false;
//}

//bool BrowserClient::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
//{
//    return false;
//}

void BrowserClient::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
    OE_DEBUG << LC << "OnPopupShow: " << (show ? "true" : "false") << std::endl;

    _popupNode->setNodeMask(show ? ~0 : 0);
}

void BrowserClient::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
    float x1 = (float)rect.x / (float)_width;
    float y1 = ((float)_height - (float)rect.y - (float)rect.height) / (float)_height;
    float x2 = ((float)rect.x + (float)rect.width) / (float)_width;
    float y2 = ((float)_height - (float)rect.y) / (float)_height;

    (*_popupVerts.get())[0].set(x1, y1, 0.1);
    (*_popupVerts.get())[1].set(x2, y1, 0.1);
    (*_popupVerts.get())[2].set(x2, y2, 0.1);
    (*_popupVerts.get())[3].set(x1, y2, 0.1);
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