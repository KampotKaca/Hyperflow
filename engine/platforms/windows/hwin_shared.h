#ifndef HWIN_SHARED_H
#define HWIN_SHARED_H

#include "hshared.h"

namespace hf
{
	void Windows_ConvertSize(Window* window, glm::ivec2& size);
	uint64_t Windows_GetTicks();
	uint32_t Windows_GetStyleID(WindowStyle style);
}

#endif //HWIN_SHARED_H
