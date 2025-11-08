#ifndef HWINDOW_H
#define HWINDOW_H

#include "hshared.h"
#include "hrenderer.h"

namespace hf
{
	struct Window
	{
		Window(const WindowCreationInfo& info);
		~Window();

		struct EventData
		{
			KeyState keyStates[(uint32_t)Key::MAX]{};
			ButtonState buttonStates[(uint32_t)Button::MAX]{};
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
		WindowPointerState pointerState = WindowPointerState::Normal;
		WindowEventFlags eventFlags{};
		VsyncMode vSyncMode = VsyncMode::Relaxed;

		EventData eventData{};
		RendererEventInfo rnEventInfo{};
	};
}

#endif //HWINDOW_H
