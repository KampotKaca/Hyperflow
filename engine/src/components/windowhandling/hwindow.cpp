#define private public
#include "components/windowhandling/hwindow.h"
#include "components/windowhandling/hkeyboard.h"
#include "components/windowhandling/hmouse.h"
#undef private

#include "components/hinternal.h"
#include "components/hplatform.h"

namespace hf
{
	std::string Window::GetTitle() const { return m_Title; }
	glm::ivec2 Window::GetSize() const { return m_Rect.size; }
	glm::ivec2 Window::GetPosition() const { return m_Rect.size; }
	IRect Window::GetRect() const { return m_Rect; }
	WindowFlags Window::GetFlags() const { return m_Flags; }
	WindowStyle Window::GetStyle() const { return m_Style; }

	void HandleKeyboardFocus(const Ref<Keyboard>& keyboard) noexcept;
	void HandleKeyboardFocusLoss(const Ref<Keyboard>& keyboard) noexcept;
	void HandleMouseFocus(const Ref<Mouse>& mouse, Window* window) noexcept;
	void HandleMouseFocusLoss(const Ref<Mouse>& mouse) noexcept;

	void WindowEvent_Title(Window* window, const std::string& newTitle) noexcept
	{
		window->m_Title = newTitle;
	}

	void WindowEvent_Close(Window* window) noexcept
	{
		window->m_ShouldClose = true;
	}

	void WindowEvent_Show(Window* window, bool show) noexcept
	{
		Platform_SetWindowFlag(&window->m_Flags, WindowFlags::Visible, show);
	}

	void WindowEvent_Move(Window* window, glm::ivec2 position) noexcept
	{
		window->m_Rect.position = position;
	}

	void WindowEvent_Resize(Window* window, glm::ivec2 size) noexcept
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

	void Window_SendKeyEvent(const Ref<Window>& window, Key key)
	{
		
	}
	
	void Window_SendButtonEvent(const Ref<Window>& window, Button button)
	{
		
	}

	void Window_SendCharEvent(const Ref<Window>& window)
	{

	}
	
	void Window_HandleInput(std::vector<Ref<Window>>& windows)
	{
		for(auto& window : windows)
		{
			auto& eventData = window->m_EventData;

#define EVENT_HANDLING(type, event, loc, evLoc, sys, t)\
			for (uint8_t i = 0; i < (uint8_t)type::Count; ++i)\
			{\
				auto& currentState = loc[i];\
				switch (currentState)\
				{\
					case KeyState::None: continue;\
					case KeyState::Up: currentState = KeyState::None; continue;\
					case KeyState::Down: currentState = KeyState::DownContinues; break;\
					case KeyState::DownContinues: break;\
				}\
				event(window, (type)i);\
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
						{\
							currentState = KeyState::Down;\
							event(window, e.t);\
						}\
						else LOG_WARN("Discarded Input event");\
						break;\
					case KeyState::Down:\
					case KeyState::DownContinues:\
						if(e.m_Type == sys::Event::Type::Release)\
						{\
							currentState = KeyState::Up;\
							event(window, e.t);\
						}\
						else LOG_WARN("Discarded Input event");\
						break;\
				}\
			}

			EVENT_HANDLING(Key, Window_SendKeyEvent, eventData.keyStates, window->m_Keyboard, Keyboard, m_Key)
			EVENT_HANDLING(Button, Window_SendButtonEvent, eventData.buttonStates, window->m_Mouse, Mouse, m_Button)

			eventData.charData = "";
			while(!window->m_Keyboard->CharIsEmpty())
			{
				eventData.charData += window->m_Keyboard->ReadChar();
			}

			if(!eventData.charData.empty()) Window_SendCharEvent(window);
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
		mouse->m_Scroll = { 0, 0 };
	};
	//endregion
}