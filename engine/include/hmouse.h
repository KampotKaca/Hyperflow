#ifndef HMOUSE_H
#define HMOUSE_H

#include "hshared.h"

namespace hf
{
	struct Mouse
	{
		struct Event
		{
			enum class Type { Invalid, Press, Release };
			
			Event();
			Event(Button button, Type type);

			[[nodiscard]] bool IsValid() const noexcept;

			Button button;
			Type type;
		};
		
		Mouse() = default;
		Mouse(ivec2 position, bool isInClientRegion);
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;
		
		ivec2 position{};
		vec2 scrollDelta = vec2{ 0, 0 };
		std::bitset<8> states{};
		std::queue<Event> buffer{};
		bool isInClientRegion = false;
	};

	void MouseEvent_Button(Mouse& mouse, Button button, Mouse::Event::Type type) noexcept;
	void MouseEvent_Moved(Mouse& mouse, ivec2 position) noexcept;
	void MouseEvent_Scroll(Mouse& mouse, vec2 delta) noexcept;
}

#endif //HMOUSE_H
