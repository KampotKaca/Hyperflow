#ifndef HPLATFORM_H
#define HPLATFORM_H

#include "hshared.h"
#include "hwindow.h"
#include "hkeyboard.h"
#include "hmouse.h"

namespace hf
{
	void Platform_SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value);
	void Platform_HandleEvents(EngineUpdateType updateType);
	void Platform_Initialize();
	void Platform_Dispose();
	void Platform_BeginTemporarySystemTimer(uint16_t millisecondPrecision);
	void Platform_EndTemporarySystemTimer(uint16_t millisecondPrecision);
	void Platform_Sleep(double seconds);

	ivec2 Platform_GetPointerPosition(const Window* window);

   	void KeyboardEvent_Key(Keyboard* keyboard, Key key, Keyboard::Event::Type type) noexcept;
	void KeyboardEvent_Char(Keyboard* keyboard, char character) noexcept;

	void MouseEvent_Button(Mouse* mouse, Button button, Mouse::Event::Type type) noexcept;
	void MouseEvent_Moved(Mouse* mouse, ivec2 position) noexcept;
	void MouseEvent_Scroll(Mouse* mouse, vec2 delta) noexcept;

	void WindowEvent_Title(Window* win, const char* newTitle) noexcept;
	void WindowEvent_Close(Window* win) noexcept;
	void WindowEvent_Show(Window* win, bool show) noexcept;
	void WindowEvent_Move(Window* win, ivec2 position) noexcept;
	void WindowEvent_Resize(Window* win, ivec2 size) noexcept;
	void WindowEvent_Focus(Window* win, bool isFocused) noexcept;

	void Window_HandleInput(std::vector<Ref<Window>>& windows);
}

#endif //HPLATFORM_H
