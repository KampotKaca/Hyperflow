#ifndef HWIN_SHARED_H
#define HWIN_SHARED_H

#include "hwindows.h"
#include "hshared.h"

namespace hf
{
	void Windows_ConvertSize(const Window* window, ivec2& size);
	uint64_t Windows_GetTicks();
	uint32_t Windows_GetStyleID(WindowStyle style);

	struct Win_Platform_Data
	{
		HINSTANCE instance;
	};

	extern Win_Platform_Data PLATFORM_DATA;
}

#endif //HWIN_SHARED_H
