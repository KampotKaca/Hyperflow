#define private public
#include <hyperflow.h>
#include "hwindow.h"
#include "hkeyboard.h"
#include "hmouse.h"
#undef private

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
		m_Title = data.title;
		m_Style = data.style;
		m_Parent = parent;
		m_Flags = (WindowFlags)0;
		m_Rect =
		{
			.position = data.position,
			.size = data.size
		};

		auto display = PLATFORM_DATA.display;
		int screen = DefaultScreen(display);

		auto attributes = (XSetWindowAttributes)
		{
			.background_pixel = WhitePixel(display, screen),
			.event_mask = (1L << 25) - 1,
		};

		auto root = RootWindow(display, screen);
		if (parent) root = ((LnxWindowData*)m_Parent->m_Handle)->windowHandle;
		auto lnxData = new LnxWindowData();
		m_Handle = lnxData;

		lnxData->windowHandle = XCreateWindow(display, root,
		data.position.x, data.position.y,
		data.size.x, data.size.y, 1,
		DefaultDepth(display, screen), InputOutput,
		DefaultVisual(display, screen),
		CWBackPixel | CWEventMask, &attributes);

		if(parent) XSetTransientForHint(display, lnxData->windowHandle, root);

		WIN_REGISTRY[lnxData->windowHandle] = this;

		XSetWMProtocols(display, lnxData->windowHandle, &PLATFORM_DATA.closeMessage, 1);

		XSetErrorHandler(XErrorHandler);
		XSetIOErrorHandler(XIOErrorHandler);

		auto pPos = Platform_GetPointerPosition(this);
		m_Mouse = MakeRef<Mouse>(pPos, pPos.x >= 0 && pPos.x < m_Rect.size.x && pPos.y > 0 && pPos.y < m_Rect.size.y);
		m_Keyboard = MakeRef<Keyboard>();
		m_EventData.pointerPosition = m_Mouse->GetPosition();

		SetFlags(data.flags);
		Focus();
		SetTitle(m_Title.c_str());
	}

	Window::~Window()
	{
		Close();
	}

	void Window::SetTitle(const char* title) const
	{
		XStoreName(PLATFORM_DATA.display, ((LnxWindowData*)m_Handle)->windowHandle, title);
	}
	void Window::SetSize(ivec2 size) const
	{
		XResizeWindow(PLATFORM_DATA.display, ((LnxWindowData*)m_Handle)->windowHandle, size.x, size.y);
	}
	void Window::SetPosition(ivec2 position) const
	{
		XMoveWindow(PLATFORM_DATA.display, ((LnxWindowData*)m_Handle)->windowHandle, position.x, position.y);
	}
	void Window::SetRect(IRect rect) const
	{
		SetPosition(rect.position);
		SetSize(rect.size);
	}

	void XIconify(::Window window, int32_t id)
	{
		auto it = WIN_REGISTRY.find(window);
		if (it == WIN_REGISTRY.end()) return;
		XEvent event = {0};
		event.xclient.type = ClientMessage;
		event.xclient.message_type = PLATFORM_DATA.wmState;
		event.xclient.display = PLATFORM_DATA.display;
		event.xclient.window = window;
		event.xclient.format = 32;
		event.xclient.data.l[0] = id;
		event.xclient.data.l[1] = (long)PLATFORM_DATA.maxHorz;
		event.xclient.data.l[2] = (long)PLATFORM_DATA.maxVert;
		XSendEvent(PLATFORM_DATA.display, DefaultRootWindow(PLATFORM_DATA.display), False,
			SubstructureNotifyMask | SubstructureRedirectMask, &event);
	}

	void Window::SetFlags(WindowFlags flags)
	{
		if(m_Flags == flags) return;

		auto& display = PLATFORM_DATA.display;
		auto window = ((LnxWindowData*)m_Handle)->windowHandle;
		if(((int32_t)m_Flags & (int32_t)WindowFlags::Visible) != ((int32_t)flags & (int32_t)WindowFlags::Visible))
		{
			if((int32_t)flags & (int32_t)WindowFlags::Visible) XMapWindow(display, window);
			else XUnmapWindow(display, window);
		}

		if(((int32_t)m_Flags & (int32_t)WindowFlags::Minimized) != ((int32_t)flags & (int32_t)WindowFlags::Minimized) ||
		   ((int32_t)m_Flags & (int32_t)WindowFlags::Maximized) != ((int32_t)flags & (int32_t)WindowFlags::Maximized))
		{
			uint32_t both = (int32_t)flags & (int32_t)WindowFlags::Minimized + (int32_t)flags & (int32_t)WindowFlags::Maximized;
			if((int32_t)flags & (int32_t)WindowFlags::Minimized) XIconifyWindow(display, window, DefaultScreen(display));
			if((int32_t)flags & (int32_t)WindowFlags::Maximized) XIconify(window, 1);
			if(both == 0) XIconify(window, 0);
		}

		m_Flags = flags;
	}

	void Window::Focus() const
	{
		auto& display = PLATFORM_DATA.display;
		XEvent event = {};
		event.xclient.type = ClientMessage;
		event.xclient.message_type = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
		event.xclient.display = display;
		event.xclient.window = ((LnxWindowData*)m_Handle)->windowHandle;
		event.xclient.format = 32;
		event.xclient.data.l[0] = 1;  // 1 = normal request
		event.xclient.data.l[1] = CurrentTime;

		XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
	}

	bool Window::Close()
	{
		if (m_Handle)
		{
			auto window = ((LnxWindowData*)m_Handle)->windowHandle;

			WIN_REGISTRY.erase(window);
			XDestroyWindow(PLATFORM_DATA.display, window);
			delete((LnxWindowData*)m_Handle);
			m_Handle = nullptr;
			return true;
		}
        return false;
	}
}