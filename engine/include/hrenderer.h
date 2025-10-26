#ifndef HRENDERER_H
#define HRENDERER_H

#include "hdrawprocess.h"
#include "hshared.h"

namespace hf
{
	struct Renderer
	{
		explicit Renderer(const Window* window, const RendererEventInfo& eventInfo);
		~Renderer();

		struct ThreadInfo
		{
		    ThreadMemoryStatistics memoryStatistics{};
			std::thread thread;
			std::mutex threadLock{};
			std::mutex statLock{};

			std::condition_variable renderCondition;
			uvec2 size{};

			bool isRunning{};
			RenderPacket* drawPacket{};
			List<RenderPacket*> cachedPackets{};
		};

	    RenderPacket allPackets[3]{};
	    RenderPacketDrawProcess currentDraw{};
	    RendererStatistics lastDrawStatistics{};

		bool isDrawing = false;
		ThreadInfo threadInfo{};
		const Window* window{};
		void* handle{};
		RendererEventInfo eventInfo{};
	};
}

#endif //HRENDERER_H
