#ifndef HINTERNAL_H
#define HINTERNAL_H

#include "components/windowhandling/hwindow.h"

namespace hf
{
	void Time_Load();
	void Time_Update();
	
	void KeyboardEvent_Key(Keyboard& keyboard, Key key, Keyboard::Event::Type type) noexcept;
	void KeyboardEvent_Char(Keyboard& keyboard, char character) noexcept;

	void MouseEvent_Button(Mouse& mouse, Button button, Mouse::Event::Type type) noexcept;
	void MouseEvent_Moved(Mouse& mouse, glm::ivec2 position) noexcept;
	void MouseEvent_Scroll(Mouse& mouse, glm::ivec2 position) noexcept;

	void WindowEvent_Title(Window* window, const std::string& newTitle) noexcept;
	void WindowEvent_Close(Window* window) noexcept;
	void WindowEvent_Show(Window* window, bool show) noexcept;
	void WindowEvent_Move(Window* window, glm::ivec2 position) noexcept;
	void WindowEvent_Resize(Window* window, glm::ivec2 size) noexcept;
	void WindowEvent_Focus(Window* window, bool isFocused) noexcept;
}

#endif //HINTERNAL_H
