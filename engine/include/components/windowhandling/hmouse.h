#ifndef HMOUSE_H
#define HMOUSE_H

#include "hshared.h"

namespace hf
{
	enum class Button : uint8_t { None, Left, Right, Wheel, Extra1, Extra2 };
	
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
		
		Mouse() = default;
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;
		
		[[nodiscard]] glm::ivec2 GetPosition() const noexcept;
		[[nodiscard]] glm::ivec2 GetScroll() const noexcept;
		[[nodiscard]] bool IsPressed(Button button) const noexcept;
		[[nodiscard]] bool IsEmpty() const noexcept;
		
		[[nodiscard]] Event Read() noexcept;
		
		void Dispose() noexcept;
		
	private:
		glm::ivec2 m_Position, m_Scroll;
		std::bitset<8> m_States;
		std::queue<Event> m_Buffer;
	};
}

#endif //HMOUSE_H
