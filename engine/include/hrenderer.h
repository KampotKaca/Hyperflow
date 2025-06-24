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

		void Upload_Buffer(const BufferUploadInfo& info);
		void Upload_Material(const Ref<Material>& material);

		void UploadStart_TexturePack(const Ref<TexturePack>& texPack);
		void UploadEnd_TexturePack();

		template<typename T>
		void UploadAdd_TexturePackBinding(const TexturePackBindingUploadInfo<T>& info);

		void Start_RenderPass(RenderPass pass);
		void End_RenderPass();

		void Start_ShaderSetup(ShaderSetup shaderSetup);
		void End_ShaderSetup();

		void Start_Shader(const ShaderBindingInfo& shaderBindingInfo);
		void End_Shader();

		void Start_Material(const Ref<Material>& material);
		void End_Material();

		void Start_Draw();
		void End_Draw();

		void Start_BufferSet(RenderBindingType bindingType, uint32_t setBindingIndex);
		void End_BufferSet();
		void BufferSetAdd_Buffer(Buffer buffer);

		void MaterialAdd_TexturePackBinding(const Ref<TexturePack>& texPack, uint32_t setBindingIndex);

		void DrawAdd_DrawCall(const Ref<Mesh>& mesh);
		void DrawAdd_DrawCall(const DrawCallInfo& drawCall);

		void DrawAdd_TexturePackBinding(const Ref<TexturePack>& texPack, uint32_t setBindingIndex);
		void DrawSet_PushConstant(const void* data, uint32_t dataSize);

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
