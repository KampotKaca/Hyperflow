#define private public
#include "components/htime.h"
#undef private

#include "components/hinternal.h"
#include <chrono>
#include <thread>
#include "hplatform.h"

namespace hf
{
	int16_t Time::s_TargetFrameRate = 50;
	double Time::s_TargetFrameDuration = (1.0 / s_TargetFrameRate);
	double Time::s_ApplicationStartTime = 0;
	double Time::s_CurrentTime = 0;
	double Time::s_DeltaTime = s_TargetFrameDuration;
	uint64_t Time::s_FrameCount = 0;
	double Time::s_FrameRate = 0;

	uint64_t Time::GetFrameCount() { return s_FrameCount; }
	double Time::GetDeltaTime() { return s_DeltaTime; }
	double Time::GetTimePassed() { return s_CurrentTime - s_ApplicationStartTime; }
	double Time::GetAbsoluteTimePassed() { return GetSystemTime() - s_ApplicationStartTime; }
	int16_t Time::GetTargetFrameRate() { return s_TargetFrameRate; }

	double Time::GetSystemTime()
	{
		using namespace std::chrono;
		auto time = high_resolution_clock::now().time_since_epoch();
		return duration<double>(time).count();
	}

	void Time::SetTargetFrameRate(int16_t targetFrameRate)
	{
		s_TargetFrameRate = targetFrameRate;
		s_TargetFrameDuration = (1.0 / s_TargetFrameRate);
	}

	int32_t Time::GetFrameRate() { return (int32_t)glm::round(s_FrameRate); }

	void Time_Load()
	{
		Time::s_ApplicationStartTime = Time::GetSystemTime();
		Time::s_CurrentTime = Time::s_ApplicationStartTime;
	}

	void Time_Update()
	{
		double currentTime = Time::GetSystemTime();

		if(Time::s_TargetFrameRate > 0)
		{
			auto diff = Time::s_TargetFrameDuration - (currentTime - Time::s_CurrentTime);
			if(diff > .001)
			{
				Platform_Sleep(diff);
				currentTime = Time::GetSystemTime();
			}
		}

		Time::s_DeltaTime = currentTime - Time::s_CurrentTime;
		Time::s_CurrentTime = currentTime;
		Time::s_FrameCount++;

		double current = 1.0 / hf::Time::GetDeltaTime();
		Time::s_FrameRate = std::lerp(Time::s_FrameRate, current, 5 * Time::s_DeltaTime);
	}
}