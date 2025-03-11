//region Windows

//Windows 7 or later
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>

#define NOMINMAX
#define STRICT

#include <dde.h>
#include <ddeml.h>
#include <mmsystem.h>
#include <rpc.h>

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#define _UNICODE
#include <windowsx.h>

//endregion

#include "components/hplatform.h"

#define private public
#include "components/windowhandling/hwindow.h"
#include "components/windowhandling/hkeyboard.h"
#include "components/windowhandling/hmouse.h"
#undef private

#include "hyperflow.h"
#include "components/hinternal.h"
#include <sstream>
#include <codecvt>

namespace hf
{
	//region Definitions

	const char* WINDOWS_CLASS_NAME = "HyperflowClass";

	//Converts Window Virtual Key to Hyper Key Code
	constexpr uint8_t KEY_CODES[] =
	{
		(uint8_t)Key::None,               // 0: Reserved
		(uint8_t)Button::Left,            // 1: Mouse button left                     VK_LBUTTON
		(uint8_t)Button::Right,           // 2: Mouse button right                    VK_RBUTTON
		(uint8_t)Key::None,               // 3: Not Implemented                       VK_CANCEL
		(uint8_t)Button::Wheel,           // 4: Mouse button middle (pressed wheel)   VK_MBUTTON
		(uint8_t)Button::Extra1,          // 5: Mouse button extra 1                  VK_XBUTTON1
		(uint8_t)Button::Extra2,          // 6: Mouse button extra 2                  VK_XBUTTON2
		(uint8_t)Key::None,               // 7: Reserved
		(uint8_t)Key::Backspace,          // 8: Backspace Key                         VK_BACK
		(uint8_t)Key::Tab,                // 9: Tab Key                               VK_TAB
		(uint8_t)Key::None, (uint8_t)Key::None,               // 10, 11: Reserved
		(uint8_t)Key::None,               // 12: Not Implemented                      VK_CLEAR
		(uint8_t)Key::Enter,              // 13: Enter Key                            VK_ENTER
		(uint8_t)Key::None, (uint8_t)Key::None,               // 14, 15: Unassigned
		(uint8_t)Key::Shift,              // 16: Shift Key                            VK_SHIFT
		(uint8_t)Key::Control,            // 17: Ctrl Key                             VK_CONTROL
		(uint8_t)Key::Alt,                // 18: Alt Key                              VK_MENU
		(uint8_t)Key::Pause,              // 19: Pause Key                            VK_PAUSE
		(uint8_t)Key::CapsLock,           // 20: Caps Lock Key                        VK_CAPITAL
		(uint8_t)Key::None,               // 21: Not Implemented                      VK_KANA, VK_HANGUL
		(uint8_t)Key::None,               // 22: Not Implemented                      VK_IME_ON
		(uint8_t)Key::None,               // 23: Not Implemented                      VK_JUNJA
		(uint8_t)Key::None,               // 24: Not Implemented                      VK_FINAL
		(uint8_t)Key::None,               // 25: Not Implemented                      VK_HANJA, VK_KANJI
		(uint8_t)Key::None,               // 26: Not Implemented                      VK_IME_OFF
		(uint8_t)Key::Escape,             // 27: Esc Key                              VK_ESCAPE
		(uint8_t)Key::None,               // 28: Not Implemented                      VK_CONVERT
		(uint8_t)Key::None,               // 29: Not Implemented                      VK_NONCONVERT
		(uint8_t)Key::None,               // 30: Not Implemented                      VK_ACCEPT
		(uint8_t)Key::None,               // 31: Not Implemented                      VK_MODECHANGE
		(uint8_t)Key::Space,              // 32: Space Key                            VK_SPACE
		(uint8_t)Key::PageUp,             // 33: Page Up key                          VK_PRIOR
		(uint8_t)Key::PageDown,           // 34: Page Down key                        VK_NEXT
		(uint8_t)Key::End,                // 35: End key                              VK_END
		(uint8_t)Key::Home,               // 36: Home key                             VK_HOME
		(uint8_t)Key::Left,               // 37: Left arrow key                       VK_LEFT
		(uint8_t)Key::Up,                 // 38: Up arrow key                         VK_UP
		(uint8_t)Key::Right,              // 39: Right arrow key                      VK_RIGHT
		(uint8_t)Key::Down,               // 40: Down arrow key                       VK_DOWN
		(uint8_t)Key::None,               // 41: Not Implemented                      VK_SELECT
		(uint8_t)Key::None,               // 42: Not Implemented                      VK_PRINT
		(uint8_t)Key::None,               // 43: Not Implemented                      VK_EXECUTE
		(uint8_t)Key::PrintScreen,        // 44: PrtSc Key                            VK_SNAPSHOT
		(uint8_t)Key::Insert,             // 45: Insert Key                           VK_INSERT
		(uint8_t)Key::Delete,             // 46: Delete Key                           VK_DELETE
		(uint8_t)Key::None,               // 47: Not Implemented                      VK_HELP
		(uint8_t)Key::Zero,               // 48: 0 Key
		(uint8_t)Key::One,                // 49: 1 Key
		(uint8_t)Key::Two,                // 50: 2 Key
		(uint8_t)Key::Three,              // 51: 3 Key
		(uint8_t)Key::Four,               // 52: 4 Key
		(uint8_t)Key::Five,               // 53: 5 Key
		(uint8_t)Key::Six,                // 54: 6 Key
		(uint8_t)Key::Seven,              // 55: 7 Key
		(uint8_t)Key::Eight,              // 56: 8 Key
		(uint8_t)Key::Nine,               // 57: 9 Key
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, // 58-64 Undefined
		(uint8_t)Key::A,                  // 65: a | A Key
		(uint8_t)Key::B,                  // 66: b | B Key
		(uint8_t)Key::C,                  // 67: c | C Key
		(uint8_t)Key::D,                  // 68: d | D Key
		(uint8_t)Key::E,                  // 69: e | E Key
		(uint8_t)Key::F,                  // 70: f | F Key
		(uint8_t)Key::G,                  // 71: g | G Key
		(uint8_t)Key::H,                  // 72: h | H Key
		(uint8_t)Key::I,                  // 73: i | I Key
		(uint8_t)Key::J,                  // 74: j | J Key
		(uint8_t)Key::K,                  // 75: k | K Key
		(uint8_t)Key::L,                  // 76: l | L Key
		(uint8_t)Key::M,                  // 77: m | M Key
		(uint8_t)Key::N,                  // 78: n | N Key
		(uint8_t)Key::O,                  // 79: O | O Key
		(uint8_t)Key::P,                  // 80: p | P Key
		(uint8_t)Key::Q,                  // 81: q | Q Key
		(uint8_t)Key::R,                  // 82: r | R Key
		(uint8_t)Key::S,                  // 83: s | S Key
		(uint8_t)Key::T,                  // 84: t | T Key
		(uint8_t)Key::U,                  // 85: u | U Key
		(uint8_t)Key::V,                  // 86: v | V Key
		(uint8_t)Key::W,                  // 87: w | W Key
		(uint8_t)Key::X,                  // 88: x | X Key
		(uint8_t)Key::Y,                  // 89: y | Y Key
		(uint8_t)Key::Z,                  // 90: z | Z Key
		(uint8_t)Key::LeftSuper,          // 91: Left Windows Key                     VK_LWIN
		(uint8_t)Key::RightSuper,         // 92: Right Windows Key                    VK_RWIN
		(uint8_t)Key::None,               // 93: Not Implemented                      VK_APPS
		(uint8_t)Key::None,               // 94: Reserved
		(uint8_t)Key::None,               // 95: Not Implemented                      VK_SLEEP
		(uint8_t)Key::Pad0,               // 96: Keypad 0 Key                         VK_NUMPAD0
		(uint8_t)Key::Pad1,               // 97: Keypad 1 Key                         VK_NUMPAD1
		(uint8_t)Key::Pad2,               // 98: Keypad 2 Key                         VK_NUMPAD2
		(uint8_t)Key::Pad3,               // 99: Keypad 3 Key                         VK_NUMPAD3
		(uint8_t)Key::Pad4,               // 100: Keypad 4 Key                        VK_NUMPAD4
		(uint8_t)Key::Pad5,               // 101: Keypad 5 Key                        VK_NUMPAD5
		(uint8_t)Key::Pad6,               // 102: Keypad 6 Key                        VK_NUMPAD6
		(uint8_t)Key::Pad7,               // 103: Keypad 7 Key                        VK_NUMPAD7
		(uint8_t)Key::Pad8,               // 104: Keypad 8 Key                        VK_NUMPAD8
		(uint8_t)Key::Pad9,               // 105: Keypad 9 Key                        VK_NUMPAD9
		(uint8_t)Key::PadMultiply,        // 106: Multiply Key                        VK_MULTIPLY
		(uint8_t)Key::PadAdd,             // 107: Add Key                             VK_ADD
		(uint8_t)Key::PadEqual,           // 108: Equal Key                           VK_SEPARATOR
		(uint8_t)Key::PadSubtract,        // 109: Subtract key                        VK_SUBTRACT
		(uint8_t)Key::PadDecimal,         // 110: Decimal key                         VK_DECIMAL
		(uint8_t)Key::PadDivide,          // 111: Divide key                          VK_DIVIDE
		(uint8_t)Key::F1,                 // 112: F1 key                              VK_F1
		(uint8_t)Key::F2,                 // 113: F2 key                              VK_F2
		(uint8_t)Key::F3,                 // 114: F3 key                              VK_F3
		(uint8_t)Key::F4,                 // 115: F4 key                              VK_F4
		(uint8_t)Key::F5,                 // 116: F5 key                              VK_F5
		(uint8_t)Key::F6,                 // 117: F6 key                              VK_F6
		(uint8_t)Key::F7,                 // 118: F7 key                              VK_F7
		(uint8_t)Key::F8,                 // 119: F8 key                              VK_F8
		(uint8_t)Key::F9,                 // 120: F9 key                              VK_F9
		(uint8_t)Key::F10,                // 121: F10 key                             VK_F10
		(uint8_t)Key::F11,                // 122: F11 key                             VK_F11
		(uint8_t)Key::F12,                // 123: F12 key                             VK_F12

		// 124-135: F13-F24 keys                                            VK_F13-VK_F24
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None,
		// 136-143: Reserved
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None,
		
		(uint8_t)Key::NumLock,            // 144: Num lock key                        VK_NUMLOCK
		(uint8_t)Key::ScrollLock,         // 145: Scroll lock key                     VK_SCROLL

		// 146-150: OEM specific
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None,
		// 151-159: Unassigned
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None,
		
		(uint8_t)Key::LeftShift,          // 160: Left Shift Key                      VK_LSHIFT
		(uint8_t)Key::RightShift,         // 161: Right Shift Key                     VK_RSHIFT
		(uint8_t)Key::LeftControl,        // 162: Left Ctrl Key                       VK_LCONTROL
		(uint8_t)Key::RightControl,       // 163: Right Ctrl Key                      VK_RCONTROL
		(uint8_t)Key::LeftAlt,            // 164: Left Alt Key                        VK_LMENU
		(uint8_t)Key::RightAlt,           // 165: Right Alt Key                       VK_RMENU

		// 166-185: Not Implemented
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None,
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None,
		
		(uint8_t)Key::Semicolon,          // 186: ; Key                               VK_OEM_1
		(uint8_t)Key::Equal,              // 187: = Key                               VK_OEM_PLUS
		(uint8_t)Key::Comma,              // 188: , Key                               VK_OEM_COMMA
		(uint8_t)Key::Minus,              // 189: - Key                               VK_OEM_MINUS
		(uint8_t)Key::Period,             // 190: . Key                               VK_OEM_PERIOD
		(uint8_t)Key::Slash,              // 191: / Key                               VK_OEM_2
		(uint8_t)Key::Grave,              // 192: ` Key                               VK_OEM_3

		// 193-218 Reserved
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None,
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None,
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None,
		
		(uint8_t)Key::LeftBracket,        // 219: [ Key                               VK_OEM_4
		(uint8_t)Key::Backslash,          // 220: \ Key                               VK_OEM_5
		(uint8_t)Key::RightBracket,       // 221: ] Key                               VK_OEM_6
		(uint8_t)Key::Apostrophe,         // 222: ' Key                               VK_OEM_7

		// 223-254 Not Implemented
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None,
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None,
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None,
		(uint8_t)Key::None, (uint8_t)Key::None, (uint8_t)Key::None
	};
	//endregion

	//region Exception Handling
	class WindowException : public HyperException
	{
	public:
		WindowException(int32_t lineNum, const char* file, HRESULT errorCode)
		: HyperException(lineNum, file), m_ErrorCode(errorCode)
		{

		}

		const char* what() const noexcept override
		{
			std::ostringstream oss;
			oss << GetType() << std::endl
			    << "[Error Code] " << GetErrorCode() << std::endl
			    << "[Description] " << GetErrorString();

			m_WhatBuffer = oss.str();
			return m_WhatBuffer.c_str();
		}

		static std::string TranslateErrorCode(HRESULT result)
		{
			char* pMsgBuf = nullptr;
			DWORD nMsgLen = FormatMessage
				(
					FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					nullptr, result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
				);
			if(nMsgLen == 9) return "Unidentified error code";
			std::string errorString = pMsgBuf;
			LocalFree(pMsgBuf);
			return errorString;
		}

		HRESULT GetErrorCode() const noexcept { return m_ErrorCode; }
		const char* GetType() const noexcept override { return "[Window Exception]"; }
		std::string GetErrorString() const noexcept { return TranslateErrorCode(m_ErrorCode); }

	private:
		HRESULT m_ErrorCode;
	};

#define WND_EXCEPT(hr) WindowException(__LINE__, __FILE__, hr)
#define WND_LAST_EXCEPT() WindowException(__LINE__, __FILE__, GetLastError())
	//endregion

	//region Message Handing
	LRESULT Platform_HandleEvents_WindowTitle       (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowClose       (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowShow        (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowMove        (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowResize      (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowFocus       (Window* window, WPARAM wparam, LPARAM lparam, bool focused);
	LRESULT Platform_HandleEvents_KeyboardChar      (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_KeyboardKey       (Window* window, WPARAM wparam, LPARAM lparam, Keyboard::Event::Type type);

	LRESULT Platform_HandleEvents_MouseButton       (Window* window, WPARAM wparam, LPARAM lparam, Button button, Mouse::Event::Type type);
	LRESULT Platform_HandleEvents_MouseButtonExtra  (Window* window, WPARAM wparam, LPARAM lparam, Mouse::Event::Type type);
	LRESULT Platform_HandleEvents_MouseMove         (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_MouseScroll       (Window* window, WPARAM wparam, LPARAM lparam, glm::ivec2 direction);

	//endregion

	//region WinProc

	LRESULT CALLBACK Platform_WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		auto* window = (Window*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		switch (msg)
		{
			//Window
			case WM_SETTEXT:              Platform_HandleEvents_WindowTitle      (window, wparam, lparam); break;
			case WM_CLOSE:         return Platform_HandleEvents_WindowClose      (window, wparam, lparam);
			case WM_SHOWWINDOW:    return Platform_HandleEvents_WindowShow       (window, wparam, lparam);
			case WM_MOVE:          return Platform_HandleEvents_WindowMove       (window, wparam, lparam);
			case WM_SIZE:          return Platform_HandleEvents_WindowResize     (window, wparam, lparam);
			case WM_SETFOCUS:      return Platform_HandleEvents_WindowFocus      (window, wparam, lparam, true);
			case WM_KILLFOCUS:     return Platform_HandleEvents_WindowFocus      (window, wparam, lparam, false);

			//KeyBoard
			case WM_CHAR:          return Platform_HandleEvents_KeyboardChar(window, wparam, lparam);
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:    return Platform_HandleEvents_KeyboardKey(window, wparam, lparam, Keyboard::Event::Type::Press);
			case WM_KEYUP:         return Platform_HandleEvents_KeyboardKey(window, wparam, lparam, Keyboard::Event::Type::Release);

			//Mouse
			case WM_LBUTTONDOWN:   return Platform_HandleEvents_MouseButton(window, wparam, lparam, Button::Left, Mouse::Event::Type::Press);
			case WM_RBUTTONDOWN:   return Platform_HandleEvents_MouseButton(window, wparam, lparam, Button::Right, Mouse::Event::Type::Press);
			case WM_MBUTTONDOWN:   return Platform_HandleEvents_MouseButton(window, wparam, lparam, Button::Wheel, Mouse::Event::Type::Press);
			case WM_XBUTTONDOWN:   return Platform_HandleEvents_MouseButtonExtra(window, wparam, lparam, Mouse::Event::Type::Press);

			case WM_LBUTTONUP:     return Platform_HandleEvents_MouseButton(window, wparam, lparam, Button::Left, Mouse::Event::Type::Release);
			case WM_RBUTTONUP:     return Platform_HandleEvents_MouseButton(window, wparam, lparam, Button::Right, Mouse::Event::Type::Release);
			case WM_MBUTTONUP:     return Platform_HandleEvents_MouseButton(window, wparam, lparam, Button::Wheel, Mouse::Event::Type::Release);
			case WM_XBUTTONUP:     return Platform_HandleEvents_MouseButtonExtra(window, wparam, lparam, Mouse::Event::Type::Release);

			case WM_MOUSEMOVE:     return Platform_HandleEvents_MouseMove(window, wparam, lparam);
			case WM_MOUSEWHEEL:    return Platform_HandleEvents_MouseScroll(window, wparam, lparam, glm::ivec2{ 0, 1 });
			case WM_MOUSEHWHEEL:   return Platform_HandleEvents_MouseScroll(window, wparam, lparam, glm::ivec2{ 1, 0 });

			default: break;
		}

		return DefWindowProcA(hwnd, msg, wparam, lparam);
	}

	LRESULT CALLBACK Platform_WindowProc_Create(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if(msg == WM_NCCREATE)
		{
			auto* pCreate = (CREATESTRUCT*)lparam;
			auto* window = (Window*)pCreate->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)Platform_WindowProc);
		}
		return DefWindowProcA(hwnd, msg, wparam, lparam);
	}

	//endregion

	//region Platform Specific Functions

	void Platform_Initialize()
	{
		HINSTANCE hinstance = GetModuleHandle(nullptr);

		WNDCLASSEX wndClass;
		wndClass.cbSize = sizeof(wndClass);
		wndClass.lpszClassName = WINDOWS_CLASS_NAME;
		wndClass.style = CS_OWNDC;
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wndClass.hbrBackground = nullptr;
		wndClass.lpszMenuName = nullptr;
		wndClass.hInstance = hinstance;
		wndClass.lpfnWndProc = Platform_WindowProc_Create;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hIconSm = nullptr;
		wndClass.hInstance = hinstance;

		RegisterClassEx(&wndClass);
	}

	void Platform_BeginTemporarySystemTimer(uint16_t millisecondPrecision)
	{
		timeBeginPeriod(millisecondPrecision);
	}
	void Platform_EndTemporarySystemTimer(uint16_t millisecondPrecision)
	{
		timeEndPeriod(millisecondPrecision);
	}

	uint64_t Platform_GetTicks()
	{
		return GetTickCount64();
	}

	uint32_t GetStyleID(WindowStyle style)
	{
		switch (style)
		{
			case WindowStyle::Default: return WS_SIZEBOX | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
		}
		return 0;
	}

	void Platform_Sleep(double seconds)
	{
		// A waitable timer seems to be better than the Windows Sleep().
		HANDLE WaitTimer;
		LARGE_INTEGER dueTime;
		seconds *= -10.0 * 1000.0 * 1000.0;
		dueTime.QuadPart = static_cast<LONGLONG>(seconds); //dueTime is in 100ns
		// We don't name the timer (third parameter) because CreateWaitableTimer will fail if the name
		// matches an existing name (e.g., if two threads call osaSleep).
		WaitTimer = CreateWaitableTimer(nullptr, true, nullptr);
		SetWaitableTimer(WaitTimer, &dueTime, 0, nullptr, nullptr, 0);
		WaitForSingleObject(WaitTimer, INFINITE);
		CloseHandle(WaitTimer);
	}

	glm::ivec2 Platform_GetPointerPosition(Window* window)
	{
		POINT point;
		if(!GetCursorPos(&point)) throw WND_LAST_EXCEPT();
		if(!ScreenToClient((HWND)window->m_Handle, &point)) throw WND_LAST_EXCEPT();
		return { (int32_t)point.x, (int32_t)point.y };
	}

	void Platform_ConvertSize(Window* window, glm::ivec2& size)
	{
		uint32_t currentStyle = GetStyleID(window->m_Style);
		RECT targetRect = { 0, 0, size[0], size[1] };
		if(!AdjustWindowRectEx(&targetRect, currentStyle, false, 0)) throw WND_LAST_EXCEPT();
		size[0] = targetRect.right - targetRect.left;
		size[1] = targetRect.bottom - targetRect.top;
	}

	//endregion

	//region Window

	Window::Window(const WindowData& data, const Ref<Window>& parent)
	{
		HINSTANCE hinstance = GetModuleHandle(nullptr);
		uint32_t currentStyle = GetStyleID(data.style);

		m_Title = data.title;
		m_Style = data.style;
		m_Parent = parent;
		m_ShouldClose = false;
		m_Flags = (WindowFlags)0;
		m_Rect =
		{
			.position = data.position,
			.size = data.size
		};

		HWND parentHandle = nullptr;
		if(parent != nullptr) parentHandle = (HWND)parent->m_Handle;

		glm::ivec2 convertedSize = data.size;
		Platform_ConvertSize(this, convertedSize);

		m_Handle = CreateWindowEx
		(
			0,
			WINDOWS_CLASS_NAME,
			data.title.c_str(),
			currentStyle,
			data.position[0], data.position[1],
			convertedSize[0], convertedSize[1],
			parentHandle,
			nullptr,
			hinstance,
			this
		);

		if(m_Handle == nullptr) throw WND_LAST_EXCEPT();
		
		auto pPos = Platform_GetPointerPosition(this);
		m_Mouse = MakeRef<Mouse>(pPos, pPos.x >= 0 && pPos.x < m_Rect.size.x && pPos.y > 0 && pPos.y < m_Rect.size.y);
		m_Keyboard = MakeRef<Keyboard>();
		m_EventData.pointerPosition = m_Mouse->m_Position;
		
		SetFlags(data.flags);
		Focus();
	}

	Window::~Window()
	{
		DestroyWindow((HWND)m_Handle);
	}

	void Window::SetTitle(const char* title) const
	{
		if(!SetWindowText((HWND)m_Handle, title)) throw WND_LAST_EXCEPT();
	}
	void Window::SetSize(glm::ivec2 size)
	{
		Platform_ConvertSize(this, size);
		if(!SetWindowPos((HWND)m_Handle, nullptr, 0, 0, size[0], size[1], SWP_NOMOVE))
			throw WND_LAST_EXCEPT();
	}
	void Window::SetPosition(glm::ivec2 position) const
	{
		if(!SetWindowPos((HWND)m_Handle, nullptr, position[0], position[1], 0, 0, SWP_NOSIZE))
			throw WND_LAST_EXCEPT();
	}
	void Window::SetRect(IRect rect)
	{
		Platform_ConvertSize(this, rect.size);
		if(!SetWindowPos((HWND)m_Handle, nullptr, rect.position[0], rect.position[1], rect.size[0], rect.size[1], 0))
			throw WND_LAST_EXCEPT();
	}

	bool Window::IsClosing() const { return m_ShouldClose; }

	void Window::SetFlags(WindowFlags flags)
	{
		if(m_Flags == flags) return;

		if(((int32_t)m_Flags & (int32_t)WindowFlags::Visible) != ((int32_t)flags & (int32_t)WindowFlags::Visible))
		{
			if((int32_t)flags & (int32_t)WindowFlags::Visible) ShowWindow((HWND)m_Handle, SW_SHOW);
			else ShowWindow((HWND)m_Handle, SW_HIDE);
		}

		if(((int32_t)m_Flags & (int32_t)WindowFlags::Minimized) != ((int32_t)flags & (int32_t)WindowFlags::Minimized) ||
		   ((int32_t)m_Flags & (int32_t)WindowFlags::Maximized) != ((int32_t)flags & (int32_t)WindowFlags::Maximized))
		{
			uint32_t both = (int32_t)flags & (int32_t)WindowFlags::Minimized + (int32_t)flags & (int32_t)WindowFlags::Maximized;
			if((int32_t)flags & (int32_t)WindowFlags::Minimized) ShowWindow((HWND)m_Handle, SW_MINIMIZE);
			if((int32_t)flags & (int32_t)WindowFlags::Maximized) ShowWindow((HWND)m_Handle, SW_MAXIMIZE);
			if(both == 0) ShowWindow((HWND)m_Handle, SW_RESTORE);
		}

		m_Flags = flags;
	}

	void Window::Focus() const { SetFocus((HWND)m_Handle); }

	//endregion

	//region Message Handing Impl

	void Platform_HandleEvents(EngineUpdateType updateType)
	{
		MSG msg;
		switch (updateType)
		{
			case EngineUpdateType::Continues:

				if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				break;
			case EngineUpdateType::EventRaised:
			default:

				if(GetMessage(&msg, nullptr, 0, 0) > 0)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				break;
		}
	}

	void Platform_SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value)
	{
		*flags = (WindowFlags)(((int32_t)*flags & ~(int32_t)target) | (-value & (int32_t)target));
	}

	LRESULT Platform_HandleEvents_WindowClose(Window* window,
	                                          __attribute__((unused)) WPARAM wparam,
	                                          __attribute__((unused)) LPARAM lparam)
	{
		WindowEvent_Close(window);
		return 0;
	}

	LRESULT Platform_HandleEvents_WindowShow(Window* window, WPARAM wparam,
	                                         __attribute__((unused)) LPARAM lparam)
	{
		WindowEvent_Show(window, (bool)wparam);
		return 0;
	}

	LRESULT Platform_HandleEvents_WindowMove(Window* window,
	                                         __attribute__((unused)) WPARAM wparam,
	                                         LPARAM lparam)
	{
		glm::ivec2 position =
		{
			GET_X_LPARAM(lparam),
			GET_Y_LPARAM(lparam)
		};

		WindowEvent_Move(window, position);
		return 0;
	}

	LRESULT Platform_HandleEvents_WindowResize(Window* window,
	                                           __attribute__((unused)) WPARAM wparam,
	                                           LPARAM lparam)
	{
		glm::ivec2 size =
		{
			GET_X_LPARAM(lparam),
			GET_Y_LPARAM(lparam)
		};

		WindowEvent_Resize(window, size);
		return 0;
	}

	LRESULT Platform_HandleEvents_WindowFocus(Window* window,
	                                            __attribute__((unused)) WPARAM wparam,
	                                            __attribute__((unused)) LPARAM lparam,
												bool focused)
	{
		WindowEvent_Focus(window, focused);
		return 0;
	}

	LRESULT Platform_HandleEvents_KeyboardChar(Window* window, WPARAM wparam,
	                                           __attribute__((unused)) LPARAM lparam)
	{
		KeyboardEvent_Char(window->m_Keyboard, (char) wparam);
		return 0;
	}

	LRESULT Platform_HandleEvents_KeyboardKey(Window* window, WPARAM wparam, LPARAM lparam,
	                                       Keyboard::Event::Type type)
	{
		auto key = (Key)(KEY_CODES[(uint8_t)wparam]);

		switch (type)
		{

			case Keyboard::Event::Type::Invalid: LOG_WARN("Invalid Event thrown"); break;
			case Keyboard::Event::Type::Press:

				if(!(lparam & 0x40000000) || window->m_Keyboard->IsAutoRepeatEnabled())
					KeyboardEvent_Key(window->m_Keyboard, key, type);

				break;
			case Keyboard::Event::Type::Release:
				if(window->m_Keyboard->IsPressed(key)) KeyboardEvent_Key(window->m_Keyboard, key, type);
				break;
		}
		return 0;
	}

	LRESULT Platform_HandleEvents_MouseButton(Window* window, WPARAM wparam, LPARAM lparam, Button button, Mouse::Event::Type type)
	{
		MouseEvent_Button(window->m_Mouse, button, type);
		return 0;
	}

	LRESULT Platform_HandleEvents_MouseButtonExtra(Window* window, WPARAM wparam, LPARAM lparam, Mouse::Event::Type type)
	{
		if(wparam & MK_XBUTTON1) MouseEvent_Button(window->m_Mouse, Button::Extra1, type);
		if(wparam & MK_XBUTTON2) MouseEvent_Button(window->m_Mouse, Button::Extra2, type);
		return 0;
	}

	LRESULT Platform_HandleEvents_MouseMove(Window* window, WPARAM wparam, LPARAM lparam)
	{
		const POINTS pt = MAKEPOINTS(lparam);
		auto rect = window->m_Rect;
		
		if(pt.x >= 0 && pt.x < rect.size.x && pt.y > 0 && pt.y < rect.size.y)
		{
			MouseEvent_Moved(window->m_Mouse, glm::ivec2{ pt.x, pt.y });
			if(!window->m_Mouse->m_IsInClientRegion)
			{
				SetCapture((HWND)window->m_Handle);
				window->m_Mouse->m_IsInClientRegion = true;
			}
		}
		else
		{
			if(wparam & (MK_LBUTTON | MK_RBUTTON)) MouseEvent_Moved(window->m_Mouse, glm::ivec2{ pt.x, pt.y });
			else
			{
				ReleaseCapture();
				window->m_Mouse->m_IsInClientRegion = false;
			}
		}
		
		glm::ivec2 position =
		{
			GET_X_LPARAM(lparam),
			GET_Y_LPARAM(lparam)
		};
		MouseEvent_Moved(window->m_Mouse, position);

		return 0;
	}

	LRESULT Platform_HandleEvents_MouseScroll(Window* window, WPARAM wparam, LPARAM lparam, glm::ivec2 direction)
	{
		auto delta = GET_WHEEL_DELTA_WPARAM(wparam) / (float)WHEEL_DELTA;
		
		MouseEvent_Scroll(window->m_Mouse, (glm::vec2)direction * delta);

		return 0;
	}

	LRESULT Platform_HandleEvents_WindowTitle(Window *window, WPARAM wparam, LPARAM lparam)
	{
		auto newTitle = reinterpret_cast<LPCWSTR>(lparam);

		if (newTitle)
		{
			std::wstring ws(newTitle);
			WindowEvent_Title(window, (const char*)ws.c_str());
		}
		return 0;
	}

//endregion
}