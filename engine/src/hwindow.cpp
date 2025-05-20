#include "hinternal.h"
#include "hyperflow.h"
#include "hwindow.h"
#include "hrenderer.h"
#include "../rendering/include/hex_renderer.h"

namespace hf
{
	Window::Window(const WindowCreationInfo& info, const Ref<Window>& parent)
		: title(info.title), style(info.style), vSyncMode(info.vsyncMode), parent(parent),
		  rnEventInfo(info.rnEventInfo)
	{
		flags = (WindowFlags)0;
		rect =
		{
			.position = info.position,
			.size = info.size
		};
		renderer = nullptr;

		inter::window::Open(this);

		auto pPos = inter::platform::GetPointerPosition(this);
		mouse.position = pPos;
		mouse.isInClientRegion = pPos.x >= 0 && pPos.x < rect.size.x && pPos.y > 0 && pPos.y < rect.size.y;
		eventData.pointerPosition = mouse.position;

		inter::window::SetFlags(this, info.flags);
		inter::window::Focus(this);
		inter::window::SetTitle(this, title);
	}

	Window::~Window()
	{
		inter::window::Close(this);
	}

	namespace window
	{
		Ref<Window> Open(const WindowCreationInfo &data, const Ref<Window> &parent)
		{
			auto newWindow = MakeRef<Window>(data, parent);
			inter::HF.windows.push_back(newWindow);

			if (inter::HF.renderingApi.type != RenderingApiType::None)
			{
				newWindow->renderer = MakeRef<Renderer>(newWindow.get(), newWindow->rnEventInfo);
				auto rn = newWindow->renderer;
				inter::HF.renderingApi.api.PostInstanceLoad(newWindow->renderer->handle,
					rn->eventInfo.onPassCreationCallback(rn));
			}
			return newWindow;
		}

		bool Close(const Ref<Window> &window)
		{
			std::erase_if(inter::HF.windows, [&](Ref<Window> &w)
			{
				return w.get() == window.get();
			});
			return inter::window::Close(window.get());
		}

		void CloseAll()
		{
			for (auto& window : inter::HF.windows)
				inter::window::Close(window.get());
			inter::HF.windows.clear();
		}

		const std::string& GetTitle(const Ref<Window> &window) { return window->title; }
		ivec2 GetSize(const Ref<Window> &window) { return window->rect.size; }
		ivec2 GetPosition(const Ref<Window> &window) { return window->rect.size; }
		IRect GetRect(const Ref<Window> &window) { return window->rect; }
		WindowFlags GetFlags(const Ref<Window> &window) { return window->flags; }
		WindowStyle GetStyle(const Ref<Window> &window) { return window->style; }
		void* GetHandle(const Ref<Window> &window) { return inter::window::GetWindowHandle(window.get()); }
		Ref<Renderer> GetRenderer(const Ref<Window> &window) { return window->renderer; }

		bool IsClosing(const Ref<Window> &window) { return window->handle == nullptr; }
		Ref<Renderer> GetRenderer(const Ref<Window> &window);

		void SetTitle(const Ref<Window> &window, const std::string& title) { if (!IsClosing(window)) inter::window::SetTitle(window.get(), title); }
		void SetSize(const Ref<Window> &window, const ivec2 size) { if (!IsClosing(window)) inter::window::SetSize(window.get(), size); }
		void SetPosition(const Ref<Window> &window, const ivec2 position) { if (!IsClosing(window)) inter::window::SetPosition(window.get(), position); }
		void SetRect(const Ref<Window> &window, const IRect rect) { if (!IsClosing(window)) inter::window::SetRect(window.get(), rect); }

		void SetFlags(const Ref<Window> &window, const WindowFlags flags) { if (!IsClosing(window)) inter::window::SetFlags(window.get(), flags); }
		void Focus(const Ref<Window> &window) { if (!IsClosing(window)) inter::window::Focus(window.get()); }

		void SetVSyncMode(const Ref<Window> &window, VsyncMode mode)
		{
			window->vSyncMode = mode;
			inter::HF.renderingApi.api.SetVSync(window->renderer->handle, mode);
		}

		VsyncMode GetVSyncMode(const Ref<Window> &window) { return window->vSyncMode; }
	}

	void HandleKeyboardFocus(Keyboard& keyboard) noexcept;
	void HandleKeyboardFocusLoss(Keyboard& keyboard) noexcept;
	void HandleMouseFocus(Mouse& mouse, Window* window) noexcept;
	void HandleMouseFocusLoss(Mouse& mouse) noexcept;

	void WindowEvent_Title(Window* win, const char* newTitle) noexcept
	{
		win->title = std::string(newTitle);
	}

	void WindowEvent_Close(Window* win) noexcept
	{
		inter::window::Close(win);
	}

	void WindowEvent_Show(Window* win, bool show) noexcept
	{
		inter::platform::SetWindowFlag(&win->flags, WindowFlags::Visible, show);
	}

	void WindowEvent_Move(Window* window, ivec2 position) noexcept
	{
		window->rect.position = position;
	}

	void WindowEvent_Resize(Window* window, ivec2 size) noexcept
	{
		window->rect.size = size;
		if (window->renderer)
		{
			window->renderer->size = size;
			inter::HF.renderingApi.api.RegisterFrameBufferChange(window->renderer->handle, size);
		}
	}

	void WindowEvent_Focus(Window* window, bool isFocused) noexcept
	{
		if(isFocused)
		{
			HandleKeyboardFocus(window->keyboard);
			HandleMouseFocus(window->mouse, window);
		}
		else
		{
			HandleKeyboardFocusLoss(window->keyboard);
			HandleMouseFocusLoss(window->mouse);
		}
	}
	
	void Window_HandleInput(std::vector<Ref<Window>>& windows)
	{
		for(const auto& window : windows)
		{
			auto& eventData = window->eventData;

#define EVENT_HANDLING(loc, evLoc, sys, t)\
			for (auto& currentState : loc)\
			{\
				switch (currentState)\
				{\
					case KeyState::None: continue;\
					case KeyState::Up: currentState = KeyState::None; continue;\
					case KeyState::Down: currentState = KeyState::DownContinues; break;\
					case KeyState::DownContinues: break;\
				}\
			}\
			while(!evLoc.buffer.empty())\
			{\
				auto e = evLoc.buffer.front();\
				evLoc.buffer.pop();\
				if(!e.IsValid()) continue;\
				auto& currentState = loc[(uint8_t)e.t];\
				switch (currentState)\
				{\
					case KeyState::None:\
					case KeyState::Up:\
						if(e.type == sys::Event::Type::Press)\
							currentState = KeyState::Down;\
						break;\
					case KeyState::Down:\
					case KeyState::DownContinues:\
						if(e.type == sys::Event::Type::Release)\
							currentState = KeyState::Up;\
						break;\
				}\
			}

			EVENT_HANDLING(eventData.keyStates, window->keyboard, Keyboard, key)
			EVENT_HANDLING(eventData.buttonStates, window->mouse, Mouse, button)

			eventData.charData = "";
			while(!window->keyboard.charBuffer.empty())
			{
				auto newChar = window->keyboard.charBuffer.front();
				window->keyboard.charBuffer.pop();
				eventData.charData += newChar;
			}

			eventData.scrollDelta = window->mouse.scrollDelta;
			window->mouse.scrollDelta = glm::vec2{ 0, 0 };

			eventData.pointerDelta = window->mouse.position - eventData.pointerPosition;
			eventData.pointerPosition = window->mouse.position;
		}
	}

	//region Focus Control
	void HandleKeyboardFocus(Keyboard& keyboard) noexcept
	{
	}

	void HandleKeyboardFocusLoss(Keyboard& keyboard) noexcept
	{
		keyboard.charBuffer = std::queue<char>();
		std::queue<Keyboard::Event> tempBuffer;

		while (!keyboard.buffer.empty())
		{
			auto e = keyboard.buffer.front();
			keyboard.buffer.pop();
			if(e.IsValid())
			{
				if(e.type == Keyboard::Event::Type::Release) tempBuffer.push(e);
				else keyboard.states[(uint8_t)e.key] = false;
			}
		}

		for (uint32_t i = 0; i < 256; ++i)
		{
			if(keyboard.states[i]) tempBuffer.emplace((Key)i, Keyboard::Event::Type::Release);
		}

		keyboard.states = 0;
		keyboard.buffer = tempBuffer;
	}

	void HandleMouseFocus(Mouse& mouse, Window* window) noexcept
	{
		mouse.position = inter::platform::GetPointerPosition(window);
	}

	void HandleMouseFocusLoss(Mouse& mouse) noexcept
	{
		std::queue<Mouse::Event> tempBuffer;
		while (!mouse.buffer.empty())
		{
			auto e = mouse.buffer.front();
			mouse.buffer.pop();
			if(e.IsValid())
			{
				if(e.type == Mouse::Event::Type::Release) tempBuffer.push(e);
				else mouse.states[(uint8_t)e.button] = false;
			}
		}

		for (uint32_t i = 0; i < 256; ++i)
		{
			if(mouse.states[i]) tempBuffer.emplace((Button)i, Mouse::Event::Type::Release);
		}

		mouse.states = 0;
		mouse.buffer = tempBuffer;
		mouse.scrollDelta = { 0, 0 };
	};
	//endregion
}