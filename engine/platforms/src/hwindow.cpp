#define private public
#include "hwindow.h"
#include "hkeyboard.h"
#include "hmouse.h"
#undef private

#include "hplatform.h"
#include "hrenderer.h"

namespace hf
{
	const std::string& Window::GetTitle() const { return m_Title; }
	ivec2 Window::GetSize() const { return m_Rect.size; }
	ivec2 Window::GetPosition() const { return m_Rect.size; }
	IRect Window::GetRect() const { return m_Rect; }
	WindowFlags Window::GetFlags() const { return m_Flags; }
	WindowStyle Window::GetStyle() const { return m_Style; }
	void *Window::GetHandle() const { return m_Handle; }
	Ref<Renderer> Window::GetRenderer() const { return m_Renderer; }
	bool Window::IsClosing() const { return m_Handle == nullptr; }
	void Window::Update()
	{
		m_Renderer->StartFrame();
		m_Renderer->EndFrame();
	}

	void HandleKeyboardFocus(const Ref<Keyboard>& keyboard) noexcept;
	void HandleKeyboardFocusLoss(const Ref<Keyboard>& keyboard) noexcept;
	void HandleMouseFocus(const Ref<Mouse>& mouse, Window* window) noexcept;
	void HandleMouseFocusLoss(const Ref<Mouse>& mouse) noexcept;

	void WindowEvent_Title(Window* window, const char* newTitle) noexcept
	{
		window->m_Title = std::string(newTitle);
	}

	void WindowEvent_Close(Window* window) noexcept
	{
		window->Close();
	}

	void WindowEvent_Show(Window* window, bool show) noexcept
	{
		Platform_SetWindowFlag(&window->m_Flags, WindowFlags::Visible, show);
	}

	void WindowEvent_Move(Window* window, ivec2 position) noexcept
	{
		window->m_Rect.position = position;
	}

	void WindowEvent_Resize(Window* window, ivec2 size) noexcept
	{
		window->m_Rect.size = size;
	}

	void WindowEvent_Focus(Window* window, bool isFocused) noexcept
	{
		if(isFocused)
		{
			HandleKeyboardFocus(window->m_Keyboard);
			HandleMouseFocus(window->m_Mouse, window);
		}
		else
		{
			HandleKeyboardFocusLoss(window->m_Keyboard);
			HandleMouseFocusLoss(window->m_Mouse);
		}
	}
	
	void Window_HandleInput(std::vector<Ref<Window>>& windows)
	{
		for(const auto& window : windows)
		{
			auto& eventData = window->m_EventData;

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

			EVENT_HANDLING(Key, eventData.keyStates, window->m_Keyboard, Keyboard, m_Key)
			EVENT_HANDLING(Button, eventData.buttonStates, window->m_Mouse, Mouse, m_Button)

			eventData.charData = "";
			while(!window->m_Keyboard->CharIsEmpty())
			{
				eventData.charData += window->m_Keyboard->ReadChar();
			}

			eventData.scrollDelta = window->m_Mouse->GetScrollDelta();
			window->m_Mouse->m_ScrollDelta = glm::vec2{ 0, 0 };

			eventData.pointerDelta = window->m_Mouse->GetPosition() - eventData.pointerPosition;
			eventData.pointerPosition = window->m_Mouse->GetPosition();
		}
	}

	//region Focus Control
	void HandleKeyboardFocus(const Ref<Keyboard>& keyboard) noexcept
	{
	}

	void HandleKeyboardFocusLoss(const Ref<Keyboard>& keyboard) noexcept
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

	void HandleMouseFocus(const Ref<Mouse>& mouse, Window* window) noexcept
	{
		mouse->m_Position = Platform_GetPointerPosition(window);
	}

	void HandleMouseFocusLoss(const Ref<Mouse>& mouse) noexcept
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
}