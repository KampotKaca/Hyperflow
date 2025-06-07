#ifndef HRENDERER_H
#define HRENDERER_H

#include "hshared.h"
#include "../config.h"
#include "hdrawprocess.h"

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
			uvec2 size{};

			RenderPacket packetQueue[RENDERING_MAX_PACKET_QUEUE_SIZE];
			uint8_t packetCount{};
			RenderPacketDrawProcess currentDraw{};
			std::atomic_bool isDrawing = false;
		};

		ThreadInfo threadInfo{};
		const Window* window{};
		void* handle{};
		RendererEventInfo eventInfo{};
	};
}

#endif //HRENDERER_H
