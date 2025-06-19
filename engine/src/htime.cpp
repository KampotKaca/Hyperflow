#include "htime.h"
#include <chrono>
#include <thread>
#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
	Time::Time()
	{
		creationTime = time::GetSystemTime();
		currentTime = creationTime;
	}

	Time::~Time() = default;

	void Time::StartFrame()
	{
		double cTime = time::GetSystemTime();

		if(targetFrameRate > 0)
		{
			auto diff = targetFrameDuration - (cTime - currentTime);
			if(diff > .001)
			{
				inter::platform::Sleep(diff);
				cTime = time::GetSystemTime();
			}
		}

		deltaTime = cTime - currentTime;
		currentTime = cTime;
		frameCount++;

		double current = 1.0 / deltaTime;
		frameRate = std::lerp(frameRate, current, 5 * deltaTime);
	}

	double Time::GetTimePassed() const { return currentTime - creationTime; }
	double Time::GetAbsoluteTimePassed() const { return time::GetSystemTime() - creationTime; }

	void Time::SetTargetFrameRate(int16_t targetFrameRate)
	{
		this->targetFrameRate = targetFrameRate;
		targetFrameDuration = (1.0 / targetFrameRate);
	}

	int32_t Time::GetFrameRate() const { return (int32_t)glm::round(frameRate); }

	namespace time
	{
		uint64_t GetFrameCount() { return inter::HF.time.frameCount; }
		double GetDeltaTime(){ return inter::HF.time.deltaTime; }
		double GetTimePassed() { return inter::HF.time.GetTimePassed(); }
		double GetAbsoluteTimePassed() { return inter::HF.time.GetAbsoluteTimePassed(); }
		int16_t GetTargetFrameRate() { return inter::HF.time.targetFrameRate; }
		int32_t GetFrameRate() { return inter::HF.time.GetFrameRate(); }
		void SetTargetFrameRate(int16_t targetFrameRate) { return inter::HF.time.SetTargetFrameRate(targetFrameRate); }

		TimeUniformInfo GetUniformInfo()
		{
			return
			{
				.deltaTime = inter::HF.time.deltaTime,
				.timeSinceStartup = inter::HF.time.GetTimePassed(),
			};
		}

		double GetSystemTime()
		{
			using namespace std::chrono;
			const auto time = high_resolution_clock::now().time_since_epoch();
			return duration<double>(time).count();
		}
	}
}