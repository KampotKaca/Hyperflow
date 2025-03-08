#ifndef HTIME_H
#define HTIME_H

#include "hshared.h"

namespace hf
{
	class Time
	{
	public:
		static uint64_t GetFrameCount();
		static double GetDeltaTime();
		static double GetTimePassedAfterLaunch();
		static int16_t GetTargetFrameRate();
		static double GetSystemTime();

		static void SetTargetFrameRate(int16_t targetFrameRate);

	private:
		static int16_t s_TargetFrameRate;
		static double s_TargetFrameDuration;
		static double s_ApplicationStartTime;
		static double s_CurrentTime;
		static double s_DeltaTime;
		static uint64_t s_FrameCount;
	};
}

#endif //HTIME_H
