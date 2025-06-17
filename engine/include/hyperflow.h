#ifndef HYPERFLOW_H
#define HYPERFLOW_H

#include "hshared.h"
#include "hexception.h"
#include "hgenericexception.h"
#include "../components/include/hcomponents.h"

#include "hwindow.h"
#include "hrenderer.h"
#include "htexture.h"
#include "htexturepack.h"
#include "htexturepackallocator.h"
#include "hshader.h"
#include "hbuffer.h"
#include "hvertbuffer.h"
#include "hindexbuffer.h"
#include "hstoragebuffer.h"
#include "hmesh.h"

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
		bool IsDown(Key key);
		bool IsDownContinues(Key key);
		bool IsUp(Key key);

		bool IsDown(Button button);
		bool IsDownContinues(Button button);
		bool IsUp(Button button);

		KeyState GetState(Key key);
		ButtonState GetState(Button button);
		const std::string& GetWrite();

		vec2 GetPointerPosition();
		vec2 GetPointerDelta();
		vec2 GetScrollDelta();
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

	RenderPass DefineRenderPass(const RenderPassDefinitionInfo& info);
	ShaderSetup DefineShaderSetup(const ShaderSetupDefinitionInfo& info);
	TextureSampler DefineTextureSampler(const TextureSamplerDefinitionInfo& info);
	TextureLayout DefineTextureLayout(const TextureLayoutDefinitionInfo& info);

	BufferAttrib DefineBufferAttrib(const BufferAttribDefinitionInfo& info);
	BufferAttrib DefineBufferAttrib(const char* assetPath);
	UniformBuffer DefineUniformBuffer(const UniformBufferDefinitionInfo& info);
	UniformAllocator DefineUniformAllocator(const UniformAllocatorDefinitionInfo& info);

	namespace material
	{
		//No need to destroy the material, if it goes out of scope it is automatically freed!
		Ref<Material> Create(const MaterialCreationInfo& info);
		void Upload(const Ref<Material>& material, const void* data);
		uint16_t GetBufferIndex(const Ref<Material>& material);
	}

	namespace skybox
	{
		void BindDefaultCubemap();
		void BindCubemap(const Ref<Cubemap>& cubemap);
		void Draw(const Ref<Renderer>& rn, const SkyboxInfo& info);
		[[nodiscard]] bool IsDefaultCubemapBound();
	}

	namespace gridlines
	{
		void Draw(const Ref<Renderer>& rn, const GridLinesInfo& info);
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
