#ifndef HKEYBOARD_H
#define HKEYBOARD_H

#include "hshared.h"

namespace hf
{
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
			[[nodiscard]] bool IsValid() const noexcept;
		
		private:
			Key m_Key;
			Type m_Type;
		};
		
		Keyboard() = default;
		
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
