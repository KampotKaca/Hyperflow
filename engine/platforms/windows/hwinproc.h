#ifndef HWINPROC_H
#define HWINPROC_H

#include "hwindows.h"
#include "components/window/hwindow.h"
#include "components/window/hkeyboard.h"
#include "components/window/hmouse.h"

namespace hf
{
	LRESULT Platform_HandleEvents_WindowTitle       (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowClose       (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowShow        (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowMove        (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowResize      (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_WindowFocus       (Window* window, WPARAM wparam, LPARAM lparam, bool focused);
	LRESULT Platform_HandleEvents_KeyboardChar      (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_KeyboardKey       (Window* window, WPARAM wparam, LPARAM lparam, Keyboard::Event::Type type);

	LRESULT Platform_HandleEvents_MouseButton       (Window* window, WPARAM wparam, LPARAM lparam, Button button, Mouse::Event::Type type);
	LRESULT Platform_HandleEvents_MouseButtonExtra  (Window* window, WPARAM wparam, LPARAM lparam, Mouse::Event::Type type);
	LRESULT Platform_HandleEvents_MouseMove         (Window* window, WPARAM wparam, LPARAM lparam);
	LRESULT Platform_HandleEvents_MouseScroll       (Window* window, WPARAM wparam, LPARAM lparam, glm::ivec2 direction);

}

#endif //HWINPROC_H
