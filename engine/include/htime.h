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

		[[nodiscard]] double_t GetTimePassed() const;
		[[nodiscard]] double_t GetAbsoluteTimePassed() const;
		[[nodiscard]] int32_t GetFrameRate() const;

		void SetTargetFrameRate(int16_t targetFrameRate);
		TimeUniformInfo GetUniformInfo()const;

		int16_t targetFrameRate = 50;
		double_t targetFrameDuration = (1.0 / targetFrameRate);
		double_t creationTime = 0;
		double_t currentTime = 0;
		double_t deltaTime = targetFrameDuration;
		uint64_t frameCount = 0;
		double_t frameRate = targetFrameRate;
	};
}

#endif //HTIME_H
