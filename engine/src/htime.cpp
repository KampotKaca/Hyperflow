#include "htime.h"
#include <chrono>
#include <thread>
#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
	Time::Time()
	{
		creationTime = GetSystemTime();
		currentTime = creationTime;
	}

	Time::~Time() = default;

	void Time::StartFrame()
	{
		double_t cTime = GetSystemTime();

		if(targetFrameRate > 0)
		{
			auto diff = targetFrameDuration - (cTime - currentTime);
			if(diff > .001)
			{
				inter::platform::Sleep(diff);
				cTime = GetSystemTime();
			}
		}

		deltaTime = cTime - currentTime;
		currentTime = cTime;
		frameCount++;

		const double_t current = 1.0 / deltaTime;
		frameRate = std::lerp(frameRate, current, 5 * deltaTime);
	}

	double_t Time::GetTimePassed() const { return currentTime - creationTime; }
	double_t Time::GetAbsoluteTimePassed() const { return GetSystemTime() - creationTime; }

	void Time::SetTargetFrameRate(int16_t targetFrameRate)
	{
		this->targetFrameRate = targetFrameRate;
		targetFrameDuration = (1.0 / targetFrameRate);
	}

	int32_t Time::GetFrameRate() const { return (int32_t)glm::round(frameRate); }

    TimeUniformInfo Time::GetUniformInfo()const
	{
	    TimeUniformInfo info{};
	    info.deltaTime = deltaTime;
	    info.timeSinceStartup = GetTimePassed();

	    return info;
	}

	uint64_t GetFrameCount() { return inter::HF.time.frameCount; }
	double_t GetDeltaTime(){ return inter::HF.time.deltaTime; }
	double_t GetTimePassed() { return inter::HF.time.GetTimePassed(); }
	double_t GetAbsoluteTimePassed() { return inter::HF.time.GetAbsoluteTimePassed(); }
	int16_t GetTargetFrameRate() { return inter::HF.time.targetFrameRate; }
	int32_t GetFrameRate() { return inter::HF.time.GetFrameRate(); }
	void SetTargetFrameRate(int16_t targetFrameRate) { return inter::HF.time.SetTargetFrameRate(targetFrameRate); }

	double_t GetSystemTime()
	{
		using namespace std::chrono;
		const auto time = high_resolution_clock::now().time_since_epoch();
		return duration<double_t>(time).count();
	}
}