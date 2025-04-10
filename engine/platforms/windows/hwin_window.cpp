#include "hwin_shared.h"

#include "hyperflow.h"
#include "hinternal.h"
#include "hwindow.h"
#include "hplatform.h"
#include "exceptions/hwindowexception.h"

namespace hf
{
	Window::Window(const WindowData& data, const Ref<Window>& parent)
	{
		uint32_t currentStyle = Windows_GetStyleID(data.style);

		title = data.title;
		style = data.style;
		this->parent = parent;
		flags = (WindowFlags)0;
		rect =
		{
			.position = data.position,
			.size = data.size
		};

		HWND parentHandle = nullptr;
		if(parent != nullptr) parentHandle = (HWND)parent->handle;

		ivec2 convertedSize = data.size;
		Windows_ConvertSize(this, convertedSize);

		handle = CreateWindowEx
		(
			0,
			WINDOWS_CLASS_NAME,
			data.title.c_str(),
			currentStyle,
			data.position[0], data.position[1],
			convertedSize[0], convertedSize[1],
			parentHandle,
			nullptr,
			PLATFORM_DATA.instance,
			this
		);

		if(handle == nullptr) throw WND_LAST_EXCEPT();

		auto pPos = Platform_GetPointerPosition(this);
		mouse.position = pPos;
		mouse.isInClientRegion = pPos.x >= 0 && pPos.x < rect.size.x && pPos.y > 0 && pPos.y < rect.size.y;
		eventData.pointerPosition = mouse.position;

		inter::window::SetFlags(this, data.flags);
		inter::window::Focus(this);
	}

	Window::~Window()
	{
		inter::window::Close(this);
	}

	namespace inter::window
	{
		void SetTitle(const Window* win, const char* title)
		{
			if(!SetWindowText((HWND)win->handle, title)) throw WND_LAST_EXCEPT();
		}
		void SetSize(const Window* win, ivec2 size)
		{
			Windows_ConvertSize(win, size);
			if(!SetWindowPos((HWND)win->handle, nullptr, 0, 0, size[0], size[1], SWP_NOMOVE))
				throw WND_LAST_EXCEPT();
		}
		void SetPosition(const Window* win, ivec2 position)
		{
			if(!SetWindowPos((HWND)win->handle, nullptr, position[0], position[1], 0, 0, SWP_NOSIZE))
				throw WND_LAST_EXCEPT();
		}
		void SetRect(const Window* win, IRect rect)
		{
			Windows_ConvertSize(win, rect.size);
			if(!SetWindowPos((HWND)win->handle, nullptr, rect.position[0], rect.position[1], rect.size[0], rect.size[1], 0))
				throw WND_LAST_EXCEPT();
		}

		void SetFlags(Window* win, WindowFlags flags)
		{
			if(win->flags == flags) return;

			if(((int32_t)win->flags & (int32_t)WindowFlags::Visible) != ((int32_t)flags & (int32_t)WindowFlags::Visible))
			{
				if((int32_t)flags & (int32_t)WindowFlags::Visible) ShowWindow((HWND)win->handle, SW_SHOW);
				else ShowWindow((HWND)win->handle, SW_HIDE);
			}

			if(((int32_t)win->flags & (int32_t)WindowFlags::Minimized) != ((int32_t)flags & (int32_t)WindowFlags::Minimized) ||
			   ((int32_t)win->flags & (int32_t)WindowFlags::Maximized) != ((int32_t)flags & (int32_t)WindowFlags::Maximized))
			{
				uint32_t both = (int32_t)flags & (int32_t)WindowFlags::Minimized + (int32_t)flags & (int32_t)WindowFlags::Maximized;
				if((int32_t)flags & (int32_t)WindowFlags::Minimized) ShowWindow((HWND)win->handle, SW_MINIMIZE);
				if((int32_t)flags & (int32_t)WindowFlags::Maximized) ShowWindow((HWND)win->handle, SW_MAXIMIZE);
				if(both == 0) ShowWindow((HWND)win->handle, SW_RESTORE);
			}

			win->flags = flags;
		}

		void Focus(const Window* win)
		{
			SetFocus((HWND)win->handle);
			SetForegroundWindow((HWND)win->handle);
		}
		bool Close(Window* win)
		{
			if (win->handle && IsWindow((HWND)win->handle))
			{
				delete(win->renderer);
				if(!DestroyWindow((HWND)win->handle)) throw WND_LAST_EXCEPT();
				win->handle = nullptr;
				return true;
			}

			return false;
		}
	}
}