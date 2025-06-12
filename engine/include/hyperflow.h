#ifndef HYPERFLOW_H
#define HYPERFLOW_H

#include "hshared.h"
#include "hexception.h"
#include "hgenericexception.h"
#include "../components/include/hcomponents.h"

namespace hf
{
	void Preload();
	void Shutdown();

	void Run(const EngineData& engineData);
	bool IsRunning();
	Ref<Window> GetMainWindow();
	const std::string& GetApplicationTitle();
	void Terminate();

	namespace input
	{
		bool IsDown(const Ref<Window> &window, Key key);
		bool IsDownContinues(const Ref<Window> &window, Key key);
		bool IsUp(const Ref<Window> &window, Key key);

		bool IsDown(const Ref<Window> &window, Button button);
		bool IsDownContinues(const Ref<Window> &window, Button button);
		bool IsUp(const Ref<Window> &window, Button button);

		bool IsDown(Key key);
		bool IsDownContinues(Key key);
		bool IsUp(Key key);

		bool IsDown(Button button);
		bool IsDownContinues(Button button);
		bool IsUp(Button button);

		KeyState GetState(Key key);
		ButtonState GetState(Button button);
		const std::string& GetWrite();

		ivec2 GetPointerPosition();
		ivec2 GetPointerDelta();
		vec2 GetScrollDelta();

		KeyState GetState(const Ref<Window> &window, Key key);
		ButtonState GetState(const Ref<Window> &window, Button button);
		const std::string& GetWrite(const Ref<Window> &window);

		ivec2 GetPointerPosition(const Ref<Window> &window);
		ivec2 GetPointerDelta(const Ref<Window> &window);
		vec2 GetScrollDelta(const Ref<Window> &window);
	}

	namespace window
	{
		Ref<Window> Open(const WindowCreationInfo& data, const Ref<Window>& parent);
		void Close(const Ref<Window> &window);
		void CloseAll();

		const std::string& GetTitle(const Ref<Window> &window);
		ivec2 GetSize(const Ref<Window> &window);
		ivec2 GetPosition(const Ref<Window> &window);
		IRect GetRect(const Ref<Window> &window);
		IRect GetFrameRect(const Ref<Window> &window);
		WindowState GetState(const Ref<Window> &window);
		WindowStyle GetStyle(const Ref<Window> &window);
		WindowPointerState GetPointerState(const Ref<Window> &window);
		void* GetHandle(const Ref<Window> &window);
		Ref<Renderer> GetRenderer(const Ref<Window> &window);
		bool IsClosed(const Ref<Window> &window);
		VsyncMode GetVSyncMode(const Ref<Window> &window);

		void SetTitle(const Ref<Window> &window, const std::string& title);
		void SetSize(const Ref<Window> &window, ivec2 size);
		void SetPosition(const Ref<Window> &window, ivec2 position);
		void SetRect(const Ref<Window> &window, IRect rect);
		void SetState(const Ref<Window> &window, WindowState state);
		void SetPointerState(const Ref<Window> &window, WindowPointerState state);

		void Focus(const Ref<Window> &window);
		void SetVSyncMode(const Ref<Window> &window, VsyncMode mode);
		bool SetIcons(const Ref<Window>& window, const char* folderPath);
	}

	namespace time
	{
		uint64_t GetFrameCount();
		double GetDeltaTime();
		double GetTimePassed();
		double GetAbsoluteTimePassed();
		int16_t GetTargetFrameRate();
		double GetSystemTime();
		int32_t GetFrameRate();
		void SetTargetFrameRate(int16_t targetFrameRate);
	}

	namespace renderer
	{
		bool IsRunning(const Ref<Renderer>& rn);

		//Destroy every renderer which is not connected to the window, before you try to change api
		void ChangeApi(RenderingApiType targetApi);
		RenderingApiType GetApiType();
		RenderingApiType GetBestApiType();
		bool IsValidApi(RenderingApiType targetApi);
		uvec2 GetSize(const Ref<Renderer>& rn);

		void Resize(const Ref<Renderer>& rn, uvec2 size);
	}

	namespace renderpass
	{
		RenderPass Define(const RenderPassDefinitionInfo& info);
		void Bind(const Ref<Renderer>& rn, RenderPass pass);
	}

	namespace shader
	{
		Ref<Shader> Create(const ShaderCreationInfo& info);
		void Destroy(const Ref<Shader>& shader);
		void Destroy(const Ref<Shader>* pShaders, uint32_t count);
		bool IsRunning(const Ref<Shader>& shader);
	}

	namespace shadersetup
	{
		ShaderSetup Define(const ShaderSetupDefinitionInfo& info);
	}

	namespace texture
	{
		Ref<Texture> Create(const TextureCreationInfo& info);
		Ref<Texture> Create(const char* assetPath);
		void Destroy(const Ref<Texture>& texture);
		void Destroy(const Ref<Texture>* pTextures, uint32_t count);
		bool IsRunning(const Ref<Texture>& texture);

		void SubmitAll();
	}

	namespace cubemap
	{
		Ref<Cubemap> Create(const CubemapCreationInfo& info);
		Ref<Cubemap> Create(const char* assetPath);
		void Destroy(const Ref<Cubemap>& cubemap);
		void Destroy(const Ref<Cubemap>* pCubemaps, uint32_t count);
	}

	namespace texturepack
	{
		Ref<TexturePack> Create(const TexturePackCreationInfo& info);
		void Destroy(const Ref<TexturePack>& pack);
		void Destroy(const Ref<TexturePack>* pPacks, uint32_t count);
		bool IsRunning(const Ref<TexturePack>& pack);

		void SetBindingSampler(const Ref<TexturePack>& pack, uint32_t bindingIndex, TextureSampler sampler);
		void SetBindingTextures(const Ref<TexturePack>& pack, uint32_t bindingIndex, const Ref<Texture>* pTextures, uint32_t textureCount, uint32_t textureOffset);
		void SetBinding(const Ref<TexturePack>& pack, uint32_t bindingIndex, TextureSampler sampler, const Ref<Texture>* pTextures, uint32_t textureCount, uint32_t textureOffset);
	}

	namespace texturepackallocator
	{
		Ref<TexturePackAllocator> Create(const TexturePackAllocatorCreationInfo& info);
		void Destroy(const Ref<TexturePackAllocator>& texPackAllocator);
		void Destroy(const Ref<TexturePackAllocator>* pTexPackAllocators, uint32_t count);
		bool IsRunning(const Ref<TexturePackAllocator>& texPackAllocator);
	}

	namespace texturesampler
	{
		TextureSampler Define(const TextureSamplerDefinitionInfo& info);
	}

	namespace texturelayout
	{
		TextureLayout Define(const TextureLayoutDefinitionInfo& info);
	}

	namespace bufferattrib
	{
		BufferAttrib Define(const BufferAttribDefinitionInfo& info);
		BufferAttrib Define(const char* assetPath);
	}

	namespace uniformbuffer
	{
		UniformBuffer Define(const UniformBufferDefinitionInfo& info);
	}

	namespace uniformallocator
	{
		UniformAllocator Define(const UniformAllocatorDefinitionInfo& info);
	}

	namespace buffer
	{
		void SubmitAll();
	}

	namespace vertbuffer
	{
		Ref<VertBuffer> Create(const VertBufferCreationInfo& info);
		void Destroy(const Ref<VertBuffer>& buffer);
		void Destroy(const Ref<VertBuffer>* pBuffers, uint32_t count);
		bool IsRunning(const Ref<VertBuffer>& buffer);
		void Upload(const VertBufferUploadInfo& info);
	}

	namespace indexbuffer
	{
		Ref<IndexBuffer> Create(const IndexBufferCreationInfo& info);
		void Destroy(const Ref<IndexBuffer>& buffer);
		void Destroy(const Ref<IndexBuffer>* pBuffers, uint32_t count);
		bool IsRunning(const Ref<IndexBuffer>& buffer);
		void Upload(const IndexBufferUploadInfo& info);
	}

	namespace storagebuffer
	{
		Ref<StorageBuffer> Create(const StorageBufferCreationInfo& info);
		void Destroy(const Ref<StorageBuffer>& buffer);
		void Destroy(const Ref<StorageBuffer>* pBuffers, uint32_t count);
		bool IsRunning(const Ref<StorageBuffer>& buffer);
		void Upload(const StorageBufferUploadInfo& info);
	}

	namespace mesh
	{
		Ref<Mesh> Create(const MeshCreationInfo& info);
		Ref<Mesh> Create(const char* assetPath);
		void Destroy(const Ref<Mesh>& mesh);
		void Destroy(const Ref<Mesh>* pMeshes, uint32_t count);
		bool IsRunning(const Ref<Mesh>& mesh);
		MeshStats GetStats(const Ref<Mesh>& mesh);
	}

	namespace material
	{
		//No need to destroy the material, if it goes out of scope it is automatically freed!
		Ref<Material> Create(const MaterialCreationInfo& info);
		void Upload(const Ref<Material>& material, const void* data);
		uint16_t GetBufferIndex(const Ref<Material>& material);
	}

	namespace draw
	{
		void UploadUniformPacket(const Ref<Renderer>& rn, const UniformBufferUpload& info);

		void StartRenderPassPacket(const Ref<Renderer>& rn, RenderPass pass);
		void EndRenderPassPacket(const Ref<Renderer>& rn);

		void StartShaderSetupPacket(const Ref<Renderer>& rn, ShaderSetup shaderSetup);
		void EndShaderSetupPacket(const Ref<Renderer>& rn);

		void StartShaderPacket(const Ref<Renderer>& rn, const ShaderBindingInfo& shaderBindingInfo);
		void EndShaderPacket(const Ref<Renderer>& rn);

		void StartMaterialPacket(const Ref<Renderer>& rn, const Ref<Material>& material);
		void EndMaterialPacket(const Ref<Renderer>& rn);

		void StartDrawPacket(const Ref<Renderer>& rn);
		void EndDrawPacket(const Ref<Renderer>& rn);

		void ShaderSetupAdd_UniformBinding(const Ref<Renderer>& rn, const UniformBufferBindInfo& uniformBinding);
		void MaterialAdd_TexturePackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack, uint32_t setBindingIndex);

		void PacketAdd_DrawCall(const Ref<Renderer>& rn, const Ref<Mesh>& mesh);
		void PacketAdd_DrawCall(const Ref<Renderer>& rn, const DrawCallInfo& drawCall);
		void PacketAdd_TexturePackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack, uint32_t setBindingIndex);
		void PacketSet_PushConstant(const Ref<Renderer>& rn, const void* data, uint32_t dataSize);
	}

	namespace utils
	{
		ivec3 ConvertVersion(const char* version);
		bool ReadFile(const std::string& filename, bool addNullTerminator, std::vector<char>& result);
		bool FileExists(const char* path);

		[[nodiscard]] void* Allocate(std::size_t n);
		[[nodiscard]] void* AllocateAligned(std::size_t n, std::align_val_t align);
		void Deallocate(void* p);
		void DeallocateAligned(void* p, std::align_val_t align);

		void ReadTextureDetails(void* yamlTree, void* yamlRoot, TextureDetails& result);
	}

	namespace primitives
	{
		BufferAttrib GetQuadBufferAttrib();
		TextureLayout GetEmptyTextureLayout();

		BufferAttrib GetCubeBufferAttrib();
		TextureSampler GetCubemapSampler();
		UniformBuffer GetCameraUniform();

		Ref<Mesh> GetCube();
		Ref<Material> GetEmptyMaterial();
	}
}

#endif //HYPERFLOW_H
