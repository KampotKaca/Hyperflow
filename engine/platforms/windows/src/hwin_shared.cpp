#include "hwin_shared.h"
#include "hwindowexception.h"

namespace hf
{
	Win_Platform_Data PLATFORM_DATA;

	void Windows_ConvertSize(const Window* window, ivec2& size)
	{
		uint32_t currentStyle = Windows_GetStyleID(window->style);
		RECT targetRect = { 0, 0, size[0], size[1] };
		if(!AdjustWindowRectEx(&targetRect, currentStyle, false, 0)) throw WND_LAST_EXCEPT();
		size[0] = targetRect.right - targetRect.left;
		size[1] = targetRect.bottom - targetRect.top;
	}

	uint64_t Windows_GetTicks()
	{
		return GetTickCount64();
	}

	uint32_t Windows_GetStyleID(WindowStyle style)
	{
		switch (style)
		{
			case WindowStyle::Default: return WS_SIZEBOX | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
		}
		return 0;
	}
}