#include "hplatform.h"
#include "hgenericexception.h"

#include <hyperflow.h>
#include <X11/extensions/XInput2.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "hlnx_eventhandling.h"
#include "hlnx_window.h"
#include <ctime>

namespace hf
{
	void Platform_SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value)
	{
		*flags = (WindowFlags)(((int32_t)*flags & ~(int32_t)target) | (-value & (int32_t)target));
	}

	void Platform_HandleEvents(EngineUpdateType updateType)
	{
		auto display = PLATFORM_DATA.display;
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

	void Platform_Initialize()
	{
		auto display = XOpenDisplay(nullptr);
		auto rootWindow = XRootWindow(display, 0);

		int xi_opcode, event, error;
		if (!XQueryExtension(display, "XInputExtension", &xi_opcode, &event, &error))
			throw GENERIC_EXCEPT("[X11]", "Error: XInput extension is not supported!");

		int major = 2;
		int minor = 0;
		int retval = XIQueryVersion(display, &major, &minor);
		if (retval != Success)
			throw GENERIC_EXCEPT("[X11]", "Error: XInput 2.0 is not supported (ancient X11?)");

		unsigned char mask_bytes[(XI_LASTEVENT + 7) / 8] = {0};
    	XISetMask(mask_bytes, XI_RawMotion);

		XIEventMask eventMask =
		{
			.deviceid = XIAllMasterDevices,
			.mask_len = sizeof(mask_bytes),
			.mask = mask_bytes,
		};
    	XISelectEvents(display, rootWindow, &eventMask, 1);

		PLATFORM_DATA.closeMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
		PLATFORM_DATA.wmState = XInternAtom(display, "_NET_WM_STATE", False);
		PLATFORM_DATA.maxHorz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
		PLATFORM_DATA.maxVert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);

		PLATFORM_DATA.display = display;
		PLATFORM_DATA.xiOpcode = xi_opcode;
	}

	void Platform_Dispose()
    {
		XCloseDisplay(PLATFORM_DATA.display);
    }

	void Platform_BeginTemporarySystemTimer(uint16_t millisecondPrecision)
	{

	}
	void Platform_EndTemporarySystemTimer(uint16_t millisecondPrecision)
	{
	}

	void Platform_Sleep(double seconds)
	{
		timespec ts{};
		ts.tv_sec = (time_t)seconds;
		ts.tv_nsec = (long)((seconds - (double)ts.tv_sec) * 1e9);
		nanosleep(&ts, nullptr);
	}

	ivec2 Platform_GetPointerPosition(Window* window)
	{
		auto winHandle = (LnxWindowData*)window->GetHandle();
		::Window root, child;
		ivec2 rootPos{}, winPos{};
		unsigned int mask_return;
		int retval = XQueryPointer(PLATFORM_DATA.display, winHandle->windowHandle, &root, &child,
								   &rootPos.x, &rootPos.y,
								   &winPos.x, &winPos.y,
								   &mask_return);

		if (!retval) return { 0, 0 };
		return rootPos;
	}
}