#ifndef HINTERNAL_H
#define HINTERNAL_H

#include "hshared.h"
#include "components/window/hwindow.h"
#include "components/window/hkeyboard.h"
#include "components/window/hmouse.h"

namespace hf
{
	void Time_Load();
	void Time_Update();
	
	void KeyboardEvent_Key(const Ref<Keyboard>& keyboard, Key key, Keyboard::Event::Type type) noexcept;
	void KeyboardEvent_Char(const Ref<Keyboard>& keyboard, char character) noexcept;

	void MouseEvent_Button(const Ref<Mouse>& mouse, Button button, Mouse::Event::Type type) noexcept;
	void MouseEvent_Moved(const Ref<Mouse>& mouse, glm::ivec2 position) noexcept;
	void MouseEvent_Scroll(const Ref<Mouse>& mouse, glm::vec2 delta) noexcept;

	void WindowEvent_Title(Window* window, const char* newTitle) noexcept;
	void WindowEvent_Close(Window* window) noexcept;
	void WindowEvent_Show(Window* window, bool show) noexcept;
	void WindowEvent_Move(Window* window, glm::ivec2 position) noexcept;
	void WindowEvent_Resize(Window* window, glm::ivec2 size) noexcept;
	void WindowEvent_Focus(Window* window, bool isFocused) noexcept;

	void Window_HandleInput(std::vector<Ref<Window>>& windows);
}

#endif //HINTERNAL_H
