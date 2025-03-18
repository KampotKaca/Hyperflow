#include "hplatform.h"

#include <hyperflow.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "hlnx_eventhandling.h"

namespace hf
{
	void Platform_SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value)
	{
		*flags = (WindowFlags)(((int32_t)*flags & ~(int32_t)target) | (-value & (int32_t)target));
	}

	void Platform_HandleEvents(EngineUpdateType updateType)
	{
		XEvent event;
		auto display = (Display*)Hyperflow::GetPlatformHandle();
		switch (updateType)
		{
			case EngineUpdateType::Continues:

				while (XPending(display))
				{
					Platform_HandleNextEvent();
				}

			break;
			case EngineUpdateType::EventRaised:
			default:

				do
				{
					Platform_HandleNextEvent();
				}while (XPending(display));

			break;
		}
	}

	void* Platform_Initialize()
	{
		Display* display = XOpenDisplay(nullptr);
		return display;
	}

	void Platform_Dispose(void* platformHandle)
    {
		XCloseDisplay((Display*)platformHandle);
    }

	void Platform_BeginTemporarySystemTimer(uint16_t millisecondPrecision)
	{
	}
	void Platform_EndTemporarySystemTimer(uint16_t millisecondPrecision)
	{
	}

	void Platform_Sleep(double seconds)
	{

	}

	glm::ivec2 Platform_GetPointerPosition(Window* window)
	{
		return { 0, 0 };
	}
}