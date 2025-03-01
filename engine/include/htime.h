#ifndef HTIME_H
#define HTIME_H

#include "hshared.h"

namespace hf
{
	extern void hPauseThread(uint32_t milliseconds);
	extern uint64_t hGetFrameCount();
	extern double hGetDeltaTime();
	extern double hGetTimePassedAfterLaunch();
	extern int16_t hGetTargetFrameRate();

	extern int16_t hGetCurrentFrameRate();

	extern void hSetTargetFrameRate(int16_t targetFrameRate);
}

#endif //HTIME_H
