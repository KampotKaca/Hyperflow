#ifndef HKEYBOARD_H
#define HKEYBOARD_H

#include "hshared.h"

namespace hf
{
	enum class Key : uint8_t
	{
		None,
		Backspace,
		Tab,
		Enter,
		Shift,
		Control,
		Alt,
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
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		Apostrophe,
		NumLock,
		ScrollLock,
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
	};
	
	class Keyboard
	{
	public:
		class Event
		{
		public:
			enum class Type { Invalid, Press, Release, };
			Event();
			Event(Key key, Type type);
			
			[[nodiscard]] Key GetKey() const noexcept;
			[[nodiscard]] Type GetType() const noexcept;
		
		private:
			Key m_Key;
			Type m_Type;
		};
		
		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;
		
		[[nodiscard]] bool IsPressed(Key key) const noexcept;
		[[nodiscard]] bool IsEmpty() const noexcept;
		[[nodiscard]] bool CharIsEmpty() const noexcept;
		[[nodiscard]] bool IsAutoRepeatEnabled() const noexcept;
		
		Event Read() noexcept;
		void Dispose() noexcept;
		char ReadChar() noexcept;
		void DisposeChar() noexcept;
		void DisposeAll() noexcept;
		
		void SetAutoRepeat(bool enable) noexcept;
		
	private:
		bool m_AutoRepeatEnabled = false;
		std::bitset<256> m_States;
		std::queue<Event> m_Buffer;
		std::queue<char> m_CharBuffer;
	};
}

#endif //HKEYBOARD_H
