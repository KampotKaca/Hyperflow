#ifndef HMOUSE_H
#define HMOUSE_H

#include "hshared.h"

namespace hf
{
	class Mouse
	{
	public:
		class Event
		{
		public:
			enum class Type { Invalid, Press, Release };
			
			Event();
			Event(Button button, Type type);
			
			[[nodiscard]] Button GetButton() const noexcept;
			[[nodiscard]] Type GetType() const noexcept;
			[[nodiscard]] bool IsValid() const noexcept;
			
		private:
			Button m_Button;
			Type m_Type;
		};
		
		Mouse(ivec2 position, bool isInClientRegion);
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;
		
		[[nodiscard]] ivec2 GetPosition() const noexcept;
		[[nodiscard]] vec2 GetScrollDelta() const noexcept;
		[[nodiscard]] bool IsPressed(Button button) const noexcept;
		[[nodiscard]] bool IsEmpty() const noexcept;
		
		[[nodiscard]] Event Read() noexcept;
		
		void Dispose() noexcept;
		
	private:
		ivec2 m_Position;
		vec2 m_ScrollDelta = vec2{ 0, 0 };
		std::bitset<8> m_States;
		std::queue<Event> m_Buffer;
		bool m_IsInClientRegion;
	};
}

#endif //HMOUSE_H
