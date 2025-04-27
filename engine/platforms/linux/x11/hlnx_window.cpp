#include <hyperflow.h>
#include "../../../include/hwindow.h"
#include "hinternal.h"
#include "../../../include/hplatform.h"
#include "hlnx_eventhandling.h"
#include "exceptions/hwindowexception.h"
#include "hlnx_window.h"

#include <dlfcn.h>
#include <bits/dlfcn.h>

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

	namespace inter::window
	{
		void SetTitle(const Window* win, const std::string& title)
		{
			if (!win->handle) return;
			XStoreName(PLATFORM_DATA.display, ((LnxWindowData*)win->handle)->window, title.c_str());
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

		void Open(Window* win)
		{
			auto display = PLATFORM_DATA.display;
			int screen = DefaultScreen(display);

			auto attributes = (XSetWindowAttributes)
			{
				.background_pixel = WhitePixel(display, screen),
				.event_mask = StructureNotifyMask | ExposureMask | FocusChangeMask | KeyPressMask |
			  				  KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
			  				  PointerMotionMask | EnterWindowMask | LeaveWindowMask,
			};

			auto root = RootWindow(display, screen);
			if (win->parent) root = ((LnxWindowData*)win->parent->handle)->window;
			auto lnxData = new LnxWindowData();
			win->handle = lnxData;

			lnxData->window = XCreateWindow(display, root,
			win->rect.position.x, win->rect.position.y,
			win->rect.size.x, win->rect.size.y, 1,
			DefaultDepth(display, screen), InputOutput,
			DefaultVisual(display, screen),
			CWBackPixel | CWEventMask, &attributes);

			if(win->parent) XSetTransientForHint(display, lnxData->window, root);

			WIN_REGISTRY[lnxData->window] = win;

			XSetWMProtocols(display, lnxData->window, &PLATFORM_DATA.closeMessage, 1);

			XSetErrorHandler(XErrorHandler);
			XSetIOErrorHandler(XIOErrorHandler);
		}

		bool Close(Window* win)
		{
			if (win->handle)
			{
				auto window = ((LnxWindowData*)win->handle)->window;

				WIN_REGISTRY.erase(window);
				XDestroyWindow(PLATFORM_DATA.display, window);
				rendering::DestroyRenderer(win->renderer.get());
				win->renderer = nullptr;
				delete((LnxWindowData*)win->handle);
				win->handle = nullptr;
				win->parent = nullptr;
				return true;
			}
			return false;
		}

		void* GetWindowHandle(const Window* win)
		{
			return (void*)((LnxWindowData*)win->handle)->window;
		}
	}

	namespace inter
	{
		void* LoadDll(const char* dllName)
		{
			char exePath[PATH_MAX];
			ssize_t count = readlink("/proc/self/exe", exePath, PATH_MAX);
			std::string baseDir = std::string(exePath, (count > 0) ? count : 0);

			size_t lastSlash = baseDir.find_last_of('/');
			std::string exeDir = baseDir.substr(0, lastSlash + 1);

			std::string path = exeDir + "lib" + dllName + ".so";
			if (!utils::FileExists(path.c_str()))
				throw GENERIC_EXCEPT("[Hyperflow]", "Unable to find dll at path %s", path.c_str());

			void* dll = dlopen(path.c_str(), RTLD_NOW);
			if (!dll) throw GENERIC_EXCEPT("[Hyperflow]", "Unable to load dll\n[Error] %s", dlerror());
			return dll;
		}

		void* GetFuncPtr(void* dll, const char* funcName)
		{
			return dlsym(dll, funcName);
		}

		void UnloadDll(void* dll)
		{
			dlclose(dll);
		}

		void* GetPlatformInstance()
		{
			return PLATFORM_DATA.display;
		}
	}
}
