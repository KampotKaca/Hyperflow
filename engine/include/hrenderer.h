#ifndef HRENDERER_H
#define HRENDERER_H

#include "hshared.h"
#include "../config.h"

namespace hf
{
	struct Renderer
	{
		Renderer(const Window* window, const RendererEventInfo& eventInfo);
		~Renderer();

		struct ThreadInfo
		{
			std::jthread thread;
			std::mutex threadlock{};
			bool wasModified = false;
			uvec2 size{};
		};

		uvec2 size{};
		const Window* window{};
		void* handle{};
		void* vertBufferCache[MAX_NUM_BUFFER_CACHE]{};
		RendererEventInfo eventInfo{};
	};
}

#endif //HRENDERER_H
