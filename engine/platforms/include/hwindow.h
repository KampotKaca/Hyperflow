#ifndef HWINDOW_H
#define HWINDOW_H

#include "hshared.h"
#include "hmouse.h"
#include "hkeyboard.h"
#include "hrenderer.h"

namespace hf
{
	struct Window
	{
		Window(const WindowData& data, const Ref<Window>& parent);
		~Window();

		struct EventData
		{
			KeyState keyStates[(uint8_t)Key::Count]{};
			ButtonState buttonStates[(uint8_t)Button::Count]{};
			std::string charData;
			ivec2 pointerPosition;
			ivec2 pointerDelta;
			vec2 scrollDelta;
		};

		std::string title{};
		IRect rect{};
		void* handle{};
		WindowStyle style{};
		WindowFlags flags{};
		Ref<Window> parent{};

		Keyboard keyboard{};
		Mouse mouse{};
		EventData eventData{};
		URef<Renderer> renderer{};
	};
}

#endif //HWINDOW_H
