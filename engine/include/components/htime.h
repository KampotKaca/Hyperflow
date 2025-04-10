#ifndef HTIME_H
#define HTIME_H

#include "hshared.h"

namespace hf
{
	struct Time
	{
		Time();
		~Time();

		void StartFrame();

		[[nodiscard]] double GetTimePassed() const;
		[[nodiscard]] double GetAbsoluteTimePassed() const;
		[[nodiscard]] int32_t GetFrameRate() const;

		void SetTargetFrameRate(int16_t targetFrameRate);

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
