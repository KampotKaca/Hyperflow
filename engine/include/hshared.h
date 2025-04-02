#ifndef HSHARED_H
#define HSHARED_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdarg>
#include <vector>
#include <set>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <bitset>
#include <queue>
#include <optional>

#include "glm/fwd.hpp"

extern "C"
{
#include "log.h"
};

#include <sys/stat.h>
#include <sys/types.h>
#include <memory>

#include "components/hmath.h"

#if PLATFORM_LINUX
#ifdef None
#undef None;
#endif
#endif

namespace hf
{
	//region Aditionalls
	typedef void (*ActionCallback)(void* params);

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T>
	using URef = std::unique_ptr<T>;

	template <typename T, typename... Args>
	Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	Ref<T> MakeURef(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	//endregion

	//region Definitions

	enum class Key : uint8_t
	{
		None,
		Backspace,
		Tab,
		Enter,
		Pause,
		CapsLock,
		Escape,
		Space,
		PageUp, PageDown,
		End, Home,
		Left, Up, Right, Down,
		PrintScreen,
		Insert,
		Delete,
		Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
		LeftSuper, RightSuper,
		Pad0, Pad1, Pad2, Pad3, Pad4, Pad5, Pad6, Pad7, Pad8, Pad9,
		PadMultiply,
		PadAdd,
		PadEqual,
		PadSubtract,
		PadDecimal,
		PadDivide,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		LeftBracket,
		Backslash,
		RightBracket,
		Apostrophe,
		NumLock,
		ScrollLock,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		LeftShift, RightShift,
		LeftControl, RightControl,
		LeftAlt, RightAlt,
		Semicolon,
		Equal,
		Comma,
		Minus,
		Period,
		Slash,
		Grave,
		Count
	};

	enum class Button : uint8_t { None, Left, Right, Wheel, Extra1, Extra2, Count };

	enum class KeyState { None, Down, DownContinues, Up };
	typedef KeyState ButtonState;

	struct Rect
	{
		vec2 position;
		vec2 size;
	};

	struct IRect
	{
		ivec2 position;
		ivec2 size;
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
		ivec2 position;
		ivec2 size;
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
