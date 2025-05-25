#include <GLFW/glfw3.h>
#include "hx11_shared.h"
#include "hx11_platform.h"
#include "hgenericexception.h"
#include "hwindow.h"

#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>

namespace hf
{
	void X11_Load()
	{
		if(!glfwInit())
		{
			throw GENERIC_EXCEPT("[GLFW]", "Error: Failed to initialize GLFW!");
		}
		// auto display = XOpenDisplay(nullptr);
		// auto rootWindow = XRootWindow(display, 0);
	 //
		// int xi_opcode, event, error;
		// if (!XQueryExtension(display, "XInputExtension", &xi_opcode, &event, &error))
		// 	throw GENERIC_EXCEPT("[X11]", "Error: XInput extension is not supported!");
	 //
		// int major = 2;
		// int minor = 0;
		// int retval = XIQueryVersion(display, &major, &minor);
		// if (retval != Success)
		// 	throw GENERIC_EXCEPT("[X11]", "Error: XInput 2.0 is not supported (ancient X11?)");
	 //
		// unsigned char mask_bytes[(XI_LASTEVENT + 7) / 8] = {0};
  //   	XISetMask(mask_bytes, XI_RawMotion);
	 //
		// XIEventMask eventMask =
		// {
		// 	.deviceid = XIAllMasterDevices,
		// 	.mask_len = sizeof(mask_bytes),
		// 	.mask = mask_bytes,
		// };
  //   	XISelectEvents(display, rootWindow, &eventMask, 1);
	 //
		// PLATFORM_DATA.closeMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
		// PLATFORM_DATA.wmState = XInternAtom(display, "_NET_WM_STATE", False);
		// PLATFORM_DATA.maxHorz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
		// PLATFORM_DATA.maxVert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
	 //
		// PLATFORM_DATA.display = display;
		// PLATFORM_DATA.xiOpcode = xi_opcode;
	}

	void X11_Unload()
    {
		glfwTerminate();
		// XCloseDisplay(PLATFORM_DATA.display);
    }

	ivec2 X11_GetPointerPosition(const Window* window)
	{
		// auto winHandle = (X11Window*)window->handle;
		return { 0, 0 };
		// auto winHandle = (X11Window*)window->handle;
		// ::Window root, child;
		// ivec2 rootPos{}, winPos{};
		// unsigned int mask_return;
		// int retval = XQueryPointer(PLATFORM_DATA.display, winHandle->window, &root, &child,
		// 						   &rootPos.x, &rootPos.y,
		// 						   &winPos.x, &winPos.y,
		// 						   &mask_return);
		//
		// if (!retval) return { 0, 0 };
		// return rootPos;
	}

	void X11_HandleEvents(EngineUpdateType updateType)
	{
		glfwPollEvents();

		// auto display = PLATFORM_DATA.display;
		// switch (updateType)
		// {
		// case EngineUpdateType::Continues:
		//
		// 	while (XPending(display))
		// 	{
		// 		X11_HandleNextEvent();
		// 	}
		//
		// 	break;
		// case EngineUpdateType::EventRaised:
		// default:
		//
		// 	do
		// 	{
		// 		X11_HandleNextEvent();
		// 	}while (XPending(display));
		//
		// 	break;
		// }
	}
}