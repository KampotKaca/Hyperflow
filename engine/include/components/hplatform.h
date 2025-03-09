#ifndef HPLATFORM_H
#define HPLATFORM_H

#include "hshared.h"

namespace hf
{
	void Platform_SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value);
	void Platform_HandleEvents(std::vector<Ref<Window>>& windows, EngineUpdateType updateType);
	void Platform_Initialize();
	void Platform_BeginTemporarySystemTimer(uint16_t millisecondPrecision);
	void Platform_EndTemporarySystemTimer(uint16_t millisecondPrecision);
	uint64_t Platform_GetTicks();
	void Platform_Sleep(double seconds);
}

#endif //HPLATFORM_H
