#ifndef HLNX_EVENTHANDLING_H
#define HLNX_EVENTHANDLING_H

#include "hx11_shared.h"
#include "hex_platform.h"

namespace hf
{
	struct X11Window
	{
		::Window window;
	};

	struct X11Platform
	{
		Display* display;
		int32_t xiOpcode;

		Atom closeMessage;
		Atom wmState;
		Atom maxHorz;
		Atom maxVert;
	};

	extern X11Platform PLATFORM_DATA;
	extern std::unordered_map<::Window, Window*> WIN_REGISTRY;

	void X11_Load();
	void X11_Unload();
	ivec2 X11_GetPointerPosition(const Window* window);
    void X11_HandleNextEvent();
	void X11_HandleEvents(EngineUpdateType updateType);

	void X11_WindowSetTitle(const Window* win, const char* title);
	void X11_WindowSetSize(const Window* win, ivec2 size);
	void X11_WindowSetPosition(const Window* win, ivec2 position);
	void X11_WindowSetRect(const Window* win, IRect rect);
	void X11_WindowSetFlags(Window* win, WindowFlags flags);
	void X11_WindowFocus(const Window* win);
	void X11_WindowOpen(Window* win);
	void X11_WindowClose(Window* win);
	void* X11_WindowGetHandle(const Window* win);
}

#endif //HLNX_EVENTHANDLING_H
