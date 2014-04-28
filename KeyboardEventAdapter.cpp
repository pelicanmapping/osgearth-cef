#include "KeyboardEventAdapter"

#ifdef WIN32
  #include <windows.h>
#endif


using namespace osgEarth;
using namespace osgEarth::Cef;


#define LC "[KeyboardEventAdapter] "


    KeyboardEventAdapter::KeyboardEventAdapter()
    {

#ifdef WIN32
            _keymap[osgGA::GUIEventAdapter::KEY_Escape] = VK_ESCAPE;
            _keymap[osgGA::GUIEventAdapter::KEY_F1] = VK_F1;
            _keymap[osgGA::GUIEventAdapter::KEY_F2] = VK_F2;
            _keymap[osgGA::GUIEventAdapter::KEY_F3] = VK_F3;
            _keymap[osgGA::GUIEventAdapter::KEY_F4] = VK_F4;
            _keymap[osgGA::GUIEventAdapter::KEY_F5] = VK_F5;
            _keymap[osgGA::GUIEventAdapter::KEY_F6] = VK_F6;
            _keymap[osgGA::GUIEventAdapter::KEY_F7] = VK_F7;
            _keymap[osgGA::GUIEventAdapter::KEY_F8] = VK_F8;
            _keymap[osgGA::GUIEventAdapter::KEY_F9] = VK_F9;
            _keymap[osgGA::GUIEventAdapter::KEY_F10] = VK_F10;
            _keymap[osgGA::GUIEventAdapter::KEY_F11] = VK_F11;
            _keymap[osgGA::GUIEventAdapter::KEY_F12] = VK_F12;
            _keymap[osgGA::GUIEventAdapter::KEY_Backquote] = 0xc0;
            _keymap[osgGA::GUIEventAdapter::KEY_0] = '0';
            _keymap[osgGA::GUIEventAdapter::KEY_1] = '1';
            _keymap[osgGA::GUIEventAdapter::KEY_2] = '2';
            _keymap[osgGA::GUIEventAdapter::KEY_3] = '3';
            _keymap[osgGA::GUIEventAdapter::KEY_4] = '4';
            _keymap[osgGA::GUIEventAdapter::KEY_5] = '5';
            _keymap[osgGA::GUIEventAdapter::KEY_6] = '6';
            _keymap[osgGA::GUIEventAdapter::KEY_7] = '7';
            _keymap[osgGA::GUIEventAdapter::KEY_8] = '8';
            _keymap[osgGA::GUIEventAdapter::KEY_9] = '9';
            _keymap[osgGA::GUIEventAdapter::KEY_Minus] = 0xbd;
            _keymap[osgGA::GUIEventAdapter::KEY_Equals] = 0xbb;
            _keymap[osgGA::GUIEventAdapter::KEY_BackSpace] = VK_BACK;
            _keymap[osgGA::GUIEventAdapter::KEY_Tab] = VK_TAB;
            _keymap[osgGA::GUIEventAdapter::KEY_A] = 'A';
            _keymap[osgGA::GUIEventAdapter::KEY_B] = 'B';
            _keymap[osgGA::GUIEventAdapter::KEY_C] = 'C';
            _keymap[osgGA::GUIEventAdapter::KEY_D] = 'D';
            _keymap[osgGA::GUIEventAdapter::KEY_E] = 'E';
            _keymap[osgGA::GUIEventAdapter::KEY_F] = 'F';
            _keymap[osgGA::GUIEventAdapter::KEY_G] = 'G';
            _keymap[osgGA::GUIEventAdapter::KEY_H] = 'H';
            _keymap[osgGA::GUIEventAdapter::KEY_I] = 'I';
            _keymap[osgGA::GUIEventAdapter::KEY_J] = 'J';
            _keymap[osgGA::GUIEventAdapter::KEY_K] = 'K';
            _keymap[osgGA::GUIEventAdapter::KEY_L] = 'L';
            _keymap[osgGA::GUIEventAdapter::KEY_M] = 'M';
            _keymap[osgGA::GUIEventAdapter::KEY_N] = 'N';
            _keymap[osgGA::GUIEventAdapter::KEY_O] = 'O';
            _keymap[osgGA::GUIEventAdapter::KEY_P] = 'P';
            _keymap[osgGA::GUIEventAdapter::KEY_Q] = 'Q';
            _keymap[osgGA::GUIEventAdapter::KEY_R] = 'R';
            _keymap[osgGA::GUIEventAdapter::KEY_S] = 'S';
            _keymap[osgGA::GUIEventAdapter::KEY_T] = 'T';
            _keymap[osgGA::GUIEventAdapter::KEY_U] = 'U';
            _keymap[osgGA::GUIEventAdapter::KEY_V] = 'V';
            _keymap[osgGA::GUIEventAdapter::KEY_W] = 'W';
            _keymap[osgGA::GUIEventAdapter::KEY_X] = 'X';
            _keymap[osgGA::GUIEventAdapter::KEY_Y] = 'Y';
            _keymap[osgGA::GUIEventAdapter::KEY_Z] = 'Z';
            _keymap[osgGA::GUIEventAdapter::KEY_Leftbracket] = 0xdb;
            _keymap[osgGA::GUIEventAdapter::KEY_Rightbracket] = 0xdd;
            _keymap[osgGA::GUIEventAdapter::KEY_Backslash] = 0xdc;
            _keymap[osgGA::GUIEventAdapter::KEY_Caps_Lock] = VK_CAPITAL;
            _keymap[osgGA::GUIEventAdapter::KEY_Semicolon] = 0xba;
            _keymap[osgGA::GUIEventAdapter::KEY_Quote] = 0xde;
            _keymap[osgGA::GUIEventAdapter::KEY_Return] = VK_RETURN;
            _keymap[osgGA::GUIEventAdapter::KEY_Shift_L] = VK_LSHIFT;
            _keymap[osgGA::GUIEventAdapter::KEY_Comma] = 0xbc;
            _keymap[osgGA::GUIEventAdapter::KEY_Period] = 0xbe;
            _keymap[osgGA::GUIEventAdapter::KEY_Slash] = 0xbf;
            _keymap[osgGA::GUIEventAdapter::KEY_Shift_R] = VK_RSHIFT;
            _keymap[osgGA::GUIEventAdapter::KEY_Control_L] = VK_LCONTROL;
            _keymap[osgGA::GUIEventAdapter::KEY_Super_L] = VK_LWIN;
            _keymap[osgGA::GUIEventAdapter::KEY_Space] = VK_SPACE;
            _keymap[osgGA::GUIEventAdapter::KEY_Alt_L] = VK_LMENU;
            _keymap[osgGA::GUIEventAdapter::KEY_Alt_R] = VK_RMENU;
            _keymap[osgGA::GUIEventAdapter::KEY_Super_R] = VK_RWIN;
            _keymap[osgGA::GUIEventAdapter::KEY_Menu] = VK_APPS;
            _keymap[osgGA::GUIEventAdapter::KEY_Control_R] = VK_RCONTROL;
            _keymap[osgGA::GUIEventAdapter::KEY_Print] = VK_SNAPSHOT;
            _keymap[osgGA::GUIEventAdapter::KEY_Scroll_Lock] = VK_SCROLL;
            _keymap[osgGA::GUIEventAdapter::KEY_Pause] = VK_PAUSE;
            _keymap[osgGA::GUIEventAdapter::KEY_Home] = VK_HOME;
            _keymap[osgGA::GUIEventAdapter::KEY_Page_Up] = VK_PRIOR;
            _keymap[osgGA::GUIEventAdapter::KEY_End] = VK_END;
            _keymap[osgGA::GUIEventAdapter::KEY_Page_Down] = VK_NEXT;
            _keymap[osgGA::GUIEventAdapter::KEY_Delete] = VK_DELETE;
            _keymap[osgGA::GUIEventAdapter::KEY_Insert] = VK_INSERT;
            _keymap[osgGA::GUIEventAdapter::KEY_Left] = VK_LEFT;
            _keymap[osgGA::GUIEventAdapter::KEY_Up] = VK_UP;
            _keymap[osgGA::GUIEventAdapter::KEY_Right] = VK_RIGHT;
            _keymap[osgGA::GUIEventAdapter::KEY_Down] = VK_DOWN;
            _keymap[osgGA::GUIEventAdapter::KEY_Num_Lock] = VK_NUMLOCK;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Divide] = VK_DIVIDE;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Multiply] = VK_MULTIPLY;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Subtract] = VK_SUBTRACT;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Add] = VK_ADD;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Home] = VK_NUMPAD7;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Up] = VK_NUMPAD8;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Page_Up] = VK_NUMPAD9;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Left] = VK_NUMPAD4;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Begin] = VK_NUMPAD5;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Right] = VK_NUMPAD6;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_End] = VK_NUMPAD1;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Down] = VK_NUMPAD2;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Page_Down] = VK_NUMPAD3;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Insert] = VK_NUMPAD0;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Delete] = VK_DECIMAL;
            _keymap[osgGA::GUIEventAdapter::KEY_Clear] = VK_CLEAR;
#endif

            //TODO: keymaps for other platforms?

    }

    int KeyboardEventAdapter::remapKey(int key)
    {
        KeyMap::const_iterator map = _keymap.find(key);
        return map==_keymap.end() ? key : map->second;
    }