#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define private public
#include <hyperflow.h>
#include "hwindow.h"
#include "hkeyboard.h"
#include "hmouse.h"
#undef private

#include "hplatform.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace hf
{
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
		XMapWindow(display, (::Window)m_Handle);

		auto pPos = Platform_GetPointerPosition(this);
		m_Mouse = MakeRef<Mouse>(pPos, pPos.x >= 0 && pPos.x < m_Rect.size.x && pPos.y > 0 && pPos.y < m_Rect.size.y);
		m_Keyboard = MakeRef<Keyboard>();
		m_EventData.pointerPosition = m_Mouse->m_Position;

		SetFlags(data.flags);
		Focus();
	}

	Window::~Window()
	{
		Close();
	}

	void Window::SetTitle(const char* title) const
	{

	}
	void Window::SetSize(glm::ivec2 size) const
	{

	}
	void Window::SetPosition(glm::ivec2 position) const
	{

	}
	void Window::SetRect(IRect rect) const
	{

	}

	void Window::SetFlags(WindowFlags flags)
	{
		if(m_Flags == flags) return;



		m_Flags = flags;
	}

	void Window::Focus() const
	{

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