#include "hkeyboard.h"

namespace hf
{
	Keyboard::Event::Event() : key(Key::None), type(Type::Invalid) { }
	Keyboard::Event::Event(Key key, Type t) : key(key), type(t) { }
	bool Keyboard::Event::IsValid() const noexcept { return key != Key::None && type != Type::Invalid; }

	void KeyboardEvent_Key(Keyboard& keyboard, Key key, Keyboard::Event::Type type) noexcept
	{
		if(type == Keyboard::Event::Type::Invalid) return;

		keyboard.states[(uint8_t)key] = type == Keyboard::Event::Type::Press;
		keyboard.buffer.emplace( key, type );
	}
	
	void KeyboardEvent_Char(Keyboard& keyboard, char character) noexcept
	{
		keyboard.charBuffer.push(character);
	}
}