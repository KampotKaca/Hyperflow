#ifndef HWINDOW_H
#define HWINDOW_H

#include "hshared.h"
#include "hrenderer.h"

namespace hf
{
	struct Window
	{
		Window(const WindowCreationInfo& info, const Ref<Window>& parent);
		~Window();

		struct EventData
		{
			KeyState keyStates[(uint8_t)Key::MAX]{};
			ButtonState buttonStates[(uint8_t)Button::MAX]{};
			std::string charData{};
			vec2 pointerPosition{};
			vec2 pointerDelta{};
			vec2 scrollDelta{};
		};

		void* handle{};
		std::string title{};
		IRect rect{};
		WindowStyle style{};
		WindowState state{};
		WindowEventFlags eventFlags{};
		VsyncMode vSyncMode = VsyncMode::Relaxed;
		Ref<Window> parent{};

		EventData eventData{};
		Ref<Renderer> renderer{};

		RendererEventInfo rnEventInfo{};
	};
}

#endif //HWINDOW_H
