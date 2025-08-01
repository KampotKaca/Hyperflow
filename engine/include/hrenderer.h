#ifndef HRENDERER_H
#define HRENDERER_H

#include "hshared.h"
#include "hdrawprocess.h"

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
			std::vector<RenderPacket*> cachedPackets{};
		};

	    RenderPacket allPackets[3]{};
	    RenderPacketDrawProcess currentDraw{};
		bool isDrawing = false;
		ThreadInfo threadInfo{};
		const Window* window{};
		void* handle{};
		RendererEventInfo eventInfo{};
	};
}

#endif //HRENDERER_H
