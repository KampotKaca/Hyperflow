#define private public
#include "hkeyboard.h"
#include "hmouse.h"
#undef private

#include "hinternal.h"
#include "hyperflow.h"
#include "hwindow.h"
#include "hplatform.h"
#include "hrenderer.h"

namespace hf
{
	namespace window
	{
		const std::string& GetTitle(const Ref<Window> &window) { return window->title; }
		ivec2 GetSize(const Ref<Window> &window) { return window->rect.size; }
		ivec2 GetPosition(const Ref<Window> &window) { return window->rect.size; }
		IRect GetRect(const Ref<Window> &window) { return window->rect; }
		WindowFlags GetFlags(const Ref<Window> &window) { return window->flags; }
		WindowStyle GetStyle(const Ref<Window> &window) { return window->style; }
		void* GetHandle(const Ref<Window> &window) { return window->handle; }
		const Renderer* GetRenderer(const Ref<Window> &window) { return window->renderer; }
		bool IsClosing(const Ref<Window> &window) { return window->handle == nullptr; }

		void SetTitle(const Ref<Window> &window, const char* title) { inter::window::SetTitle(window.get(), title); }
		void SetSize(const Ref<Window> &window, ivec2 size) { inter::window::SetSize(window.get(), size); }
		void SetPosition(const Ref<Window> &window, ivec2 position) { inter::window::SetPosition(window.get(), position); }
		void SetRect(const Ref<Window> &window, IRect rect) { inter::window::SetRect(window.get(), rect); }

		void SetFlags(const Ref<Window> &window, WindowFlags flags) { inter::window::SetFlags(window.get(), flags); }
		void Focus(const Ref<Window> &window) { inter::window::Focus(window.get()); }
		bool Close(const Ref<Window> &window) { return inter::window::Close(window.get()); }
	}

	void HandleKeyboardFocus(Keyboard* keyboard) noexcept;
	void HandleKeyboardFocusLoss(Keyboard* keyboard) noexcept;
	void HandleMouseFocus(Mouse* mouse, Window* window) noexcept;
	void HandleMouseFocusLoss(Mouse* mouse) noexcept;

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

#define EVENT_HANDLING(type, loc, evLoc, sys, t)\
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
			while(!evLoc->IsEmpty())\
			{\
				auto e = evLoc->Read();\
				if(!e.IsValid()) continue;\
				auto& currentState = loc[(uint8_t)e.t];\
				switch (currentState)\
				{\
					case KeyState::None:\
					case KeyState::Up:\
						if(e.m_Type == sys::Event::Type::Press)\
							currentState = KeyState::Down;\
						break;\
					case KeyState::Down:\
					case KeyState::DownContinues:\
						if(e.m_Type == sys::Event::Type::Release)\
							currentState = KeyState::Up;\
						break;\
				}\
			}

			EVENT_HANDLING(Key, eventData.keyStates, window->keyboard, Keyboard, m_Key)
			EVENT_HANDLING(Button, eventData.buttonStates, window->mouse, Mouse, m_Button)

			eventData.charData = "";
			while(!window->keyboard->CharIsEmpty())
			{
				eventData.charData += window->keyboard->ReadChar();
			}

			eventData.scrollDelta = window->mouse->GetScrollDelta();
			window->mouse->m_ScrollDelta = glm::vec2{ 0, 0 };

			eventData.pointerDelta = window->mouse->GetPosition() - eventData.pointerPosition;
			eventData.pointerPosition = window->mouse->GetPosition();
		}
	}

	//region Focus Control
	void HandleKeyboardFocus(Keyboard* keyboard) noexcept
	{
	}

	void HandleKeyboardFocusLoss(Keyboard* keyboard) noexcept
	{
		keyboard->DisposeChar();
		std::queue<Keyboard::Event> tempBuffer;

		while (!keyboard->IsEmpty())
		{
			auto e = keyboard->Read();
			if(e.IsValid())
			{
				if(e.GetType() == Keyboard::Event::Type::Release) tempBuffer.push(e);
				else keyboard->m_States[(uint8_t)e.GetKey()] = false;
			}
		}

		for (uint32_t i = 0; i < 256; ++i)
		{
			if(keyboard->m_States[i]) tempBuffer.emplace((Key)i, Keyboard::Event::Type::Release);
		}

		keyboard->m_States = 0;
		keyboard->m_Buffer = tempBuffer;
	}

	void HandleMouseFocus(Mouse* mouse, Window* window) noexcept
	{
		mouse->m_Position = Platform_GetPointerPosition(window);
	}

	void HandleMouseFocusLoss(Mouse* mouse) noexcept
	{
		std::queue<Mouse::Event> tempBuffer;
		while (!mouse->IsEmpty())
		{
			auto e = mouse->Read();
			if(e.IsValid())
			{
				if(e.GetType() == Mouse::Event::Type::Release) tempBuffer.push(e);
				else mouse->m_States[(uint8_t)e.GetButton()] = false;
			}
		}

		for (uint32_t i = 0; i < 256; ++i)
		{
			if(mouse->m_States[i]) tempBuffer.emplace((Button)i, Mouse::Event::Type::Release);
		}

		mouse->m_States = 0;
		mouse->m_Buffer = tempBuffer;
		mouse->m_ScrollDelta = { 0, 0 };
	};
	//endregion

	namespace inter::window
	{
		void Update(const Window* win)
		{
			win->renderer->StartFrame();
			win->renderer->EndFrame();
		}
	}
}