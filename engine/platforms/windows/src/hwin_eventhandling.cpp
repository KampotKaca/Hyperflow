#include "hwin_shared.h"
#include "hwin_eventhandling.h"
#include "hplatform.h"

namespace hf
{
	//region Definitions

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
		(uint8_t)Key::LeftShift,          // 16: Shift Key                            VK_SHIFT
		(uint8_t)Key::LeftControl,        // 17: Ctrl Key                             VK_CONTROL
		(uint8_t)Key::LeftAlt,            // 18: Alt Key                              VK_MENU
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

	//region Message Handing Impl

	LRESULT Platform_HandleEvents_WindowClose(Window* window,
	                                          __attribute__((unused)) WPARAM wparam,
	                                          __attribute__((unused)) LPARAM lparam) noexcept
	{
		WindowEvent_Close(window);
		return 0;
	}

	LRESULT Platform_HandleEvents_WindowShow(Window* window, WPARAM wparam,
	                                         __attribute__((unused)) LPARAM lparam) noexcept
	{
		WindowEvent_Show(window, (bool)wparam);
		return 0;
	}

	LRESULT Platform_HandleEvents_WindowMove(Window* window,
	                                         __attribute__((unused)) WPARAM wparam,
	                                         LPARAM lparam) noexcept
	{
		ivec2 position =
		{
			GET_X_LPARAM(lparam),
			GET_Y_LPARAM(lparam)
		};

		WindowEvent_Move(window, position);
		return 0;
	}

	LRESULT Platform_HandleEvents_WindowResize(Window* window,
	                                           __attribute__((unused)) WPARAM wparam,
	                                           LPARAM lparam) noexcept
	{
		ivec2 size =
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
	                                          bool focused) noexcept
	{
		WindowEvent_Focus(window, focused);
		return 0;
	}

	LRESULT Platform_HandleEvents_KeyboardChar(Window* window, WPARAM wparam,
	                                           __attribute__((unused)) LPARAM lparam) noexcept
	{
		KeyboardEvent_Char(window->keyboard, (char) wparam);
		return 0;
	}

	LRESULT Platform_HandleEvents_KeyboardKey(Window* window, WPARAM wparam, LPARAM lparam,
	                                          Keyboard::Event::Type type) noexcept
	{
		auto key = (Key)(KEY_CODES[(uint8_t)wparam]);

		switch (type)
		{

			case Keyboard::Event::Type::Invalid: LOG_WARN("Invalid Event thrown"); break;
			case Keyboard::Event::Type::Press:

				if(!(lparam & 0x40000000) || window->keyboard.autoRepeatEnabled)
					KeyboardEvent_Key(window->keyboard, key, type);

				break;
			case Keyboard::Event::Type::Release:
				if(window->keyboard.states[(uint8_t)key]) KeyboardEvent_Key(window->keyboard, key, type);
				break;
		}
		return 0;
	}

	LRESULT Platform_HandleEvents_MouseButton(Window* window, WPARAM wparam, LPARAM lparam, Button button, Mouse::Event::Type type) noexcept
	{
		MouseEvent_Button(window->mouse, button, type);
		return 0;
	}

	LRESULT Platform_HandleEvents_MouseButtonExtra(Window* window, WPARAM wparam, LPARAM lparam, Mouse::Event::Type type) noexcept
	{
		if(wparam & MK_XBUTTON1) MouseEvent_Button(window->mouse, Button::Extra1, type);
		if(wparam & MK_XBUTTON2) MouseEvent_Button(window->mouse, Button::Extra2, type);
		return 0;
	}

	LRESULT Platform_HandleEvents_MouseMove(Window* window, WPARAM wparam, LPARAM lparam) noexcept
	{
		const POINTS pt = MAKEPOINTS(lparam);
		auto rect = window->rect;

		if(pt.x >= 0 && pt.x < rect.size.x && pt.y > 0 && pt.y < rect.size.y)
		{
			MouseEvent_Moved(window->mouse, ivec2{ pt.x, pt.y });
			if(!window->mouse.isInClientRegion)
			{
				SetCapture((HWND)window->handle);
				window->mouse.isInClientRegion = true;
			}
		}
		else
		{
			if(wparam & (MK_LBUTTON | MK_RBUTTON)) MouseEvent_Moved(window->mouse, ivec2{ pt.x, pt.y });
			else
			{
				ReleaseCapture();
				window->mouse.isInClientRegion = false;
			}
		}

		ivec2 position =
		{
			GET_X_LPARAM(lparam),
			GET_Y_LPARAM(lparam)
		};
		MouseEvent_Moved(window->mouse, position);

		return 0;
	}

	LRESULT Platform_HandleEvents_MouseScroll(Window* window, WPARAM wparam, LPARAM lparam, ivec2 direction) noexcept
	{
		auto delta = GET_WHEEL_DELTA_WPARAM(wparam) / (float)WHEEL_DELTA;

		MouseEvent_Scroll(window->mouse, (vec2)direction * delta);

		return 0;
	}

	LRESULT Platform_HandleEvents_WindowTitle(Window *window, WPARAM wparam, LPARAM lparam) noexcept
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
