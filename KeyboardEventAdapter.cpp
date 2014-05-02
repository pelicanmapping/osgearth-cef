#include "KeyboardEventAdapter"

#include "include/internal/cef_types.h"

#ifdef WIN32
  #include <windows.h>
#elif X11
  #include <X11/keysym.h>
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
#elif X11
            _keymap[osgGA::GUIEventAdapter::KEY_Escape] = XK_Escape;
            _keymap[osgGA::GUIEventAdapter::KEY_F1] = XK_F1;
            _keymap[osgGA::GUIEventAdapter::KEY_F2] = XK_F2;
            _keymap[osgGA::GUIEventAdapter::KEY_F3] = XK_F3;
            _keymap[osgGA::GUIEventAdapter::KEY_F4] = XK_F4;
            _keymap[osgGA::GUIEventAdapter::KEY_F5] = XK_F5;
            _keymap[osgGA::GUIEventAdapter::KEY_F6] = XK_F6;
            _keymap[osgGA::GUIEventAdapter::KEY_F7] = XK_F7;
            _keymap[osgGA::GUIEventAdapter::KEY_F8] = XK_F8;
            _keymap[osgGA::GUIEventAdapter::KEY_F9] = XK_F9;
            _keymap[osgGA::GUIEventAdapter::KEY_F10] = XK_F10;
            _keymap[osgGA::GUIEventAdapter::KEY_F11] = XK_F11;
            _keymap[osgGA::GUIEventAdapter::KEY_F12] = XK_F12;
            _keymap['`'] = XK_quoteleft;
            _keymap['-'] = XK_minus;
            _keymap['='] = XK_equal;
            _keymap[osgGA::GUIEventAdapter::KEY_BackSpace] = XK_BackSpace;
            _keymap[osgGA::GUIEventAdapter::KEY_Tab] = XK_Tab;
            _keymap['['] = XK_bracketleft;
            _keymap[']'] = XK_bracketright;
            _keymap['\\'] = XK_backslash;
            _keymap[osgGA::GUIEventAdapter::KEY_Caps_Lock] = XK_Caps_Lock;
            _keymap[';'] = XK_semicolon;
            _keymap['\''] = XK_apostrophe;
            _keymap[osgGA::GUIEventAdapter::KEY_Return] = XK_Return;
            _keymap[','] = XK_comma;
            _keymap['.'] = XK_period;
            _keymap['/'] = XK_slash;
            _keymap[' '] = XK_space;
            _keymap[osgGA::GUIEventAdapter::KEY_Shift_L] = XK_Shift_L;
            _keymap[osgGA::GUIEventAdapter::KEY_Shift_R] = XK_Shift_R;
            _keymap[osgGA::GUIEventAdapter::KEY_Control_L] = XK_Control_L;
            _keymap[osgGA::GUIEventAdapter::KEY_Control_R] = XK_Control_R;
            _keymap[osgGA::GUIEventAdapter::KEY_Meta_L] = XK_Meta_L;
            _keymap[osgGA::GUIEventAdapter::KEY_Meta_R] = XK_Meta_R;
            _keymap[osgGA::GUIEventAdapter::KEY_Alt_L] = XK_Alt_L;
            _keymap[osgGA::GUIEventAdapter::KEY_Alt_R] = XK_Alt_R;
            _keymap[osgGA::GUIEventAdapter::KEY_Super_L] = XK_Super_L;
            _keymap[osgGA::GUIEventAdapter::KEY_Super_R] = XK_Super_R;
            _keymap[osgGA::GUIEventAdapter::KEY_Hyper_L] = XK_Hyper_L;
            _keymap[osgGA::GUIEventAdapter::KEY_Hyper_R] = XK_Hyper_R;
            _keymap[osgGA::GUIEventAdapter::KEY_Menu] = XK_Menu;
            _keymap[osgGA::GUIEventAdapter::KEY_Print] = XK_Print;
            _keymap[osgGA::GUIEventAdapter::KEY_Scroll_Lock] = XK_Scroll_Lock;
            _keymap[osgGA::GUIEventAdapter::KEY_Pause] = XK_Pause;
            _keymap[osgGA::GUIEventAdapter::KEY_Home] = XK_Home;
            _keymap[osgGA::GUIEventAdapter::KEY_Page_Up] = XK_Page_Up;
            _keymap[osgGA::GUIEventAdapter::KEY_End] = XK_End;
            _keymap[osgGA::GUIEventAdapter::KEY_Page_Down] = XK_Page_Down;
            _keymap[osgGA::GUIEventAdapter::KEY_Delete] = XK_Delete;
            _keymap[osgGA::GUIEventAdapter::KEY_Insert] = XK_Insert;
            _keymap[osgGA::GUIEventAdapter::KEY_Left] = XK_Left;
            _keymap[osgGA::GUIEventAdapter::KEY_Up] = XK_Up;
            _keymap[osgGA::GUIEventAdapter::KEY_Right] = XK_Right;
            _keymap[osgGA::GUIEventAdapter::KEY_Down] = XK_Down;
            _keymap[osgGA::GUIEventAdapter::KEY_Num_Lock] = XK_Num_Lock;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Divide] = XK_KP_Divide;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Multiply] = XK_KP_Multiply;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Subtract] = XK_KP_Subtract;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Add] = XK_KP_Add;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Home] = XK_KP_Home;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Up] = XK_KP_Up;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Page_Up] = XK_KP_Page_Up;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Left] = XK_KP_Left;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Begin] = XK_KP_Begin;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Right] = XK_KP_Right;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_End] = XK_KP_End;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Down] = XK_KP_Down;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Page_Down] = XK_KP_Page_Down;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Insert] = XK_KP_Insert;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Delete] = XK_KP_Delete;
            _keymap[osgGA::GUIEventAdapter::KEY_KP_Enter] = XK_KP_Enter;
            _keymap['1'] = XK_1;
            _keymap['2'] = XK_2;
            _keymap['3'] = XK_3;
            _keymap['4'] = XK_4;
            _keymap['5'] = XK_5;
            _keymap['6'] = XK_6;
            _keymap['7'] = XK_7;
            _keymap['8'] = XK_8;
            _keymap['9'] = XK_9;
            _keymap['0'] = XK_0;
            _keymap['A'] = XK_A;
            _keymap['B'] = XK_B;
            _keymap['C'] = XK_C;
            _keymap['D'] = XK_D;
            _keymap['E'] = XK_E;
            _keymap['F'] = XK_F;
            _keymap['G'] = XK_G;
            _keymap['H'] = XK_H;
            _keymap['I'] = XK_I;
            _keymap['J'] = XK_J;
            _keymap['K'] = XK_K;
            _keymap['L'] = XK_L;
            _keymap['M'] = XK_M;
            _keymap['N'] = XK_N;
            _keymap['O'] = XK_O;
            _keymap['P'] = XK_P;
            _keymap['Q'] = XK_Q;
            _keymap['R'] = XK_R;
            _keymap['S'] = XK_S;
            _keymap['T'] = XK_T;
            _keymap['U'] = XK_U;
            _keymap['V'] = XK_V;
            _keymap['W'] = XK_W;
            _keymap['X'] = XK_X;
            _keymap['Y'] = XK_Y;
            _keymap['Z'] = XK_Z;
            _keymap['a'] = XK_a;
            _keymap['b'] = XK_b;
            _keymap['c'] = XK_c;
            _keymap['d'] = XK_d;
            _keymap['e'] = XK_e;
            _keymap['f'] = XK_f;
            _keymap['g'] = XK_g;
            _keymap['h'] = XK_h;
            _keymap['i'] = XK_i;
            _keymap['j'] = XK_j;
            _keymap['k'] = XK_k;
            _keymap['l'] = XK_l;
            _keymap['m'] = XK_m;
            _keymap['n'] = XK_n;
            _keymap['o'] = XK_o;
            _keymap['p'] = XK_p;
            _keymap['q'] = XK_q;
            _keymap['r'] = XK_r;
            _keymap['s'] = XK_s;
            _keymap['t'] = XK_t;
            _keymap['u'] = XK_u;
            _keymap['v'] = XK_v;
            _keymap['w'] = XK_w;
            _keymap['x'] = XK_x;
            _keymap['y'] = XK_y;
            _keymap['z'] = XK_z;
#endif

            //TODO: keymaps for other platforms?

    }

    int KeyboardEventAdapter::remapKey(int key)
    {
        KeyMap::const_iterator map = _keymap.find(key);
        return map==_keymap.end() ? key : map->second;
    }

    unsigned int KeyboardEventAdapter::getCefModifiers(int modKeyMask)
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
        if (modKeyMask & osgGA::GUIEventAdapter::MODKEY_LEFT_ALT ||
            modKeyMask & osgGA::GUIEventAdapter::MODKEY_LEFT_CTRL ||
            modKeyMask & osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT)
          modifiers |= EVENTFLAG_IS_LEFT;
        if (modKeyMask & osgGA::GUIEventAdapter::MODKEY_RIGHT_ALT ||
            modKeyMask & osgGA::GUIEventAdapter::MODKEY_RIGHT_CTRL ||
            modKeyMask & osgGA::GUIEventAdapter::MODKEY_RIGHT_SHIFT)
          modifiers |= EVENTFLAG_IS_RIGHT;

        //if (modKeyMask & osgGA::GUIEventAdapter:)
        //  modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
        //if (modKeyMask & osgGA::GUIEventAdapter::MODKEY_CTRL)
        //  modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
        //if (modKeyMask & osgGA::GUIEventAdapter::MODKEY_CTRL)
        //  modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

        return modifiers;
    }

    //int OSRWindow::GetCefKeyboardModifiers(WPARAM wparam, LPARAM lparam)
    //{
    //  switch (wparam) {
    //  case VK_RETURN:
    //    if ((lparam >> 16) & KF_EXTENDED)
    //      modifiers |= EVENTFLAG_IS_KEY_PAD;
    //    break;
    //  case VK_INSERT:
    //  case VK_DELETE:
    //  case VK_HOME:
    //  case VK_END:
    //  case VK_PRIOR:
    //  case VK_NEXT:
    //  case VK_UP:
    //  case VK_DOWN:
    //  case VK_LEFT:
    //  case VK_RIGHT:
    //    if (!((lparam >> 16) & KF_EXTENDED))
    //      modifiers |= EVENTFLAG_IS_KEY_PAD;
    //    break;
    //  case VK_NUMLOCK:
    //  case VK_NUMPAD0:
    //  case VK_NUMPAD1:
    //  case VK_NUMPAD2:
    //  case VK_NUMPAD3:
    //  case VK_NUMPAD4:
    //  case VK_NUMPAD5:
    //  case VK_NUMPAD6:
    //  case VK_NUMPAD7:
    //  case VK_NUMPAD8:
    //  case VK_NUMPAD9:
    //  case VK_DIVIDE:
    //  case VK_MULTIPLY:
    //  case VK_SUBTRACT:
    //  case VK_ADD:
    //  case VK_DECIMAL:
    //  case VK_CLEAR:
    //    modifiers |= EVENTFLAG_IS_KEY_PAD;
    //    break;
    //  case VK_SHIFT:
    //    if (isKeyDown(VK_LSHIFT))
    //      modifiers |= EVENTFLAG_IS_LEFT;
    //    else if (isKeyDown(VK_RSHIFT))
    //      modifiers |= EVENTFLAG_IS_RIGHT;
    //    break;
    //  case VK_CONTROL:
    //    if (isKeyDown(VK_LCONTROL))
    //      modifiers |= EVENTFLAG_IS_LEFT;
    //    else if (isKeyDown(VK_RCONTROL))
    //      modifiers |= EVENTFLAG_IS_RIGHT;
    //    break;
    //  case VK_MENU:
    //    if (isKeyDown(VK_LMENU))
    //      modifiers |= EVENTFLAG_IS_LEFT;
    //    else if (isKeyDown(VK_RMENU))
    //      modifiers |= EVENTFLAG_IS_RIGHT;
    //    break;
    //  case VK_LWIN:
    //    modifiers |= EVENTFLAG_IS_LEFT;
    //    break;
    //  case VK_RWIN:
    //    modifiers |= EVENTFLAG_IS_RIGHT;
    //    break;
    //  }
    //  return modifiers;
    //}

    bool KeyboardEventAdapter::confirmCharKey(int key)
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
