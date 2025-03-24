#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define private public
#include <hyperflow.h>
#include "hwindow.h"
#include "hkeyboard.h"
#include "hmouse.h"
#undef private

#include "hplatform.h"
#include "exceptions/hwindowexception.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

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
		auto root = RootWindow(display, screen);
		if (parent != nullptr) root = (::Window)m_Parent->m_Handle;
		m_Handle = (void*)XCreateSimpleWindow(display, root, data.position.x, data.position.y,
		data.size.x, data.size.y, 1, BlackPixel(display, screen), WhitePixel(display, screen));

		XSelectInput(display, (::Window)m_Handle, (1L << 25) - 1);
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
		XStoreName((Display*)Hyperflow::GetPlatformHandle(), (::Window)m_Handle, title);
	}
	void Window::SetSize(glm::ivec2 size) const
	{
		XResizeWindow((Display*)Hyperflow::GetPlatformHandle(), (::Window)m_Handle, size.x, size.y);
	}
	void Window::SetPosition(glm::ivec2 position) const
	{
		XMoveWindow((Display*)Hyperflow::GetPlatformHandle(), (::Window)m_Handle, position.x, position.y);
	}
	void Window::SetRect(IRect rect) const
	{
		SetPosition(rect.position);
		SetSize(rect.size);
	}

	void XIconify(Display* display, ::Window window, int32_t id)
	{
		Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
		Atom max_horz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
		Atom max_vert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
		XEvent event = {0};
		event.xclient.type = ClientMessage;
		event.xclient.message_type = wm_state;
		event.xclient.display = display;
		event.xclient.window = window;
		event.xclient.format = 32;
		event.xclient.data.l[0] = id;
		event.xclient.data.l[1] = max_horz;
		event.xclient.data.l[2] = max_vert;
		XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
	}

	void Window::SetFlags(WindowFlags flags)
	{
		if(m_Flags == flags) return;

		auto display = (Display*)Hyperflow::GetPlatformHandle();
		auto window = (::Window)m_Handle;
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
		event.xclient.window = (::Window)m_Handle;
		event.xclient.format = 32;
		event.xclient.data.l[0] = 1;  // 1 = normal request
		event.xclient.data.l[1] = CurrentTime;

		XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
	}

	bool Window::Close()
	{
		if (m_Handle)
		{
			XDestroyWindow((Display*)Hyperflow::GetPlatformHandle(), (::Window)m_Handle);
			m_Handle = nullptr;
			return true;
		}
        return false;
	}
}