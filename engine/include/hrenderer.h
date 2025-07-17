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
			std::thread thread;
			std::mutex threadLock{};

			std::condition_variable renderCondition;
			uvec2 size{};

			bool packetIsReady{};
			bool isRunning{};
			RenderPacket drawPacket{};
		};

#if DEBUG
	    void (*debugDrawCallback)(const Ref<Renderer>&, void*);
#endif
	    RenderPacketDrawProcess currentDraw{};
		bool isDrawing = false;
		ThreadInfo threadInfo{};
		const Window* window{};
		void* handle{};
		RendererEventInfo eventInfo{};
	};
}

#endif //HRENDERER_H
