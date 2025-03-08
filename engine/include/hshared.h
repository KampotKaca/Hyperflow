#ifndef HSHARED_H
#define HSHARED_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdarg>
#include <vector>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

extern "C"
{
#include "log.h"
};

#include <sys/stat.h>
#include <sys/types.h>

#include <cinttypes>
#include <memory>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include "glm/mat4x4.hpp"

namespace hf
{
	//region Aditionalls

	typedef void (*ActionCallback)(void* params);
	typedef __int128 KeyCode;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename... Args>
	Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	//endregion

	//region Input

	#define BIT_SET_RANGE(from, to) (((KeyCode)1 << (to - from + 1)) - 1) << from

	#define KEY_NULL                      0                       // Key: NULL, used for no key pressed

	//Mouse Buttons
	constexpr KeyCode KEY_BUTTON_LEFT     = ((KeyCode)1 << 0);       // Mouse button left
	constexpr KeyCode KEY_BUTTON_RIGHT    = ((KeyCode)1 << 1);       // Mouse button right
	constexpr KeyCode KEY_BUTTON_MIDDLE   = ((KeyCode)1 << 2);       // Mouse button middle (pressed wheel)
	constexpr KeyCode KEY_BUTTON_EXTRA1   = ((KeyCode)1 << 3);       // Mouse button extra (advanced mouse device)
	constexpr KeyCode KEY_BUTTON_EXTRA2   = ((KeyCode)1 << 4);       // Mouse button extra (advanced mouse device)

	// Alphanumeric keys
	constexpr KeyCode KEY_APOSTROPHE      = ((KeyCode)1 << 5);       // Key: '
	constexpr KeyCode KEY_COMMA           = ((KeyCode)1 << 6);       // Key: ,
	constexpr KeyCode KEY_MINUS           = ((KeyCode)1 << 7);       // Key: -
	constexpr KeyCode KEY_PERIOD          = ((KeyCode)1 << 8);       // Key: .
	constexpr KeyCode KEY_SLASH           = ((KeyCode)1 << 9);       // Key: /
	constexpr KeyCode KEY_ZERO            = ((KeyCode)1 << 10);      // Key: 0
	constexpr KeyCode KEY_ONE             = ((KeyCode)1 << 11);      // Key: 1
	constexpr KeyCode KEY_TWO             = ((KeyCode)1 << 12);      // Key: 2
	constexpr KeyCode KEY_THREE           = ((KeyCode)1 << 13);      // Key: 3
	constexpr KeyCode KEY_FOUR            = ((KeyCode)1 << 14);      // Key: 4
	constexpr KeyCode KEY_FIVE            = ((KeyCode)1 << 15);      // Key: 5
	constexpr KeyCode KEY_SIX             = ((KeyCode)1 << 16);      // Key: 6
	constexpr KeyCode KEY_SEVEN           = ((KeyCode)1 << 17);      // Key: 7
	constexpr KeyCode KEY_EIGHT           = ((KeyCode)1 << 18);      // Key: 8
	constexpr KeyCode KEY_NINE            = ((KeyCode)1 << 19);      // Key: 9
	constexpr KeyCode KEY_SEMICOLON       = ((KeyCode)1 << 20);      // Key: ;
	constexpr KeyCode KEY_EQUAL           = ((KeyCode)1 << 21);      // Key: =
	constexpr KeyCode KEY_A               = ((KeyCode)1 << 22);      // Key: A | a
	constexpr KeyCode KEY_B               = ((KeyCode)1 << 23);      // Key: B | b
	constexpr KeyCode KEY_C               = ((KeyCode)1 << 24);      // Key: C | c
	constexpr KeyCode KEY_D               = ((KeyCode)1 << 25);      // Key: D | d
	constexpr KeyCode KEY_E               = ((KeyCode)1 << 26);      // Key: E | e
	constexpr KeyCode KEY_F               = ((KeyCode)1 << 27);      // Key: F | f
	constexpr KeyCode KEY_G               = ((KeyCode)1 << 28);      // Key: G | g
	constexpr KeyCode KEY_H               = ((KeyCode)1 << 29);      // Key: H | h
	constexpr KeyCode KEY_I               = ((KeyCode)1 << 30);      // Key: I | i
	constexpr KeyCode KEY_J               = ((KeyCode)1 << 31);      // Key: J | j
	constexpr KeyCode KEY_K               = ((KeyCode)1 << 32);      // Key: K | k
	constexpr KeyCode KEY_L               = ((KeyCode)1 << 33);      // Key: L | l
	constexpr KeyCode KEY_M               = ((KeyCode)1 << 34);      // Key: M | m
	constexpr KeyCode KEY_N               = ((KeyCode)1 << 35);      // Key: N | n
	constexpr KeyCode KEY_O               = ((KeyCode)1 << 36);      // Key: O | o
	constexpr KeyCode KEY_P               = ((KeyCode)1 << 37);      // Key: P | p
	constexpr KeyCode KEY_Q               = ((KeyCode)1 << 38);      // Key: Q | q
	constexpr KeyCode KEY_R               = ((KeyCode)1 << 39);      // Key: R | r
	constexpr KeyCode KEY_S               = ((KeyCode)1 << 40);      // Key: S | s
	constexpr KeyCode KEY_T               = ((KeyCode)1 << 41);      // Key: T | t
	constexpr KeyCode KEY_U               = ((KeyCode)1 << 42);      // Key: U | u
	constexpr KeyCode KEY_V               = ((KeyCode)1 << 43);      // Key: V | v
	constexpr KeyCode KEY_W               = ((KeyCode)1 << 44);      // Key: W | w
	constexpr KeyCode KEY_X               = ((KeyCode)1 << 45);      // Key: X | x
	constexpr KeyCode KEY_Y               = ((KeyCode)1 << 46);      // Key: Y | y
	constexpr KeyCode KEY_Z               = ((KeyCode)1 << 47);      // Key: Z | z
	constexpr KeyCode KEY_LEFT_BRACKET    = ((KeyCode)1 << 48);      // Key: [
	constexpr KeyCode KEY_BACKSLASH       = ((KeyCode)1 << 49);      // Key: '\'
	constexpr KeyCode KEY_RIGHT_BRACKET   = ((KeyCode)1 << 50);      // Key: ]
	constexpr KeyCode KEY_GRAVE           = ((KeyCode)1 << 51);      // Key: `

	// Function keys
	constexpr KeyCode KEY_SPACE           = ((KeyCode)1 << 52);      // Key: Space
	constexpr KeyCode KEY_ESCAPE          = ((KeyCode)1 << 53);      // Key: Esc
	constexpr KeyCode KEY_ENTER           = ((KeyCode)1 << 54);      // Key: Enter
	constexpr KeyCode KEY_TAB             = ((KeyCode)1 << 55);      // Key: Tab
	constexpr KeyCode KEY_BACKSPACE       = ((KeyCode)1 << 56);      // Key: Backspace
	constexpr KeyCode KEY_INSERT          = ((KeyCode)1 << 57);      // Key: Ins
	constexpr KeyCode KEY_DELETE          = ((KeyCode)1 << 58);      // Key: Del
	constexpr KeyCode KEY_RIGHT           = ((KeyCode)1 << 59);      // Key: Cursor right
	constexpr KeyCode KEY_LEFT            = ((KeyCode)1 << 60);      // Key: Cursor left
	constexpr KeyCode KEY_DOWN            = ((KeyCode)1 << 61);      // Key: Cursor down
	constexpr KeyCode KEY_UP              = ((KeyCode)1 << 62);      // Key: Cursor up
	constexpr KeyCode KEY_PAGE_UP         = ((KeyCode)1 << 63);      // Key: Page up
	constexpr KeyCode KEY_PAGE_DOWN       = ((KeyCode)1 << 64);      // Key: Page down
	constexpr KeyCode KEY_HOME            = ((KeyCode)1 << 65);      // Key: Home
	constexpr KeyCode KEY_END             = ((KeyCode)1 << 66);      // Key: End
	constexpr KeyCode KEY_CAPS_LOCK       = ((KeyCode)1 << 67);      // Key: Caps lock
	constexpr KeyCode KEY_SCROLL_LOCK     = ((KeyCode)1 << 68);      // Key: Scroll down
	constexpr KeyCode KEY_NUM_LOCK        = ((KeyCode)1 << 69);      // Key: Num lock
	constexpr KeyCode KEY_PRINT_SCREEN    = ((KeyCode)1 << 70);      // Key: Print screen
	constexpr KeyCode KEY_PAUSE           = ((KeyCode)1 << 71);      // Key: Pause
	constexpr KeyCode KEY_F1              = ((KeyCode)1 << 72);      // Key: F1
	constexpr KeyCode KEY_F2              = ((KeyCode)1 << 73);      // Key: F2
	constexpr KeyCode KEY_F3              = ((KeyCode)1 << 74);      // Key: F3
	constexpr KeyCode KEY_F4              = ((KeyCode)1 << 75);      // Key: F4
	constexpr KeyCode KEY_F5              = ((KeyCode)1 << 76);      // Key: F5
	constexpr KeyCode KEY_F6              = ((KeyCode)1 << 77);      // Key: F6
	constexpr KeyCode KEY_F7              = ((KeyCode)1 << 78);      // Key: F7
	constexpr KeyCode KEY_F8              = ((KeyCode)1 << 79);      // Key: F8
	constexpr KeyCode KEY_F9              = ((KeyCode)1 << 80);      // Key: F9
	constexpr KeyCode KEY_F10             = ((KeyCode)1 << 81);      // Key: F10
	constexpr KeyCode KEY_F11             = ((KeyCode)1 << 82);      // Key: F11
	constexpr KeyCode KEY_F12             = ((KeyCode)1 << 83);      // Key: F12
	constexpr KeyCode KEY_LEFT_SHIFT      = ((KeyCode)1 << 84);      // Key: Shift left
	constexpr KeyCode KEY_LEFT_CONTROL    = ((KeyCode)1 << 85);      // Key: Control left
	constexpr KeyCode KEY_LEFT_ALT        = ((KeyCode)1 << 86);      // Key: Alt left
	constexpr KeyCode KEY_LEFT_SUPER      = ((KeyCode)1 << 87);      // Key: Super left
	constexpr KeyCode KEY_RIGHT_SHIFT     = ((KeyCode)1 << 88);      // Key: Shift right
	constexpr KeyCode KEY_RIGHT_CONTROL   = ((KeyCode)1 << 89);      // Key: Control right
	constexpr KeyCode KEY_RIGHT_ALT       = ((KeyCode)1 << 90);      // Key: Alt right
	constexpr KeyCode KEY_RIGHT_SUPER     = ((KeyCode)1 << 91);      // Key: Super right

	// Keypad keys
	constexpr KeyCode KEY_KP_0            = ((KeyCode)1 << 92);      // Key: Keypad 0
	constexpr KeyCode KEY_KP_1            = ((KeyCode)1 << 93);      // Key: Keypad 1
	constexpr KeyCode KEY_KP_2            = ((KeyCode)1 << 94);      // Key: Keypad 2
	constexpr KeyCode KEY_KP_3            = ((KeyCode)1 << 95);      // Key: Keypad 3
	constexpr KeyCode KEY_KP_4            = ((KeyCode)1 << 96);      // Key: Keypad 4
	constexpr KeyCode KEY_KP_5            = ((KeyCode)1 << 97);      // Key: Keypad 5
	constexpr KeyCode KEY_KP_6            = ((KeyCode)1 << 98);      // Key: Keypad 6
	constexpr KeyCode KEY_KP_7            = ((KeyCode)1 << 99);      // Key: Keypad 7
	constexpr KeyCode KEY_KP_8            = ((KeyCode)1 << 100);     // Key: Keypad 8
	constexpr KeyCode KEY_KP_9            = ((KeyCode)1 << 101);     // Key: Keypad 9
	constexpr KeyCode KEY_KP_DECIMAL      = ((KeyCode)1 << 102);     // Key: Keypad .
	constexpr KeyCode KEY_KP_DIVIDE       = ((KeyCode)1 << 103);     // Key: Keypad /
	constexpr KeyCode KEY_KP_MULTIPLY     = ((KeyCode)1 << 104);     // Key: Keypad *
	constexpr KeyCode KEY_KP_SUBTRACT     = ((KeyCode)1 << 105);     // Key: Keypad -
	constexpr KeyCode KEY_KP_ADD          = ((KeyCode)1 << 106);     // Key: Keypad +
	constexpr KeyCode KEY_KP_EQUAL        = ((KeyCode)1 << 107);     // Key: Keypad =

	constexpr KeyCode KEY_SHIFT           = KEY_LEFT_SHIFT | KEY_RIGHT_SHIFT;
	constexpr KeyCode KEY_CONTROL         = KEY_LEFT_CONTROL | KEY_RIGHT_CONTROL;
	constexpr KeyCode KEY_ALT             = KEY_LEFT_ALT | KEY_RIGHT_ALT;

	constexpr KeyCode KEY_MOUSE_ANY        = BIT_SET_RANGE(0, 4);
	constexpr KeyCode KEY_ALPHANUMERIC_ANY = BIT_SET_RANGE(5, 51);
	constexpr KeyCode KEY_FUNCTION_ANY     = BIT_SET_RANGE(52, 91);
	constexpr KeyCode KEY_KEYPAD_ANY       = BIT_SET_RANGE(92, 107);
	constexpr KeyCode KEY_KEYBOARD_ANY     = KEY_ALPHANUMERIC_ANY | KEY_FUNCTION_ANY | KEY_KEYPAD_ANY;
	constexpr KeyCode KEY_ANY              = KEY_MOUSE_ANY | KEY_KEYBOARD_ANY;

	#undef BIT_SET_RANGE

	enum class KeyState
	{
		None            = 0,
		Down            = 1 << 0,
		DownContinues   = 1 << 1,
		Up              = 1 << 2,
		Any             = Down | DownContinues | Up,
	};

	enum class KeyType
	{
		None          = 0,
		Keyboard      = (1 << 0),
		Mouse         = (1 << 1),
		Alphanumeric  = (1 << 2),
		Function      = (1 << 3),
		Keypad        = (1 << 4),

		KEY_TYPE_ALPHANUMERIC_KEY = Keyboard | Alphanumeric,
		KEY_TYPE_FUNCTION_KEY     = Keyboard | Function,
		KEY_TYPE_KEYPAD_KEY       = Keyboard | Keypad,
	};

	struct Rect
	{
		glm::vec2 position;
		glm::vec2 size;
	};

	struct IRect
	{
		glm::ivec2 position;
		glm::ivec2 size;
	};

	//endregion

	//region Window

	enum class WindowStyle
	{
		Default
	};

	enum class WindowFlags
	{
		Visible   = 1 << 0,
		Minimized = 1 << 1,
		Maximized = 1 << 2,

		Default = Visible
	};

	struct WindowData
	{
		std::string title;
		WindowFlags flags;
		WindowStyle style;
		glm::ivec2 position;
		glm::ivec2 size;
	};


	struct KeySubscriptionData
	{
		ActionCallback callback;
		KeyState state;
		KeyCode keyCode;
	};

	//endregion

	//region Engine

	enum class EngineUpdateType
	{
		Continues,   //Updates as often as is allowed, good for games or simulations, try to avoid it if you it's possible
		EventRaised  //Waits updates only on OS events, good for apps which doesn't change without user interaction
	};

	struct EngineLifecycleCallbacks
	{
		void (*onStartCallback)();
		void (*onUpdateCallback)();
		void (*onRenderCallback)();
		void (*onQuitCallback)();
	};

	struct EngineData
	{
		std::string appTitle;
		EngineUpdateType updateType;
		EngineLifecycleCallbacks lifecycleCallbacks;
		WindowData windowData;
	};

	class Window;

	//endregion

	//region Helpers

	uint32_t TrailingZeros64(uint64_t n);
	uint32_t TrailingZeros128(__int128 n);

	//endregion
}

#endif //HSHARED_H
