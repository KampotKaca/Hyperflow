#include "hinternal.h"
#include "hyperflow.h"
#include "hwindow.h"
#include "hplatform.h"
#include "hrenderer.h"

namespace hf
{
	Window::Window(const WindowData& data, const Ref<Window>& parent)
	{
		title = data.title;
		style = data.style;
		this->parent = parent;
		flags = (WindowFlags)0;
		rect =
		{
			.position = data.position,
			.size = data.size
		};
		renderer = nullptr;
		onRenderCallback = data.onRenderCallback;

		inter::window::Open(this);

		auto pPos = Platform_GetPointerPosition(this);
		mouse.position = pPos;
		mouse.isInClientRegion = pPos.x >= 0 && pPos.x < rect.size.x && pPos.y > 0 && pPos.y < rect.size.y;
		eventData.pointerPosition = mouse.position;

		inter::window::SetFlags(this, data.flags);
		inter::window::Focus(this);
		inter::window::SetTitle(this, title.c_str());
	}

	Window::~Window()
	{
		inter::window::Close(this);
	}

	namespace window
	{
		Ref<Window> Open(const WindowData &data, const Ref<Window> &parent)
		{
			auto newWindow = MakeRef<Window>(data, parent);
			newWindow->renderer = MakeRef<Renderer>(newWindow);
			inter::HF.windows.push_back(newWindow);
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
		void* GetHandle(const Ref<Window> &window) { return window->handle; }
		bool IsClosing(const Ref<Window> &window) { return window->handle == nullptr; }

		void SetTitle(const Ref<Window> &window, const char* title) { inter::window::SetTitle(window.get(), title); }
		void SetSize(const Ref<Window> &window, const ivec2 size) { inter::window::SetSize(window.get(), size); }
		void SetPosition(const Ref<Window> &window, const ivec2 position) { inter::window::SetPosition(window.get(), position); }
		void SetRect(const Ref<Window> &window, const IRect rect) { inter::window::SetRect(window.get(), rect); }

		void SetFlags(const Ref<Window> &window, const WindowFlags flags) { inter::window::SetFlags(window.get(), flags); }
		void Focus(const Ref<Window> &window) { inter::window::Focus(window.get()); }
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
		Platform_SetWindowFlag(&win->flags, WindowFlags::Visible, show);
	}

	void WindowEvent_Move(Window* window, ivec2 position) noexcept
	{
		window->rect.position = position;
	}

	void WindowEvent_Resize(Window* window, ivec2 size) noexcept
	{
		window->rect.size = size;
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
		mouse.position = Platform_GetPointerPosition(window);
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

	namespace inter::window
	{
		void Update(const Window* win)
		{
			auto rn = win->renderer.get();
			rendering::StartFrame(rn);
			if (win->onRenderCallback) win->onRenderCallback();
			rendering::EndFrame(rn);
		}
	}
}