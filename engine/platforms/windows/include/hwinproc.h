#ifndef HWINPROC_H
#define HWINPROC_H

#include "hwin_shared.h"
#include "hwindow.h"

namespace hf
{
	LRESULT Platform_HandleEvents_WindowTitle       (Window* window, WPARAM wparam, LPARAM lparam) noexcept;
	LRESULT Platform_HandleEvents_WindowClose       (Window* window, WPARAM wparam, LPARAM lparam) noexcept;
	LRESULT Platform_HandleEvents_WindowShow        (Window* window, WPARAM wparam, LPARAM lparam) noexcept;
	LRESULT Platform_HandleEvents_WindowMove        (Window* window, WPARAM wparam, LPARAM lparam) noexcept;
	LRESULT Platform_HandleEvents_WindowResize      (Window* window, WPARAM wparam, LPARAM lparam) noexcept;
	LRESULT Platform_HandleEvents_WindowFocus       (Window* window, WPARAM wparam, LPARAM lparam, bool focused) noexcept;
	LRESULT Platform_HandleEvents_KeyboardChar      (Window* window, WPARAM wparam, LPARAM lparam) noexcept;
	LRESULT Platform_HandleEvents_KeyboardKey       (Window* window, WPARAM wparam, LPARAM lparam, Keyboard::Event::Type type) noexcept;

	LRESULT Platform_HandleEvents_MouseButton       (Window* window, WPARAM wparam, LPARAM lparam, Button button, Mouse::Event::Type type) noexcept;
	LRESULT Platform_HandleEvents_MouseButtonExtra  (Window* window, WPARAM wparam, LPARAM lparam, Mouse::Event::Type type) noexcept;
	LRESULT Platform_HandleEvents_MouseMove         (Window* window, WPARAM wparam, LPARAM lparam) noexcept;
	LRESULT Platform_HandleEvents_MouseScroll       (Window* window, WPARAM wparam, LPARAM lparam, ivec2 direction) noexcept;

}

#endif //HWINPROC_H
