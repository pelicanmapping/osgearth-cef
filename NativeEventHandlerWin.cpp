#include "NativeEventHandlerWin"

#include "include/internal/cef_types.h"


using namespace osgEarth;
using namespace osgEarth::Cef;


#define LC "[NativeEventHandlerWin] "


NativeEventHandlerWin::NativeEventHandlerWin(HWND hWnd, BrowserClient* browserClient, CefBrowser* browser)
: _browserClient(browserClient), _browser(browser), _oldWndProc(0L)
{
  _oldWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(&NativeEventHandlerWin::WndProc));

  if (!_oldWndProc)
    OE_WARN << LC << "Error registering WNDPROC handler" << std::endl;

  SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

int NativeEventHandlerWin::GetCefMouseModifiers(WPARAM wparam)
{
  int modifiers = 0;
  if (wparam & MK_CONTROL)
    modifiers |= EVENTFLAG_CONTROL_DOWN;
  if (wparam & MK_SHIFT)
    modifiers |= EVENTFLAG_SHIFT_DOWN;
  if (isKeyDown(VK_MENU))
    modifiers |= EVENTFLAG_ALT_DOWN;
  if (wparam & MK_LBUTTON)
    modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
  if (wparam & MK_MBUTTON)
    modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
  if (wparam & MK_RBUTTON)
    modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

  // Low bit set from GetKeyState indicates "toggled".
  if (::GetKeyState(VK_NUMLOCK) & 1)
    modifiers |= EVENTFLAG_NUM_LOCK_ON;
  if (::GetKeyState(VK_CAPITAL) & 1)
    modifiers |= EVENTFLAG_CAPS_LOCK_ON;
  return modifiers;
}

int NativeEventHandlerWin::GetCefKeyboardModifiers(WPARAM wparam, LPARAM lparam)
{
  int modifiers = 0;
  if (isKeyDown(VK_SHIFT))
    modifiers |= EVENTFLAG_SHIFT_DOWN;
  if (isKeyDown(VK_CONTROL))
    modifiers |= EVENTFLAG_CONTROL_DOWN;
  if (isKeyDown(VK_MENU))
    modifiers |= EVENTFLAG_ALT_DOWN;

  // Low bit set from GetKeyState indicates "toggled".
  if (::GetKeyState(VK_NUMLOCK) & 1)
    modifiers |= EVENTFLAG_NUM_LOCK_ON;
  if (::GetKeyState(VK_CAPITAL) & 1)
    modifiers |= EVENTFLAG_CAPS_LOCK_ON;

  switch (wparam) {
  case VK_RETURN:
    if ((lparam >> 16) & KF_EXTENDED)
      modifiers |= EVENTFLAG_IS_KEY_PAD;
    break;
  case VK_INSERT:
  case VK_DELETE:
  case VK_HOME:
  case VK_END:
  case VK_PRIOR:
  case VK_NEXT:
  case VK_UP:
  case VK_DOWN:
  case VK_LEFT:
  case VK_RIGHT:
    if (!((lparam >> 16) & KF_EXTENDED))
      modifiers |= EVENTFLAG_IS_KEY_PAD;
    break;
  case VK_NUMLOCK:
  case VK_NUMPAD0:
  case VK_NUMPAD1:
  case VK_NUMPAD2:
  case VK_NUMPAD3:
  case VK_NUMPAD4:
  case VK_NUMPAD5:
  case VK_NUMPAD6:
  case VK_NUMPAD7:
  case VK_NUMPAD8:
  case VK_NUMPAD9:
  case VK_DIVIDE:
  case VK_MULTIPLY:
  case VK_SUBTRACT:
  case VK_ADD:
  case VK_DECIMAL:
  case VK_CLEAR:
    modifiers |= EVENTFLAG_IS_KEY_PAD;
    break;
  case VK_SHIFT:
    if (isKeyDown(VK_LSHIFT))
      modifiers |= EVENTFLAG_IS_LEFT;
    else if (isKeyDown(VK_RSHIFT))
      modifiers |= EVENTFLAG_IS_RIGHT;
    break;
  case VK_CONTROL:
    if (isKeyDown(VK_LCONTROL))
      modifiers |= EVENTFLAG_IS_LEFT;
    else if (isKeyDown(VK_RCONTROL))
      modifiers |= EVENTFLAG_IS_RIGHT;
    break;
  case VK_MENU:
    if (isKeyDown(VK_LMENU))
      modifiers |= EVENTFLAG_IS_LEFT;
    else if (isKeyDown(VK_RMENU))
      modifiers |= EVENTFLAG_IS_RIGHT;
    break;
  case VK_LWIN:
    modifiers |= EVENTFLAG_IS_LEFT;
    break;
  case VK_RWIN:
    modifiers |= EVENTFLAG_IS_RIGHT;
    break;
  }
  return modifiers;
}

bool NativeEventHandlerWin::isKeyDown(WPARAM wparam)
{
  return (GetKeyState(wparam) & 0x8000) != 0;
}

LRESULT CALLBACK NativeEventHandlerWin::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  NativeEventHandlerWin* handler = reinterpret_cast<NativeEventHandlerWin*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));


  if (handler && handler->_browserClient && handler->_browserClient->getInFocus())
  {
    switch (message)
    {
      case WM_SYSCHAR:
      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
      case WM_KEYDOWN:
      case WM_KEYUP:
      case WM_CHAR:
      {
        CefKeyEvent event;
        event.windows_key_code = wParam;
        event.native_key_code = lParam;
        event.is_system_key = message == WM_SYSCHAR ||
                              message == WM_SYSKEYDOWN ||
                              message == WM_SYSKEYUP;

        if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
          event.type = KEYEVENT_RAWKEYDOWN;
        else if (message == WM_KEYUP || message == WM_SYSKEYUP)
          event.type = KEYEVENT_KEYUP;
        else
          event.type = KEYEVENT_CHAR;
        event.modifiers = GetCefKeyboardModifiers(wParam, lParam);

        if (handler && handler->_browser)
          handler->_browser->GetHost()->SendKeyEvent(event);

        break;
      }
    }
  }

  return CallWindowProc(handler->_oldWndProc, hWnd, message, wParam, lParam);
}
