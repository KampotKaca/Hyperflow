#include "GLFW/glfw3.h"

#include "hx11_shared.h"
#include "hx11_platform.h"
#include "hx11_exception.h"
#include "hwindow.h"

#include <X11/extensions/XInput2.h>

#include "hgenericexception.h"

namespace hf
{
	X11Platform PLATFORM_DATA{};

	void XIconify(const ::Window win, const int32_t id)
	{
		// auto it = WIN_REGISTRY.find(win);
		// if (it == WIN_REGISTRY.end()) return;
		// XEvent event = {0};
		// event.xclient.type = ClientMessage;
		// event.xclient.message_type = PLATFORM_DATA.wmState;
		// event.xclient.display = PLATFORM_DATA.display;
		// event.xclient.window = win;
		// event.xclient.format = 32;
		// event.xclient.data.l[0] = id;
		// event.xclient.data.l[1] = (long)PLATFORM_DATA.maxHorz;
		// event.xclient.data.l[2] = (long)PLATFORM_DATA.maxVert;
		// XSendEvent(PLATFORM_DATA.display, DefaultRootWindow(PLATFORM_DATA.display), False,
		// 	SubstructureNotifyMask | SubstructureRedirectMask, &event);
	}

	void X11_WindowSetTitle(const Window* win, const char* title)
	{
		// if (!win->handle) return;
		// XStoreName(PLATFORM_DATA.display, ((X11Window*)win->handle)->window, title);
		// XFlush(PLATFORM_DATA.display);
	}
	void X11_WindowSetSize(const Window* win, ivec2 size)
	{
		// if (!win->handle) return;
		// XResizeWindow(PLATFORM_DATA.display, ((X11Window*)win->handle)->window, size.x, size.y);
		// XFlush(PLATFORM_DATA.display);
	}
	void X11_WindowSetPosition(const Window* win, ivec2 position)
	{
		// if (!win->handle) return;
		// XMoveWindow(PLATFORM_DATA.display, ((X11Window*)win->handle)->window, position.x, position.y);
		// XFlush(PLATFORM_DATA.display);
	}
	void X11_WindowSetRect(const Window* win, IRect rect)
	{
		// X11_WindowSetPosition(win, rect.position);
		// X11_WindowSetSize(win, rect.size);
	}

	void X11_WindowSetFlags(Window* win, WindowFlags flags)
	{
		// if(!win->handle || win->flags == flags) return;
		//
		// auto& display = PLATFORM_DATA.display;
		// auto window = ((X11Window*)win->handle)->window;
		// if(((int32_t)win->flags & (int32_t)WindowFlags::Visible) != ((int32_t)flags & (int32_t)WindowFlags::Visible))
		// {
		// 	if((int32_t)flags & (int32_t)WindowFlags::Visible) XMapWindow(display, window);
		// 	else XUnmapWindow(display, window);
		// }
		//
		// if(((int32_t)win->flags & (int32_t)WindowFlags::Minimized) != ((int32_t)flags & (int32_t)WindowFlags::Minimized) ||
		//    ((int32_t)win->flags & (int32_t)WindowFlags::Maximized) != ((int32_t)flags & (int32_t)WindowFlags::Maximized))
		// {
		// 	uint32_t both = (int32_t)flags & (int32_t)WindowFlags::Minimized + (int32_t)flags & (int32_t)WindowFlags::Maximized;
		// 	if((int32_t)flags & (int32_t)WindowFlags::Minimized) XIconifyWindow(display, window, DefaultScreen(display));
		// 	if((int32_t)flags & (int32_t)WindowFlags::Maximized) XIconify(window, 1);
		// 	if(both == 0) XIconify(window, 0);
		// }
		//
		// XFlush(display);
		// win->flags = flags;
	}

	void X11_WindowFocus(const Window* win)
	{
		// if (!win->handle) return;
		// auto& display = PLATFORM_DATA.display;
		// XEvent event = {};
		// event.xclient.type = ClientMessage;
		// event.xclient.message_type = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
		// event.xclient.display = display;
		// event.xclient.window = ((X11Window*)win->handle)->window;
		// event.xclient.format = 32;
		// event.xclient.data.l[0] = 1;  // 1 = normal request
		// event.xclient.data.l[1] = CurrentTime;
		//
		// XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
		// XFlush(display);
	}

	void X11_WindowOpen(Window* win)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
		glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

		auto newWindow = glfwCreateWindow(win->rect.size.x, win->rect.size.y, win->title.c_str(), nullptr, nullptr);
		if (!newWindow) throw GENERIC_EXCEPT("[Hyperflow]", "Failed to create window!");

		glfwFocusWindow(newWindow);
		win->handle = newWindow;

		// auto display = PLATFORM_DATA.display;
		// int screen = DefaultScreen(display);
		//
		// auto attributes = (XSetWindowAttributes)
		// {
		// 	.background_pixel = WhitePixel(display, screen),
		// 	.event_mask = StructureNotifyMask | ExposureMask | FocusChangeMask | KeyPressMask |
		//   				  KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
		//   				  PointerMotionMask | EnterWindowMask | LeaveWindowMask,
		// };
		//
		// auto root = RootWindow(display, screen);
		// if (win->parent) root = ((X11Window*)win->parent->handle)->window;
		// auto lnxData = new X11Window();
		// win->handle = lnxData;
		//
		// lnxData->window = XCreateWindow(display, root,
		// win->rect.position.x, win->rect.position.y,
		// win->rect.size.x, win->rect.size.y, 1,
		// DefaultDepth(display, screen), InputOutput,
		// DefaultVisual(display, screen),
		// CWBackPixel | CWEventMask, &attributes);
		//
		// if(win->parent) XSetTransientForHint(display, lnxData->window, root);
		//
		// WIN_REGISTRY[lnxData->window] = win;
		//
		// XSetWMProtocols(display, lnxData->window, &PLATFORM_DATA.closeMessage, 1);
		//
		// XSetErrorHandler(XErrorHandler);
		// XSetIOErrorHandler(XIOErrorHandler);
	}

	void X11_WindowClose(Window* win)
	{
		auto window = (GLFWwindow*)win->handle;
		glfwDestroyWindow(window);
		win->handle = nullptr;
		win->parent = nullptr;
		// auto window = ((X11Window*)win->handle)->window;
		// WIN_REGISTRY.erase(window);
		// XDestroyWindow(PLATFORM_DATA.display, window);
		// delete((X11Window*)win->handle);
		// win->handle = nullptr;
		// win->parent = nullptr;
	}
}
