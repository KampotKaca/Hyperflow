#define private public
#include "hwindow.h"
#include "hkeyboard.h"
#include "hmouse.h"
#undef private

#include <d3d10.h>
#include <windows.h>

#include "components/hinternal.h"
#include "hplatform.h"
#include "hrenderer.h"

namespace hf
{
	const std::string& Window::GetTitle() const { return m_Title; }
	glm::ivec2 Window::GetSize() const { return m_Rect.size; }
	glm::ivec2 Window::GetPosition() const { return m_Rect.size; }
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

	void Window_SendShortcut(const InputShortcut* ins, const Window::EventData& states)
	{
		for(auto& shortcutKey : ins->keys)
		{
			auto& state = states.keyStates[(uint8_t)shortcutKey];
			if(state == KeyState::Up || state == KeyState::None) return;
		}

		for(auto& shortcutButton : ins->buttons)
		{
			auto& state = states.buttonStates[(uint8_t)shortcutButton];
			if(state == KeyState::Up || state == KeyState::None) return;
		}

		ins->Callback();
	}

	void Window_SendEvent(const Ref<Window>& window, Key key)
	{
		auto& keyCallbacks = window->m_Callbacks.m_KeyCallbacks;
		auto& state = window->m_EventData.keyStates[(uint8_t)key];
		for(auto& callback : keyCallbacks[(uint8_t)key])
			if(state == callback->state) callback->Callback();

		if(state == KeyState::Down)
		{
			auto& keyShortcuts = window->m_Callbacks.m_KeyShortcuts;
			for(auto& shortcut : keyShortcuts[(uint8_t)key])
				Window_SendShortcut(shortcut, window->m_EventData);
		}
	}
	
	void Window_SendEvent(const Ref<Window>& window, Button button)
	{
		auto& buttonCallbacks = window->m_Callbacks.m_ButtonCallbacks;
		auto& state = window->m_EventData.buttonStates[(uint8_t)button];
		for(auto& callback : buttonCallbacks[(uint8_t)button])
			if(state == callback->state) callback->Callback();

		if(state == KeyState::Down)
		{
			auto& buttonShortcuts = window->m_Callbacks.m_ButtonShortcuts;
			for(auto& shortcut : buttonShortcuts[(uint8_t)button])
				Window_SendShortcut(shortcut, window->m_EventData);
		}
	}

	void Window_SendCharEvent(const Ref<Window>& window)
	{
		for (auto& callback : window->m_Callbacks.m_CharCallbacks)
			callback(window->m_EventData.charData);
	}
	
	void Window_SendPointerEvent(const Ref<Window>& window)
	{
		for (auto& callback : window->m_Callbacks.m_PointerMoveCallbacks)
			callback(window->m_EventData.pointerDelta);
	}
	
	void Window_SendScrollEvent(const Ref<Window>& window)
	{
		for (auto& callback : window->m_Callbacks.m_ScrollCallbacks)
			callback(window->m_EventData.scrollDelta);
	}
	
	void Window_HandleInput(std::vector<Ref<Window>>& windows)
	{
		std::vector<std::pair<Ref<Window>, Key>> keyEvents;
		std::vector<std::pair<Ref<Window>, Button>> buttonEvents;
		
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
				event.emplace_back(window, (type)i);\
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
							event.emplace_back(window, e.t);\
						}\
						else LOG_WARN("Discarded Input event");\
						break;\
					case KeyState::Down:\
					case KeyState::DownContinues:\
						if(e.m_Type == sys::Event::Type::Release)\
						{\
							currentState = KeyState::Up;\
							event.emplace_back(window, e.t);\
						}\
						else LOG_WARN("Discarded Input event");\
						break;\
				}\
			}
			
			EVENT_HANDLING(Key, keyEvents, eventData.keyStates, window->m_Keyboard, Keyboard, m_Key)
			EVENT_HANDLING(Button, buttonEvents, eventData.buttonStates, window->m_Mouse, Mouse, m_Button)

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
		
		for(auto& window : windows)
		{
			auto& eventData = window->m_EventData;
			if (!eventData.charData.empty()) Window_SendCharEvent(window);
			if(eventData.pointerDelta != glm::ivec2{ 0, 0 }) Window_SendPointerEvent(window);
			if(eventData.scrollDelta != glm::vec2{ 0, 0 }) Window_SendScrollEvent(window);
		}

		for (auto& p : keyEvents) Window_SendEvent(p.first, p.second);
		for (auto& p : buttonEvents) Window_SendEvent(p.first, p.second);
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