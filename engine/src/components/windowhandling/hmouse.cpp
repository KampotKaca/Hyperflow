#define private public
#include "components/windowhandling/hmouse.h"
#undef private

#include "hyperconfig.h"

namespace hf
{
	Mouse::Mouse(glm::ivec2 position, bool isInClientRegion) : m_Position(position), m_IsInClientRegion(isInClientRegion) { }
	
	Mouse::Event::Event() : m_Button(Button::None), m_Type(Type::Invalid) { }
	Mouse::Event::Event(Button button, Mouse::Event::Type type) : m_Button(button), m_Type(type) { }
	
	Button Mouse::Event::GetButton() const noexcept { return m_Button; }
	Mouse::Event::Type Mouse::Event::GetType() const noexcept { return m_Type; }
	bool Mouse::Event::IsValid() const noexcept { return m_Button != Button::None && m_Type != Type::Invalid; }

	glm::ivec2 Mouse::GetPosition() const noexcept { return m_Position; }
	glm::vec2 Mouse::GetScrollDelta() const noexcept { return m_ScrollDelta; }
	bool Mouse::IsPressed(Button button) const noexcept { return m_States[(uint8_t)button]; }
	bool Mouse::IsEmpty() const noexcept { return m_Buffer.empty(); }
	
	Mouse::Event Mouse::Read() noexcept
	{
		if(!m_Buffer.empty())
		{
			auto e = m_Buffer.front();
			m_Buffer.pop();
			return e;
		}
		return {};
	}
	
	void Mouse::Dispose() noexcept
	{
		m_Buffer = std::queue<Event>();
	}

	void MouseEvent_Button(const Ref<Mouse>& mouse, Button button, Mouse::Event::Type type) noexcept
	{
		if(type == Mouse::Event::Type::Invalid) return;

		mouse->m_States[(uint8_t)button] = type == Mouse::Event::Type::Press;
		mouse->m_Buffer.emplace(button, type);
	}
	
	void MouseEvent_Moved(const Ref<Mouse>& mouse, glm::ivec2 position) noexcept
	{
		mouse->m_Position = position;
	}

	void MouseEvent_Scroll(const Ref<Mouse>& mouse, glm::vec2 delta) noexcept
	{
		mouse->m_ScrollDelta += delta;
	}
}