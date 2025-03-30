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

		[[nodiscard]] uint64_t GetFrameCount() const;
		[[nodiscard]] double GetDeltaTime() const;
		[[nodiscard]] double GetTimePassed() const;
		[[nodiscard]] double GetAbsoluteTimePassed() const;
		[[nodiscard]] int16_t GetTargetFrameRate() const;
		[[nodiscard]] int32_t GetFrameRate() const;

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
