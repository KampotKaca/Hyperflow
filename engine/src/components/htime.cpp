#define private public
#include "components/htime.h"
#undef private

#include <chrono>
#include <thread>
#include "hplatform.h"
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

	namespace time
	{
		uint64_t GetFrameCount() { return inter::HF.time.GetFrameCount(); }
		double GetDeltaTime(){ return inter::HF.time.GetDeltaTime(); }
		double GetTimePassed() { return inter::HF.time.GetTimePassed(); }
		double GetAbsoluteTimePassed() { return inter::HF.time.GetAbsoluteTimePassed(); }
		int16_t GetTargetFrameRate() { return inter::HF.time.GetTargetFrameRate(); }
		int32_t GetFrameRate() { return inter::HF.time.GetFrameRate(); }
		void SetTargetFrameRate(int16_t targetFrameRate) { return inter::HF.time.SetTargetFrameRate(targetFrameRate); }

		double GetSystemTime()
		{
			using namespace std::chrono;
			const auto time = high_resolution_clock::now().time_since_epoch();
			return duration<double>(time).count();
		}
	}
}