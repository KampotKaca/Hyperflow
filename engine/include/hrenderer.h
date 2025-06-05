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
			bool wasModified = false;
			uvec2 size{};
		};

		uvec2 size{};
		const Window* window{};
		void* handle{};
		RendererEventInfo eventInfo{};

		RenderPacket packetQueue[RENDERING_MAX_PACKET_QUEUE_SIZE];
		uint8_t packetCount{};

		RenderPacketDrawProcess currentDraw{};
		bool isDrawing = false;
	};
}

#endif //HRENDERER_H
