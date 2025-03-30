#define private public
#include "components/htime.h"
#undef private

#include <chrono>
#include <thread>
#include "hplatform.h"
#include "hyperflow.h"

namespace hf
{
	Time::Time()
	{
		creationTime = time::GetSystemTime();
		currentTime = creationTime;
	}

	Time::~Time()
	{

	}

	void Time::Update()
	{
		double cTime = time::GetSystemTime();

		if(targetFrameRate > 0)
		{
			auto diff = targetFrameDuration - (cTime - currentTime);
			if(diff > .001)
			{
				Platform_Sleep(diff);
				cTime = time::GetSystemTime();
			}
		}

		deltaTime = cTime - currentTime;
		currentTime = cTime;
		frameCount++;

		double current = 1.0 / GetDeltaTime();
		frameRate = std::lerp(frameRate, current, 5 * deltaTime);
	}

	uint64_t Time::GetFrameCount() const { return frameCount; }
	double Time::GetDeltaTime() const { return deltaTime; }
	double Time::GetTimePassed() const { return currentTime - creationTime; }
	double Time::GetAbsoluteTimePassed() const { return time::GetSystemTime() - creationTime; }
	int16_t Time::GetTargetFrameRate() const { return targetFrameRate; }

	void Time::SetTargetFrameRate(int16_t targetFrameRate)
	{
		this->targetFrameRate = targetFrameRate;
		targetFrameDuration = (1.0 / targetFrameRate);
	}

	int32_t Time::GetFrameRate() const { return (int32_t)glm::round(frameRate); }
}