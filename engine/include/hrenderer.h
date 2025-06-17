#ifndef HRENDERER_H
#define HRENDERER_H

#include "hshared.h"
#include "hdrawprocess.h"

namespace hf
{
	struct Renderer
	{
		Renderer(const Window* window, const RendererEventInfo& eventInfo);
		~Renderer();

		[[nodiscard]] bool IsRunning() const;
		[[nodiscard]] uvec2 GetSize() const;
		void Resize(uvec2 size);

		//Destroy every renderer which is not connected to the window, before you try to change api
		static void ChangeApi(RenderingApiType targetApi);
		static RenderingApiType GetApiType();
		static RenderingApiType GetBestApiType();
		static bool IsValidApi(RenderingApiType targetApi);

		void Bind(RenderPass pass) const;

#ifndef HF_ENGINE_INTERNALS
	private:
#endif
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

		RenderPass mainPass{};
		RenderPacketDrawProcess currentDraw{};
		bool isDrawing = false;
		ThreadInfo threadInfo{};
		const Window* window{};
		void* handle{};
		RendererEventInfo eventInfo{};
	};
}

#endif //HRENDERER_H
