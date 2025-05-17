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
		Window(const WindowCreationInfo& info, const Ref<Window>& parent);
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
		bool vSyncIsOn = false;
		Ref<Window> parent{};

		Keyboard keyboard{};
		Mouse mouse{};
		EventData eventData{};
		Ref<Renderer> renderer{};

		RenderPass (*onPassCreationCallback)(const Ref<Renderer>&);
		void (*onRenderCallback)(const Ref<Renderer>&);
		void (*onPreRenderCallback)(const Ref<Renderer>&);
	};

	void WindowEvent_Title(Window* win, const char* newTitle) noexcept;
	void WindowEvent_Close(Window* win) noexcept;
	void WindowEvent_Show(Window* win, bool show) noexcept;
	void WindowEvent_Move(Window* win, ivec2 position) noexcept;
	void WindowEvent_Resize(Window* win, ivec2 size) noexcept;
	void WindowEvent_Focus(Window* win, bool isFocused) noexcept;

	void Window_HandleInput(std::vector<Ref<Window>>& windows);
}

#endif //HWINDOW_H
