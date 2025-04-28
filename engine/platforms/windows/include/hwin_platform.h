#ifndef HWIN_PLATFORM_H
#define HWIN_PLATFORM_H

#include "hwin_shared.h"

namespace hf
{
	void Win_Load();
	void Win_Unload();

	void Win_BeginTemporarySystemTimer(uint16_t millisecondPrecision);
	void Win_EndTemporarySystemTimer(uint16_t millisecondPrecision);
	void Win_Sleep(double seconds);

	ivec2 Win_GetPointerPosition(const Window* window);
	void Win_SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value);
	void Win_HandleEvents(EngineUpdateType updateType);

	HMODULE Win_LoadDll(const char* dllName);
	void Win_UnloadDll(HMODULE dll);
	FARPROC Win_GetFuncPtr(void* dll, const char* funcName);
}

#endif //HWIN_PLATFORM_H
