#ifndef HTIME_H
#define HTIME_H

#include "hshared.h"

namespace hf
{
	class Time
	{
	public:
		Time();
		~Time();

		void Update();

		uint64_t GetFrameCount() const;
		double GetDeltaTime() const;
		double GetTimePassed() const;
		double GetAbsoluteTimePassed() const;
		int16_t GetTargetFrameRate() const;
		int32_t GetFrameRate() const;

		void SetTargetFrameRate(int16_t targetFrameRate);

	private:
		int16_t targetFrameRate = 50;
		double targetFrameDuration = (1.0 / targetFrameRate);
		double creationTime = 0;
		double currentTime = 0;
		double deltaTime = targetFrameDuration;
		uint64_t frameCount = 0;
		double frameRate = targetFrameRate;
	};
}

#endif //HTIME_H
