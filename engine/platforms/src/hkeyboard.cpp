#define private public
#include "hkeyboard.h"
#undef private

#include "components/hinternal.h"
#include "hyperconfig.h"

namespace hf
{
	Keyboard::Event::Event() : m_Key(Key::None), m_Type(Type::Invalid) { }
	Keyboard::Event::Event(Key key, Type type) : m_Key(key), m_Type(type) { }
	Key Keyboard::Event::GetKey() const noexcept { return m_Key; }
	Keyboard::Event::Type Keyboard::Event::GetType() const noexcept { return m_Type; }
	bool Keyboard::Event::IsValid() const noexcept { return m_Key != Key::None && m_Type != Type::Invalid; }
	
	bool Keyboard::IsPressed(Key key) const noexcept { return m_States[(uint8_t)key]; }
	
	Keyboard::Event Keyboard::Read() noexcept
	{
		if(!m_Buffer.empty())
		{
			Event e = m_Buffer.front();
			m_Buffer.pop();
			return e;
		}
		return {};
	}
	
	bool Keyboard::IsEmpty() const noexcept { return m_Buffer.empty(); }
	void Keyboard::Dispose() noexcept { m_Buffer = std::queue<Event>(); }
	
	void Keyboard::SetAutoRepeat(bool enable) noexcept { m_AutoRepeatEnabled = enable; }
	bool Keyboard::IsAutoRepeatEnabled() const noexcept { return m_AutoRepeatEnabled; }
	
	char Keyboard::ReadChar() noexcept
	{
		if(!m_CharBuffer.empty())
		{
			char code = m_CharBuffer.front();
			m_CharBuffer.pop();
			return code;
		}
		return 0;
	}
	
	bool Keyboard::CharIsEmpty() const noexcept { return m_CharBuffer.empty(); }
	void Keyboard::DisposeChar() noexcept { m_CharBuffer = std::queue<char>(); }
	
	void Keyboard::DisposeAll() noexcept
	{
		Dispose();
		DisposeChar();
	}

	void KeyboardEvent_Key(const Ref<Keyboard>& keyboard, Key key, Keyboard::Event::Type type) noexcept
	{
		if(type == Keyboard::Event::Type::Invalid) return;

		keyboard->m_States[(uint8_t)key] = type == Keyboard::Event::Type::Press;
		keyboard->m_Buffer.emplace( key, type );
	}
	
	void KeyboardEvent_Char(const Ref<Keyboard>& keyboard, char character) noexcept
	{
		keyboard->m_CharBuffer.push(character);
	}
}