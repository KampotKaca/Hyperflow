#include "hplatform.h"

#include <windows.h>

#define private public
#include "hwindow.h"
#undef private

namespace hf
{
	//region Definitions

	typedef struct
	{
		KeyCode keyCode;
		uint32_t virtualKeyCode;
		uint32_t repeatCount;
		uint32_t scanCode;
		bool isExtendedKey;
		bool wasPrevStateDown; //was previous state down
		bool isBeingPressed;   //is key currently being pressed or released
	}KeyStrokeFlags;

	const char* WINDOWS_CLASS_NAME = "HyperflowClass";

	//Converts Window Virtual Key to Hyper Key Code
	constexpr KeyCode KEY_CODES[] =
	{
		KEY_NULL,               // 0: Reserved
		KEY_BUTTON_LEFT,        // 1: Mouse button left                     VK_LBUTTON
		KEY_BUTTON_RIGHT,       // 2: Mouse button right                    VK_RBUTTON
		KEY_NULL,               // 3: Not Implemented                       VK_CANCEL
		KEY_BUTTON_MIDDLE,      // 4: Mouse button middle (pressed wheel)   VK_MBUTTON
		KEY_BUTTON_EXTRA1,      // 5: Mouse button extra 1                  VK_XBUTTON1
		KEY_BUTTON_EXTRA2,      // 6: Mouse button extra 2                  VK_XBUTTON2
		KEY_NULL,               // 7: Reserved
		KEY_BACKSPACE,          // 8: Backspace Key                         VK_BACK
		KEY_TAB,                // 9: Tab Key                               VK_TAB
		KEY_NULL, KEY_NULL,     // 10, 11: Reserved
		KEY_NULL,               // 12: Not Implemented                      VK_CLEAR
		KEY_ENTER,              // 13: Enter Key                            VK_ENTER
		KEY_NULL, KEY_NULL,     // 14, 15: Unassigned
		KEY_SHIFT,              // 16: Shift Key                            VK_SHIFT
		KEY_CONTROL,            // 17: Ctrl Key                             VK_CONTROL
		KEY_ALT,                // 18: Alt Key                              VK_MENU
		KEY_PAUSE,              // 19: Pause Key                            VK_PAUSE
		KEY_CAPS_LOCK,          // 20: Caps Lock Key                        VK_CAPITAL
		KEY_NULL,               // 21: Not Implemented                      VK_KANA, VK_HANGUL
		KEY_NULL,               // 22: Not Implemented                      VK_IME_ON
		KEY_NULL,               // 23: Not Implemented                      VK_JUNJA
		KEY_NULL,               // 24: Not Implemented                      VK_FINAL
		KEY_NULL,               // 25: Not Implemented                      VK_HANJA, VK_KANJI
		KEY_NULL,               // 26: Not Implemented                      VK_IME_OFF
		KEY_ESCAPE,             // 27: Esc Key                              VK_ESCAPE
		KEY_NULL,               // 28: Not Implemented                      VK_CONVERT
		KEY_NULL,               // 29: Not Implemented                      VK_NONCONVERT
		KEY_NULL,               // 30: Not Implemented                      VK_ACCEPT
		KEY_NULL,               // 31: Not Implemented                      VK_MODECHANGE
		KEY_SPACE,              // 32: Space Key                            VK_SPACE
		KEY_PAGE_UP,            // 33: Page Up key                          VK_PRIOR
		KEY_PAGE_DOWN,          // 34: Page Down key                        VK_NEXT
		KEY_END,                // 35: End key                              VK_END
		KEY_HOME,               // 36: Home key                             VK_HOME
		KEY_LEFT,               // 37: Left arrow key                       VK_LEFT
		KEY_UP,                 // 38: Up arrow key                         VK_UP
		KEY_RIGHT,              // 39: Right arrow key                      VK_RIGHT
		KEY_DOWN,               // 40: Down arrow key                       VK_DOWN
		KEY_NULL,               // 41: Not Implemented                      VK_SELECT
		KEY_NULL,               // 42: Not Implemented                      VK_PRINT
		KEY_NULL,               // 43: Not Implemented                      VK_EXECUTE
		KEY_PRINT_SCREEN,       // 44: PrtSc Key                            VK_SNAPSHOT
		KEY_INSERT,             // 45: Insert Key                           VK_INSERT
		KEY_DELETE,             // 46: Delete Key                           VK_DELETE
		KEY_NULL,               // 47: Not Implemented                      VK_HELP
		KEY_ZERO,               // 48: 0 Key
		KEY_ONE,                // 49: 1 Key
		KEY_TWO,                // 50: 2 Key
		KEY_THREE,              // 51: 3 Key
		KEY_FOUR,               // 52: 4 Key
		KEY_FIVE,               // 53: 5 Key
		KEY_SIX,                // 54: 6 Key
		KEY_SEVEN,              // 55: 7 Key
		KEY_EIGHT,              // 56: 8 Key
		KEY_NINE,               // 57: 9 Key
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, // 58-64 Undefined
		KEY_A,                  // 65: a | A Key
		KEY_B,                  // 66: b | B Key
		KEY_C,                  // 67: c | C Key
		KEY_D,                  // 68: d | D Key
		KEY_E,                  // 69: e | E Key
		KEY_F,                  // 70: f | F Key
		KEY_G,                  // 71: g | G Key
		KEY_H,                  // 72: h | H Key
		KEY_I,                  // 73: i | I Key
		KEY_J,                  // 74: j | J Key
		KEY_K,                  // 75: k | K Key
		KEY_L,                  // 76: l | L Key
		KEY_M,                  // 77: m | M Key
		KEY_N,                  // 78: n | N Key
		KEY_O,                  // 79: O | O Key
		KEY_P,                  // 80: p | P Key
		KEY_Q,                  // 81: q | Q Key
		KEY_R,                  // 82: r | R Key
		KEY_S,                  // 83: s | S Key
		KEY_T,                  // 84: t | T Key
		KEY_U,                  // 85: u | U Key
		KEY_V,                  // 86: v | V Key
		KEY_W,                  // 87: w | W Key
		KEY_X,                  // 88: x | X Key
		KEY_Y,                  // 89: y | Y Key
		KEY_Z,                  // 90: z | Z Key
		KEY_LEFT_SUPER,         // 91: Left Windows Key                     VK_LWIN
		KEY_RIGHT_SUPER,        // 92: Right Windows Key                    VK_RWIN
		KEY_NULL,               // 93: Not Implemented                      VK_APPS
		KEY_NULL,               // 94: Reserved
		KEY_NULL,               // 95: Not Implemented                      VK_SLEEP
		KEY_KP_0,               // 96: Keypad 0 Key                         VK_NUMPAD0
		KEY_KP_1,               // 97: Keypad 1 Key                         VK_NUMPAD1
		KEY_KP_2,               // 98: Keypad 2 Key                         VK_NUMPAD2
		KEY_KP_3,               // 99: Keypad 3 Key                         VK_NUMPAD3
		KEY_KP_4,               // 100: Keypad 4 Key                        VK_NUMPAD4
		KEY_KP_5,               // 101: Keypad 5 Key                        VK_NUMPAD5
		KEY_KP_6,               // 102: Keypad 6 Key                        VK_NUMPAD6
		KEY_KP_7,               // 103: Keypad 7 Key                        VK_NUMPAD7
		KEY_KP_8,               // 104: Keypad 8 Key                        VK_NUMPAD8
		KEY_KP_9,               // 105: Keypad 9 Key                        VK_NUMPAD9
		KEY_KP_MULTIPLY,        // 106: Multiply Key                        VK_MULTIPLY
		KEY_KP_ADD,             // 107: Add Key                             VK_ADD
		KEY_KP_EQUAL,           // 108: Equal Key                           VK_SEPARATOR
		KEY_KP_SUBTRACT,        // 109: Subtract key                        VK_SUBTRACT
		KEY_KP_DECIMAL,         // 110: Decimal key                         VK_DECIMAL
		KEY_KP_DIVIDE,          // 111: Divide key                          VK_DIVIDE
		KEY_F1,                 // 112: F1 key                              VK_F1
		KEY_F2,                 // 113: F2 key                              VK_F2
		KEY_F3,                 // 114: F3 key                              VK_F3
		KEY_F4,                 // 115: F4 key                              VK_F4
		KEY_F5,                 // 116: F5 key                              VK_F5
		KEY_F6,                 // 117: F6 key                              VK_F6
		KEY_F7,                 // 118: F7 key                              VK_F7
		KEY_F8,                 // 119: F8 key                              VK_F8
		KEY_F9,                 // 120: F9 key                              VK_F9
		KEY_F10,                // 121: F10 key                             VK_F10
		KEY_F11,                // 122: F11 key                             VK_F11
		KEY_F12,                // 123: F12 key                             VK_F12

		// 124-135: F13-F24 keys                                            VK_F13-VK_F24
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
		// 136-143: Reserved
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,

		KEY_NUM_LOCK,           // 144: Num lock key                        VK_NUMLOCK
		KEY_SCROLL_LOCK,        // 145: Scroll lock key                     VK_SCROLL

		// 146-150: OEM specific
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
		// 151-159: Unassigned
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,

		KEY_LEFT_SHIFT,         // 160: Left Shift Key                      VK_LSHIFT
		KEY_RIGHT_SHIFT,        // 161: Right Shift Key                     VK_RSHIFT
		KEY_LEFT_CONTROL,       // 162: Left Ctrl Key                       VK_LCONTROL
		KEY_RIGHT_CONTROL,      // 163: Right Ctrl Key                      VK_RCONTROL
		KEY_LEFT_ALT,           // 164: Left Alt Key                        VK_LMENU
		KEY_RIGHT_ALT,          // 165: Right Alt Key                       VK_RMENU

		// 166-185: Not Implemented
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,

		KEY_SEMICOLON,          // 186: ; Key                               VK_OEM_1
		KEY_EQUAL,              // 187: = Key                               VK_OEM_PLUS
		KEY_COMMA,              // 188: , Key                               VK_OEM_COMMA
		KEY_MINUS,              // 189: - Key                               VK_OEM_MINUS
		KEY_PERIOD,             // 190: . Key                               VK_OEM_PERIOD
		KEY_SLASH,              // 191: / Key                               VK_OEM_2
		KEY_GRAVE,              // 192: ` Key                               VK_OEM_3

		// 193-218 Reserved
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,

		KEY_LEFT_BRACKET,       // 219: [ Key                               VK_OEM_4
		KEY_BACKSLASH,          // 220: \ Key                               VK_OEM_5
		KEY_RIGHT_BRACKET,      // 221: ] Key                               VK_OEM_6
		KEY_APOSTROPHE,         // 222: ' Key                               VK_OEM_7

		// 223-254 Not Implemented
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
		KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
		KEY_NULL, KEY_NULL,
	};
	//endregion

	//region Message Handing
	KeyStrokeFlags GetKeyStokeFlags(WPARAM wparam, LPARAM lparam);

	LRESULT Platform_HandleEvents_WindowClose       (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowShow        (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowMove        (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowResize      (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowFocused     (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowUnFocused   (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_Write             (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_KeyDown           (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_KeyUp             (Window* window, WPARAM wparam, LPARAM lparam);

	LRESULT Platform_HandleEvents_MouseDown         (Window* window, WPARAM wparam, LPARAM lparam, KeyCode keyCode);
	LRESULT Platform_HandleEvents_MouseUp           (Window* window, WPARAM wparam, LPARAM lparam, KeyCode keyCode);

	LRESULT Platform_HandleEvents_MouseExtraDown    (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_MouseExtraUp      (Window* window, WPARAM wparam, LPARAM lparam);

	//endregion

	uint32_t GetStyleID(WindowStyle style)
	{
		switch (style)
		{
			case WindowStyle::Default: return WS_SIZEBOX | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
		}
		return 0;
	}

	LRESULT CALLBACK Platform_WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		Window* window = (Window*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		switch (msg)
		{
			//Window
			case WM_CLOSE:         return Platform_HandleEvents_WindowClose      (window, wparam, lparam);
			case WM_SHOWWINDOW:    return Platform_HandleEvents_WindowShow       (window, wparam, lparam);
			case WM_MOVE:          return Platform_HandleEvents_WindowMove       (window, wparam, lparam);
			case WM_SIZE:          return Platform_HandleEvents_WindowResize     (window, wparam, lparam);
			case WM_SETFOCUS:      return Platform_HandleEvents_WindowFocused    (window, wparam, lparam);
			case WM_KILLFOCUS:     return Platform_HandleEvents_WindowUnFocused  (window, wparam, lparam);

			//KeyBoard
			case WM_CHAR:          return Platform_HandleEvents_Write            (window, wparam, lparam);
			case WM_KEYDOWN:       return Platform_HandleEvents_KeyDown          (window, wparam, lparam);
			case WM_KEYUP:         return Platform_HandleEvents_KeyUp            (window, wparam, lparam);

			//Mouse
			case WM_LBUTTONDOWN:   return Platform_HandleEvents_MouseDown        (window, wparam, lparam, KEY_BUTTON_LEFT);
			case WM_RBUTTONDOWN:   return Platform_HandleEvents_MouseDown        (window, wparam, lparam, KEY_BUTTON_RIGHT);
			case WM_MBUTTONDOWN:   return Platform_HandleEvents_MouseDown        (window, wparam, lparam, KEY_BUTTON_MIDDLE);
			case WM_XBUTTONDOWN:   return Platform_HandleEvents_MouseExtraDown   (window, wparam, lparam);

			case WM_LBUTTONUP:     return Platform_HandleEvents_MouseUp          (window, wparam, lparam, KEY_BUTTON_LEFT);
			case WM_RBUTTONUP:     return Platform_HandleEvents_MouseUp          (window, wparam, lparam, KEY_BUTTON_RIGHT);
			case WM_MBUTTONUP:     return Platform_HandleEvents_MouseUp          (window, wparam, lparam, KEY_BUTTON_MIDDLE);
			case WM_XBUTTONUP:     return Platform_HandleEvents_MouseExtraUp     (window, wparam, lparam);
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

	bool Platform_ConvertSize(Window* window, glm::ivec2& size)
	{
		uint32_t currentStyle = GetStyleID(window->m_Style);
		RECT targetRect = { 0, 0, size[0], size[1] };
		bool result = AdjustWindowRectEx(&targetRect, currentStyle, false, 0);
		size[0] = targetRect.right - targetRect.left;
		size[1] = targetRect.bottom - targetRect.top;
		return result;
	}

	//region Window

	Window::Window(const WindowData& data, const Ref<Window>& parent)
	{
		HINSTANCE hinstance = GetModuleHandle(nullptr);
		uint32_t currentStyle = GetStyleID(data.style);

//		window->prevKeyState = KEY_NULL;

		m_Title = data.title;
		m_Style = data.style;
		m_Parent = parent;
		m_ShouldClose = false;
		m_Flags = (WindowFlags)0;

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

		if(m_Handle == nullptr)
		{
			log_fatal("Unable to create window titled {0}", data.title.c_str());
			m_ShouldClose = true;
			throw;
		}

		SetFlags(data.flags);
		Focus();
	}

	Window::~Window()
	{
		DestroyWindow((HWND)m_Handle);
	}

	glm::ivec2 Window::GetSize() const
	{
		LPRECT rect = {};
		GetWindowRect((HWND)m_Handle, rect);

		return { (int32_t)rect->right - (int32_t)rect->left, (int32_t)rect->bottom - (int32_t)rect->top };
	}
	glm::ivec2 Window::GetPosition() const
	{
		LPRECT rect = {};
		GetWindowRect((HWND)m_Handle, rect);

		return { (int32_t)rect->left, (int32_t)rect->top };
	}
	IRect Window::GetRect() const
	{
		LPRECT rect = {};
		GetWindowRect((HWND)m_Handle, rect);

		return IRect
		{
			.position = { (int32_t)rect->left, (int32_t)rect->top },
			.size = { (int32_t)rect->right - (int32_t)rect->left, (int32_t)rect->bottom - (int32_t)rect->top }
		};
	}

	WindowFlags Window::GetFlags() const { return m_Flags; }
	WindowStyle Window::GetStyle() const { return m_Style; }

	void Window::SetSize(glm::ivec2 size) const
	{
		SetWindowPos((HWND)m_Handle, nullptr, 0, 0, size[0], size[1], SWP_NOMOVE);
	}
	void Window::SetPosition(glm::ivec2 position) const
	{
		SetWindowPos((HWND)m_Handle, nullptr, position[0], position[1], 0, 0, SWP_NOSIZE);
	}
	void Window::SetRect(IRect rect)
	{
		glm::ivec2 convertedSize = rect.size;
		Platform_ConvertSize(this, convertedSize);

		SetWindowPos((HWND)m_Handle, nullptr, rect.position[0], rect.position[1], convertedSize[0], convertedSize[1], 0);
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

	void Platform_SendEvent(Window* window, KeyCode keyCode, KeyState state)
	{
		uint32_t eventCount = window->m_KeyEventSubscriptions.size();
		for (uint32_t i = 0; i < eventCount; ++i)
		{
			KeySubscriptionData* element = window->m_KeyEventSubscriptions[i];
			if((element->keyCode & keyCode) && ((int32_t)element->state & (int32_t)state)) element->callback(nullptr);
		}
	}

	void Platform_HandleEvents(std::vector<Ref<Window>>& windows, EngineUpdateType updateType)
	{
		uint32_t windowCount = windows.size();
		for (uint32_t i = 0; i < windowCount; ++i)
		{
			const Ref<Window>& window = windows[i];
			window->m_WriteRecords.clear();
			KeyCode currentKeyDowns = window->m_PrevKeyState;

			while (currentKeyDowns != KEY_NULL)
			{
				uint32_t bitIndex = TrailingZeros128(currentKeyDowns);
				KeyCode keyCode = ((KeyCode)1) << bitIndex;
				currentKeyDowns &= ~keyCode;

				Platform_SendEvent(window.get(), keyCode, KeyState::DownContinues);
			}
		}

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

	KeyStrokeFlags GetKeyStokeFlags(WPARAM wparam, LPARAM lparam)
	{
		WORD virtualKeyCode = LOWORD(wparam);
		WORD repeatCount = LOWORD(lparam);
		WORD keyFlags = HIWORD(lparam);
		WORD scanCode = LOBYTE(keyFlags);

		switch (virtualKeyCode)
		{
			case VK_SHIFT: case VK_CONTROL: case VK_MENU:
				virtualKeyCode = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
				break;
			default: break;
		}

		BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;
		if (isExtendedKey) scanCode = MAKEWORD(scanCode, 0xE0);

		KeyStrokeFlags flags =
		{
			.keyCode = KEY_CODES[virtualKeyCode],
			.virtualKeyCode = virtualKeyCode,
			.repeatCount = repeatCount,
			.scanCode = scanCode,
			.isExtendedKey = (bool)isExtendedKey,
			.wasPrevStateDown = (keyFlags & (1u << 14u)) > 0,
			.isBeingPressed = (keyFlags & (1u << 15u)) > 0,
		};
		return flags;
	}

	void Platform_SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value)
	{
		*flags = (WindowFlags)(((int32_t)*flags & ~(int32_t)target) | (-value & (int32_t)target));
	}

	LRESULT Platform_HandleEvents_WindowClose(Window* window,
	                                          __attribute__((unused)) WPARAM wparam,
	                                          __attribute__((unused)) LPARAM lparam)
	{
		window->Close();
		return 0;
	}

	LRESULT Platform_HandleEvents_WindowShow(Window* window, WPARAM wparam,
	                                         __attribute__((unused)) LPARAM lparam)
	{
		Platform_SetWindowFlag(&window->m_Flags, WindowFlags::Visible, (bool)wparam);
		return 0;
	}

	LRESULT Platform_HandleEvents_WindowMove(Window* window,
	                                         __attribute__((unused)) WPARAM wparam,
	                                         LPARAM lparam)
	{
		glm::ivec2 position =
		{
			(int32_t)(int16_t)LOWORD(lparam),
			(int32_t)(int16_t)HIWORD(lparam)
		};
		return 0;
	}

	LRESULT Platform_HandleEvents_WindowResize(Window* window,
	                                           __attribute__((unused)) WPARAM wparam,
	                                           LPARAM lparam)
	{
		glm::ivec2 size =
		{
			(int32_t)(int16_t)LOWORD(lparam),
			(int32_t)(int16_t)HIWORD(lparam)
		};
		return 0;
	}

	LRESULT Platform_HandleEvents_WindowFocused(Window* window,
	                                            __attribute__((unused)) WPARAM wparam,
	                                            __attribute__((unused)) LPARAM lparam)
	{
		log_info("Window Focused");
		return 0;
	}

	LRESULT Platform_HandleEvents_WindowUnFocused(Window* window,
	                                              __attribute__((unused)) WPARAM wparam,
	                                              __attribute__((unused)) LPARAM lparam)
	{
		log_info("Window UnFocused");
		return 0;
	}

	LRESULT Platform_HandleEvents_Write(Window* window, WPARAM wparam,
	                                    __attribute__((unused)) LPARAM lparam)
	{
		char writtenCharacter = (char)wparam;
		window->m_WriteRecords.push_back(writtenCharacter);
		return 0;
	}

	void Platform_HandleEvents_DownHandling(Window* window, KeyCode keyCode)
	{
		if(!(window->m_PrevKeyState & keyCode))
		{
			window->m_PrevKeyState |= keyCode;
			Platform_SendEvent(window, keyCode, KeyState::Down);
		}
	}

	void Platform_HandleEvents_UpHandling(Window* window, KeyCode keyCode)
	{
		if(window->m_PrevKeyState & keyCode)
		{
			window->m_PrevKeyState &= ~keyCode;
			Platform_SendEvent(window, keyCode, KeyState::Up);
		}
	}

	LRESULT Platform_HandleEvents_KeyDown(Window* window, WPARAM wparam, LPARAM lparam)
	{
		KeyStrokeFlags flags = GetKeyStokeFlags(wparam, lparam);
		Platform_HandleEvents_DownHandling(window, flags.keyCode);
		return 0;
	}

	LRESULT Platform_HandleEvents_KeyUp(Window* window, WPARAM wparam, LPARAM lparam)
	{
		KeyStrokeFlags flags = GetKeyStokeFlags(wparam, lparam);
		Platform_HandleEvents_UpHandling(window, flags.keyCode);
		return 0;
	}

	LRESULT Platform_HandleEvents_MouseDown(Window* window, WPARAM wparam, LPARAM lparam, KeyCode keyCode)
	{
		Platform_HandleEvents_DownHandling(window, keyCode);
		return 0;
	}

	LRESULT Platform_HandleEvents_MouseUp(Window* window, WPARAM wparam, LPARAM lparam, KeyCode keyCode)
	{
		Platform_HandleEvents_UpHandling(window, keyCode);
		return 0;
	}

	LRESULT Platform_HandleEvents_MouseExtraDown(Window* window, WPARAM wparam, LPARAM lparam)
	{
		if(wparam & MK_XBUTTON1) Platform_HandleEvents_MouseDown(window, wparam, lparam, KEY_BUTTON_EXTRA1);
		if(wparam & MK_XBUTTON2) Platform_HandleEvents_MouseDown(window, wparam, lparam, KEY_BUTTON_EXTRA2);
		return 0;
	}

	LRESULT Platform_HandleEvents_MouseExtraUp(Window* window, WPARAM wparam, LPARAM lparam)
	{
		if(wparam & MK_XBUTTON1) Platform_HandleEvents_MouseUp(window, wparam, lparam, KEY_BUTTON_EXTRA1);
		if(wparam & MK_XBUTTON2) Platform_HandleEvents_MouseUp(window, wparam, lparam, KEY_BUTTON_EXTRA2);
		return 0;
	}

//endregion
}