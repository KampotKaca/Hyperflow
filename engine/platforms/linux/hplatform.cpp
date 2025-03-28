#include "hplatform.h"

#include <hyperflow.h>
#include <X11/extensions/XInput2.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "hlnx_eventhandling.h"
#include "hlnx_window.h"
#include "hunsupportedexception.h"

namespace hf
{
	void Platform_SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value)
	{
		*flags = (WindowFlags)(((int32_t)*flags & ~(int32_t)target) | (-value & (int32_t)target));
	}

	void Platform_HandleEvents(EngineUpdateType updateType)
	{
		auto handle = (LnxPlatformHandle*)Hyperflow::GetPlatformHandle();
		auto display = handle->display;
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
		auto display = XOpenDisplay(nullptr);
		auto rootWindow = XRootWindow(display, 0);

		int xi_opcode, event, error;
		if (!XQueryExtension(display, "XInputExtension", &xi_opcode, &event, &error))
			throw UNSUPPORTED_EXCEPT("Error: XInput extension is not supported!");

		int major = 2;
		int minor = 0;
		int retval = XIQueryVersion(display, &major, &minor);
		if (retval != Success)
			throw UNSUPPORTED_EXCEPT("Error: XInput 2.0 is not supported (ancient X11?)");

		unsigned char mask_bytes[(XI_LASTEVENT + 7) / 8] = {0};
    	XISetMask(mask_bytes, XI_RawMotion);

		XIEventMask eventMask =
		{
			.deviceid = XIAllMasterDevices,
			.mask_len = sizeof(mask_bytes),
			.mask = mask_bytes,
		};
    	XISelectEvents(display, rootWindow, &eventMask, 1);

		auto handle = new LnxPlatformHandle();
		handle->display = display;
		handle->xiOpcode = xi_opcode;
		return handle;
	}

	void Platform_Dispose(void* platformHandle)
    {
		auto handle = (LnxPlatformHandle*)Hyperflow::GetPlatformHandle();
		XCloseDisplay(handle->display);
		delete handle;
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

	ivec2 Platform_GetPointerPosition(Window* window)
	{
		auto handle = (LnxPlatformHandle*)Hyperflow::GetPlatformHandle();
		auto winHandle = (LnxWindowData*)window->GetHandle();
		::Window root, child;
		ivec2 rootPos{}, winPos{};
		unsigned int mask_return;
		int retval = XQueryPointer(handle->display, winHandle->windowHandle, &root, &child,
								   &rootPos.x, &rootPos.y,
								   &winPos.x, &winPos.y,
								   &mask_return);

		if (!retval) return { 0, 0 };
		return rootPos;
	}
}