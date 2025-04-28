#ifndef HKEYBOARD_H
#define HKEYBOARD_H

#include "hshared.h"

namespace hf
{
	struct Keyboard
	{
		struct Event
		{
			enum class Type { Invalid, Press, Release, };
			Event();
			Event(Key key, Type t);
			
			[[nodiscard]] bool IsValid() const noexcept;
		
			Key key;
			Type type;
		};
		
		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;
		
		bool autoRepeatEnabled = false;
		std::bitset<256> states = 0;
		std::queue<Event> buffer;
		std::queue<char> charBuffer;
	};

	void KeyboardEvent_Key(Keyboard& keyboard, Key key, Keyboard::Event::Type type) noexcept;
	void KeyboardEvent_Char(Keyboard& keyboard, char character) noexcept;
}

#endif //HKEYBOARD_H
