#include "hwindows.h"

#define private public
#include <hyperflow.h>
#include "hwindow.h"
#include "hkeyboard.h"
#include "hmouse.h"
#undef private

#include "hwin_shared.h"
#include "hplatform.h"
#include "exceptions/hwindowexception.h"

namespace hf
{
	Window::Window(void* platformHandle, const WindowData& data, const Ref<Window>& parent)
	{
		uint32_t currentStyle = Windows_GetStyleID(data.style);

		m_Title = data.title;
		m_Style = data.style;
		m_Parent = parent;
		m_Flags = (WindowFlags)0;
		m_Rect =
		{
			.position = data.position,
			.size = data.size
		};

		HWND parentHandle = nullptr;
		if(parent != nullptr) parentHandle = (HWND)parent->m_Handle;

		glm::ivec2 convertedSize = data.size;
		Windows_ConvertSize(this, convertedSize);

		m_Handle = CreateWindowEx
		(
			0,
			WINDOWS_CLASS_NAME,
			data.title.c_str(),
			currentStyle,
			data.position[0], data.position[1],
			convertedSize[0], convertedSize[1],
			parentHandle,
			nullptr,
			(HINSTANCE)platformHandle,
			this
		);

		if(m_Handle == nullptr) throw WND_LAST_EXCEPT();

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
		if(!SetWindowText((HWND)m_Handle, title)) throw WND_LAST_EXCEPT();
	}
	void Window::SetSize(glm::ivec2 size) const
	{
		Windows_ConvertSize(this, size);
		if(!SetWindowPos((HWND)m_Handle, nullptr, 0, 0, size[0], size[1], SWP_NOMOVE))
			throw WND_LAST_EXCEPT();
	}
	void Window::SetPosition(glm::ivec2 position) const
	{
		if(!SetWindowPos((HWND)m_Handle, nullptr, position[0], position[1], 0, 0, SWP_NOSIZE))
			throw WND_LAST_EXCEPT();
	}
	void Window::SetRect(IRect rect) const
	{
		Windows_ConvertSize(this, rect.size);
		if(!SetWindowPos((HWND)m_Handle, nullptr, rect.position[0], rect.position[1], rect.size[0], rect.size[1], 0))
			throw WND_LAST_EXCEPT();
	}

	void Window::SetFlags(WindowFlags flags)
	{
		if(m_Flags == flags) return;

		if(((int32_t)m_Flags & (int32_t)WindowFlags::Visible) != ((int32_t)flags & (int32_t)WindowFlags::Visible))
		{
			if((int32_t)flags & (int32_t)WindowFlags::Visible) ShowWindow((HWND)m_Handle, SW_SHOW);
			else ShowWindow((HWND)m_Handle, SW_HIDE);
		}

		if(((int32_t)m_Flags & (int32_t)WindowFlags::Minimized) != ((int32_t)flags & (int32_t)WindowFlags::Minimized) ||
		   ((int32_t)m_Flags & (int32_t)WindowFlags::Maximized) != ((int32_t)flags & (int32_t)WindowFlags::Maximized))
		{
			uint32_t both = (int32_t)flags & (int32_t)WindowFlags::Minimized + (int32_t)flags & (int32_t)WindowFlags::Maximized;
			if((int32_t)flags & (int32_t)WindowFlags::Minimized) ShowWindow((HWND)m_Handle, SW_MINIMIZE);
			if((int32_t)flags & (int32_t)WindowFlags::Maximized) ShowWindow((HWND)m_Handle, SW_MAXIMIZE);
			if(both == 0) ShowWindow((HWND)m_Handle, SW_RESTORE);
		}

		m_Flags = flags;
	}

	void Window::Focus() const
	{
		SetFocus((HWND)m_Handle);
		SetForegroundWindow((HWND)m_Handle);
	}
	bool Window::Close()
	{
		if (m_Handle && IsWindow((HWND)m_Handle))
		{
			if(!DestroyWindow((HWND)m_Handle)) throw WND_LAST_EXCEPT();
			m_Handle = nullptr;
			Hyperflow::ClearWindow(this);
			return true;
		}

		return false;
	}
}