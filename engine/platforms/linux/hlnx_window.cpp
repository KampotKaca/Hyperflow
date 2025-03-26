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
	int32_t XErrorHandler(Display *display, XErrorEvent *error)
	{
		throw WND_EXCEPT(error->error_code);
	}

	int32_t XIOErrorHandler(Display *display)
	{
		throw WND_EXCEPT(EXIT_FAILURE);
	}

	Window::Window(void* platformHandle, const WindowData& data, const Ref<Window>& parent)
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

		auto display = (Display*)platformHandle;
		int screen = DefaultScreen(display);

		auto attributes = (XSetWindowAttributes)
		{
			.background_pixel = WhitePixel(display, screen),
			.event_mask = KeyPressMask | KeyReleaseMask |
						  ButtonPressMask | ButtonReleaseMask |
						  VisibilityChangeMask | ResizeRedirectMask |
						  StructureNotifyMask,
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

		Platform_PushWindowToRegistry(lnxData->windowHandle, this);

		lnxData->closeMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
		lnxData->wmState = XInternAtom(display, "_NET_WM_STATE", False);
		lnxData->maxHorz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
		lnxData->maxVert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);

		XSetWMProtocols(display, lnxData->windowHandle, &lnxData->closeMessage, 1);

		XSetErrorHandler(XErrorHandler);
		XSetIOErrorHandler(XIOErrorHandler);

		auto pPos = Platform_GetPointerPosition(this);
		m_Mouse = MakeRef<Mouse>(pPos, pPos.x >= 0 && pPos.x < m_Rect.size.x && pPos.y > 0 && pPos.y < m_Rect.size.y);
		m_Keyboard = MakeRef<Keyboard>();
		m_EventData.pointerPosition = m_Mouse->m_Position;

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
		XStoreName((Display*)Hyperflow::GetPlatformHandle(), ((LnxWindowData*)m_Handle)->windowHandle, title);
	}
	void Window::SetSize(ivec2 size) const
	{
		XResizeWindow((Display*)Hyperflow::GetPlatformHandle(), ((LnxWindowData*)m_Handle)->windowHandle, size.x, size.y);
	}
	void Window::SetPosition(ivec2 position) const
	{
		XMoveWindow((Display*)Hyperflow::GetPlatformHandle(), ((LnxWindowData*)m_Handle)->windowHandle, position.x, position.y);
	}
	void Window::SetRect(IRect rect) const
	{
		SetPosition(rect.position);
		SetSize(rect.size);
	}

	void XIconify(Display* display, ::Window window, int32_t id)
	{
		auto winPtr = Platform_GetWinPtr(window);
		if (!winPtr) return;
		auto lnxData = (LnxWindowData*)winPtr->m_Handle;
		XEvent event = {0};
		event.xclient.type = ClientMessage;
		event.xclient.message_type = lnxData->wmState;
		event.xclient.display = display;
		event.xclient.window = window;
		event.xclient.format = 32;
		event.xclient.data.l[0] = id;
		event.xclient.data.l[1] = (long)lnxData->maxHorz;
		event.xclient.data.l[2] = (long)lnxData->maxVert;
		XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
	}

	void Window::SetFlags(WindowFlags flags)
	{
		if(m_Flags == flags) return;

		auto display = (Display*)Hyperflow::GetPlatformHandle();
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
			if((int32_t)flags & (int32_t)WindowFlags::Maximized) XIconify(display, window, 1);
			if(both == 0) XIconify(display, window, 0);
		}

		m_Flags = flags;
	}

	void Window::Focus() const
	{
		auto display = (Display*)Hyperflow::GetPlatformHandle();
		XEvent event = {0};
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
			Platform_PopWindowFromRegistry(window);
			XDestroyWindow((Display*)Hyperflow::GetPlatformHandle(), window);
			delete((LnxWindowData*)m_Handle);
			m_Handle = nullptr;
			return true;
		}
        return false;
	}
}