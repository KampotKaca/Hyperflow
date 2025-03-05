#ifndef HTIME_H
#define HTIME_H

#include "hshared.h"

namespace hf
{
	class Time
	{
	public:
		static void PauseThread(double seconds);
		static uint64_t GetFrameCount();
		static double GetDeltaTime();
		static double GetTimePassedAfterLaunch();
		static int16_t GetTargetFrameRate();

		static double GetAvgFrameRate();
		static void SetTargetFrameRate(int16_t targetFrameRate);

		static void LoadTime(double currentTime);
		static void UpdateTime(double currentTime);

	private:
		static int16_t s_TargetFrameRate;
		static double s_ApplicationStartTime;
		static double s_CurrentTime;
		static double s_DeltaTime;
		static uint64_t s_FrameCount;
		static double s_AvgFrameRate;
	};
}

#endif //HTIME_H
