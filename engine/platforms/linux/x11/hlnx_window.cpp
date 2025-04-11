#include <hyperflow.h>
#include "hwindow.h"
#include "hinternal.h"
#include "hplatform.h"
#include "hlnx_eventhandling.h"
#include "exceptions/hwindowexception.h"
#include "hlnx_window.h"

namespace hf
{
	LnxPlatformData PLATFORM_DATA{};
	std::unordered_map<::Window, Window*> WIN_REGISTRY{};

	int32_t XErrorHandler(Display *display, XErrorEvent *error)
	{
		char error_text[256];
		WindowException::TranslateErrorCode(error->error_code, error_text);
		LOG_ERROR("[Error] %s", error_text);
		return 0;
	}

	int32_t XIOErrorHandler(Display *display)
	{
		throw WND_EXCEPT(EXIT_FAILURE);
	}

	Window::Window(const WindowData& data, const Ref<Window>& parent)
	{
		title = data.title;
		style = data.style;
		this->parent = parent;
		flags = (WindowFlags)0;
		rect =
		{
			.position = data.position,
			.size = data.size
		};
		renderer = nullptr;

		auto display = PLATFORM_DATA.display;
		int screen = DefaultScreen(display);

		auto attributes = (XSetWindowAttributes)
		{
			.background_pixel = WhitePixel(display, screen),
			.event_mask = (1L << 25) - 1,
		};

		auto root = RootWindow(display, screen);
		if (parent) root = ((LnxWindowData*)parent->handle)->window;
		auto lnxData = new LnxWindowData();
		handle = lnxData;

		lnxData->window = XCreateWindow(display, root,
		data.position.x, data.position.y,
		data.size.x, data.size.y, 1,
		DefaultDepth(display, screen), InputOutput,
		DefaultVisual(display, screen),
		CWBackPixel | CWEventMask, &attributes);

		if(parent) XSetTransientForHint(display, lnxData->window, root);

		WIN_REGISTRY[lnxData->window] = this;

		XSetWMProtocols(display, lnxData->window, &PLATFORM_DATA.closeMessage, 1);

		XSetErrorHandler(XErrorHandler);
		XSetIOErrorHandler(XIOErrorHandler);

		auto pPos = Platform_GetPointerPosition(this);
		mouse.position = pPos;
		mouse.isInClientRegion = pPos.x >= 0 && pPos.x < rect.size.x && pPos.y > 0 && pPos.y < rect.size.y;
		eventData.pointerPosition = mouse.position;

		inter::window::SetFlags(this, data.flags);
		inter::window::Focus(this);
		inter::window::SetTitle(this, title.c_str());
	}

	Window::~Window()
	{
		inter::window::Close(this);
	}

	namespace inter::window
	{
		void SetTitle(const Window* win, const char* title)
		{
			if (!win->handle) return;
			XStoreName(PLATFORM_DATA.display, ((LnxWindowData*)win->handle)->window, title);
			XFlush(PLATFORM_DATA.display);
		}
		void SetSize(const Window* win, const ivec2 size)
		{
			if (!win->handle) return;
			XResizeWindow(PLATFORM_DATA.display, ((LnxWindowData*)win->handle)->window, size.x, size.y);
			XFlush(PLATFORM_DATA.display);
		}
		void SetPosition(const Window* win, const ivec2 position)
		{
			if (!win->handle) return;
			XMoveWindow(PLATFORM_DATA.display, ((LnxWindowData*)win->handle)->window, position.x, position.y);
			XFlush(PLATFORM_DATA.display);
		}
		void SetRect(const Window* win, const IRect rect)
		{
			SetPosition(win, rect.position);
			SetSize(win, rect.size);
		}

		void XIconify(const ::Window win, const int32_t id)
		{
			auto it = WIN_REGISTRY.find(win);
			if (it == WIN_REGISTRY.end()) return;
			XEvent event = {0};
			event.xclient.type = ClientMessage;
			event.xclient.message_type = PLATFORM_DATA.wmState;
			event.xclient.display = PLATFORM_DATA.display;
			event.xclient.window = win;
			event.xclient.format = 32;
			event.xclient.data.l[0] = id;
			event.xclient.data.l[1] = (long)PLATFORM_DATA.maxHorz;
			event.xclient.data.l[2] = (long)PLATFORM_DATA.maxVert;
			XSendEvent(PLATFORM_DATA.display, DefaultRootWindow(PLATFORM_DATA.display), False,
				SubstructureNotifyMask | SubstructureRedirectMask, &event);
		}

		void SetFlags(Window* win, WindowFlags flags)
		{
			if(!win->handle || win->flags == flags) return;

			auto& display = PLATFORM_DATA.display;
			auto window = ((LnxWindowData*)win->handle)->window;
			if(((int32_t)win->flags & (int32_t)WindowFlags::Visible) != ((int32_t)flags & (int32_t)WindowFlags::Visible))
			{
				if((int32_t)flags & (int32_t)WindowFlags::Visible) XMapWindow(display, window);
				else XUnmapWindow(display, window);
			}

			if(((int32_t)win->flags & (int32_t)WindowFlags::Minimized) != ((int32_t)flags & (int32_t)WindowFlags::Minimized) ||
			   ((int32_t)win->flags & (int32_t)WindowFlags::Maximized) != ((int32_t)flags & (int32_t)WindowFlags::Maximized))
			{
				uint32_t both = (int32_t)flags & (int32_t)WindowFlags::Minimized + (int32_t)flags & (int32_t)WindowFlags::Maximized;
				if((int32_t)flags & (int32_t)WindowFlags::Minimized) XIconifyWindow(display, window, DefaultScreen(display));
				if((int32_t)flags & (int32_t)WindowFlags::Maximized) XIconify(window, 1);
				if(both == 0) XIconify(window, 0);
			}

			XFlush(display);
			win->flags = flags;
		}

		void Focus(const Window* win)
		{
			if (!win->handle) return;
			auto& display = PLATFORM_DATA.display;
			XEvent event = {};
			event.xclient.type = ClientMessage;
			event.xclient.message_type = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
			event.xclient.display = display;
			event.xclient.window = ((LnxWindowData*)win->handle)->window;
			event.xclient.format = 32;
			event.xclient.data.l[0] = 1;  // 1 = normal request
			event.xclient.data.l[1] = CurrentTime;

			XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
			XFlush(display);
		}

		bool Close(Window* win)
		{
			if (win->handle)
			{
				auto window = ((LnxWindowData*)win->handle)->window;

				WIN_REGISTRY.erase(window);
				XDestroyWindow(PLATFORM_DATA.display, window);
				win->renderer = nullptr;
				delete((LnxWindowData*)win->handle);
				win->handle = nullptr;
				return true;
			}
			return false;
		}
	}
}