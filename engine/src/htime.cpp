#include "htime.h"
#include <chrono>
#include <thread>

namespace hf
{
	int16_t Time::s_TargetFrameRate = 60;
	double Time::s_ApplicationStartTime = 0;
	double Time::s_CurrentTime = 0;
	double Time::s_DeltaTime = 0;
	uint64_t Time::s_FrameCount = 0;
	double Time::s_AvgFrameRate = 0;

	void Time::PauseThread(double seconds)
	{
		std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
	}

	uint64_t Time::GetFrameCount() { return s_FrameCount; }
	double Time::GetDeltaTime() { return s_DeltaTime; }
	double Time::GetTimePassedAfterLaunch() { return s_CurrentTime - s_ApplicationStartTime; }
	int16_t Time::GetTargetFrameRate() { return s_TargetFrameRate; }
	double Time::GetAvgFrameRate() { return s_AvgFrameRate; }

	void Time::SetTargetFrameRate(int16_t targetFrameRate)
	{
		s_TargetFrameRate = targetFrameRate;
	}

	void Time::LoadTime(double currentTime)
	{

	}

	void Time::UpdateTime(double currentTime)
	{

	}
}