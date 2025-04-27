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
		
		ivec2 position;
		vec2 scrollDelta = vec2{ 0, 0 };
		std::bitset<8> states;
		std::queue<Event> buffer;
		bool isInClientRegion;
	};
}

#endif //HMOUSE_H
