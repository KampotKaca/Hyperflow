#define private public
#include "components/windowhandling/hkeyboard.h"
#undef private

#include "components/hinternal.h"
#include "hyperconfig.h"

namespace hf
{
	Keyboard::Event::Event() : m_Key(Key::None), m_Type(Type::Invalid) { }
	Keyboard::Event::Event(Key key, Keyboard::Event::Type type) : m_Key(key), m_Type(type) { }
	Key Keyboard::Event::GetKey() const noexcept { return m_Key; }
	Keyboard::Event::Type Keyboard::Event::GetType() const noexcept { return m_Type; }
	bool Keyboard::Event::IsValid() const noexcept { return m_Key != Key::None && m_Type != Type::Invalid; }
	
	bool Keyboard::IsPressed(Key key) const noexcept { return m_States[(uint8_t)key]; }
	
	Keyboard::Event Keyboard::Read() noexcept
	{
		if(!m_Buffer.empty())
		{
			Keyboard::Event e = m_Buffer.front();
			m_Buffer.pop();
			return e;
		}
		return Keyboard::Event();
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
	
	void Keyboard::OnFocusLoss() noexcept
	{
		DisposeChar();
		std::queue<Event> tempBuffer;
		
		while (!IsEmpty())
		{
			auto e = Read();
			if(e.IsValid())
			{
				if(e.GetType() == Event::Type::Release) tempBuffer.push(e);
				else m_States[(uint8_t)e.GetKey()] = false;
			}
		}
		
		for (uint32_t i = 0; i < 256; ++i)
		{
			if(m_States[i]) tempBuffer.push(Event((Key)i, Event::Type::Release));
		}
		
		m_States = 0;
		m_Buffer = tempBuffer;
	}
	
	void OnKeyPressed(Keyboard& keyboard, Key key) noexcept
	{
		if(keyboard.m_Buffer.size() >= MAX_KEYBOARD_BUFFER_SIZE) return;
		keyboard.m_States[(uint8_t)key] = true;
		keyboard.m_Buffer.push(Keyboard::Event(key, Keyboard::Event::Type::Press));
	}
	
	void OnKeyReleased(Keyboard& keyboard, Key key) noexcept
	{
		if(keyboard.m_Buffer.size() >= MAX_KEYBOARD_BUFFER_SIZE) return;
		keyboard.m_States[(uint8_t)key] = false;
		keyboard.m_Buffer.push(Keyboard::Event(key, Keyboard::Event::Type::Release));
	}
	
	void OnChar(Keyboard& keyboard, char character) noexcept
	{
		if(keyboard.m_CharBuffer.size() >= MAX_KEYBOARD_BUFFER_SIZE) return;
		keyboard.m_CharBuffer.push(character);
	}
}