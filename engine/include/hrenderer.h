#ifndef HRENDERER_H
#define HRENDERER_H

#include "hshared.h"
#include "../config.h"
#include "hdrawprocess.h"
#include "htime.h"

namespace hf
{
	struct Renderer
	{
		Renderer(const Window* window, const RendererEventInfo& eventInfo);
		~Renderer();

		struct ThreadInfo
		{
			std::jthread thread;
			std::mutex threadLock{};
			std::condition_variable renderCondition;
			uvec2 size{};

			bool packetIsReady{};
			RenderPacket drawPacket{};
		};

		RenderPacketDrawProcess currentDraw{};
		bool isDrawing = false;
		ThreadInfo threadInfo{};
		const Window* window{};
		void* handle{};
		RendererEventInfo eventInfo{};
	};
}

#endif //HRENDERER_H
