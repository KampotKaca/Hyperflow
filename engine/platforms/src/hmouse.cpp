#include "hmouse.h"

namespace hf
{
	Mouse::Mouse(ivec2 position, bool isInClientRegion) : position(position), isInClientRegion(isInClientRegion) { }
	
	Mouse::Event::Event() : button(Button::None), type(Type::Invalid) { }
	Mouse::Event::Event(Button button, Type type) : button(button), type(type) { }

	bool Mouse::Event::IsValid() const noexcept { return button != Button::None && type != Type::Invalid; }

	bool IsValid(const Mouse::Event& event) { return event.button != Button::None && event.type != Mouse::Event::Type::Invalid; }

	void MouseEvent_Button(Mouse& mouse, Button button, Mouse::Event::Type type) noexcept
	{
		if(type == Mouse::Event::Type::Invalid) return;

		mouse.states[(uint8_t)button] = type == Mouse::Event::Type::Press;
		mouse.buffer.emplace(button, type);
	}
	
	void MouseEvent_Moved(Mouse& mouse, ivec2 position) noexcept
	{
		mouse.position = position;
	}

	void MouseEvent_Scroll(Mouse& mouse, vec2 delta) noexcept
	{
		mouse.scrollDelta += delta;
	}
}