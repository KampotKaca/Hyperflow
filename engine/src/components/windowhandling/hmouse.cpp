#define private public
#include "components/windowhandling/hmouse.h"
#undef private

namespace hf
{
	Mouse::Event::Event() : m_Button(Button::None), m_Type(Type::Invalid) { }
	Mouse::Event::Event(Button button, Mouse::Event::Type type) : m_Button(button), m_Type(type) { }
	
	Button Mouse::Event::GetButton() const noexcept { return m_Button; }
	Mouse::Event::Type Mouse::Event::GetType() const noexcept { return m_Type; }
	
	[[maybe_unused]] glm::ivec2 Mouse::GetPosition() const noexcept
	{
		return glm::ivec2();
	}
	
	bool Mouse::IsPressed(Button button) const noexcept
	{
		return false;
	}
	
	bool Mouse::IsEmpty() const noexcept
	{
		return false;
	}
	
	Mouse::Event Mouse::Read() const noexcept
	{
		return Mouse::Event();
	}
	
	void Mouse::Dispose() noexcept
	{
		m_Buffer = std::queue<Event>();
	}
	
	void OnButtonPressed(Mouse& mouse, Button button) noexcept
	{
	
	}
	
	void OnButtonReleased(Mouse& mouse, Button button) noexcept
	{
	
	}
	
	void OnMouseMoved(Mouse& mouse, glm::ivec2 position) noexcept
	{
	
	}
	
	void OnMouseWheelScroll(Mouse& mouse, glm::vec2 scroll) noexcept
	{
	
	}
}