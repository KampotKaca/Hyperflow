#define private public
#include "components/windowhandling/hwindow.h"
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

	void HandleKeyboardFocusLoss(Keyboard& keyboard) noexcept;
	void HandleMouseFocusLoss(Mouse& mouse) noexcept;

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
		if(!isFocused)
		{
			HandleKeyboardFocusLoss(window->keyboard);
			HandleMouseFocusLoss(window->mouse);
		}
	}

	void HandleKeyboardFocusLoss(Keyboard& keyboard) noexcept
	{
		keyboard.DisposeChar();
		std::queue<Keyboard::Event> tempBuffer;

		while (!keyboard.IsEmpty())
		{
			auto e = keyboard.Read();
			if(e.IsValid())
			{
				if(e.GetType() == Keyboard::Event::Type::Release) tempBuffer.push(e);
				else keyboard.m_States[(uint8_t)e.GetKey()] = false;
			}
		}

		for (uint32_t i = 0; i < 256; ++i)
		{
			if(keyboard.m_States[i]) tempBuffer.emplace((Key)i, Keyboard::Event::Type::Release);
		}

		keyboard.m_States = 0;
		keyboard.m_Buffer = tempBuffer;
	}

	void HandleMouseFocusLoss(Mouse &mouse) noexcept
	{
		std::queue<Mouse::Event> tempBuffer;
		while (!mouse.IsEmpty())
		{
			auto e = mouse.Read();
			if(e.IsValid())
			{
				if(e.GetType() == Mouse::Event::Type::Release) tempBuffer.push(e);
				else mouse.m_States[(uint8_t)e.GetButton()] = false;
			}
		}

		for (uint32_t i = 0; i < 256; ++i)
		{
			if(mouse.m_States[i]) tempBuffer.emplace((Button)i, Mouse::Event::Type::Release);
		}

		mouse.m_States = 0;
		mouse.m_Buffer = tempBuffer;
	}
}