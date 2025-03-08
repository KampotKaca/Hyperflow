#ifndef HSHARED_H
#define HSHARED_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdarg>
#include <vector>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <bitset>
#include <queue>
#include <optional>

extern "C"
{
#include "log.h"
};

#include <sys/stat.h>
#include <sys/types.h>

#include <cinttypes>
#include <memory>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include "glm/mat4x4.hpp"

namespace hf
{
	//region Aditionalls

	typedef void (*ActionCallback)(void* params);

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename... Args>
	Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	//endregion

	struct Rect
	{
		glm::vec2 position;
		glm::vec2 size;
	};

	struct IRect
	{
		glm::ivec2 position;
		glm::ivec2 size;
	};

	//endregion

	//region Window

	enum class WindowStyle
	{
		Default
	};

	enum class WindowFlags
	{
		Visible   = 1 << 0,
		Minimized = 1 << 1,
		Maximized = 1 << 2,

		Default = Visible
	};

	struct WindowData
	{
		std::string title;
		WindowFlags flags;
		WindowStyle style;
		glm::ivec2 position;
		glm::ivec2 size;
	};

	//endregion

	//region Engine

	enum class EngineUpdateType
	{
		Continues,   //Updates as often as is allowed, good for games or simulations, try to avoid it if you it's possible
		EventRaised  //Waits updates only on OS events, good for apps which doesn't change without user interaction
	};

	struct EngineLifecycleCallbacks
	{
		void (*onStartCallback)();
		void (*onUpdateCallback)();
		void (*onRenderCallback)();
		void (*onQuitCallback)();
	};

	struct EngineData
	{
		std::string appTitle;
		EngineUpdateType updateType;
		EngineLifecycleCallbacks lifecycleCallbacks;
		WindowData windowData;
	};

	class Window;

	//endregion

	//region Helpers

	uint32_t TrailingZeros64(uint64_t n);
	uint32_t TrailingZeros128(__int128 n);

	//endregion
}

#endif //HSHARED_H
