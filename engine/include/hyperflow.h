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
	template <BS::opt_t OptFlags = BS::tp::none>
	using ThreadPool = BS::thread_pool<OptFlags>;

	void Preload();
	void Shutdown();

	void Run(const EngineData& engineData);
	bool IsRunning();
	Ref<Window> GetMainWindow();
	const std::string& GetApplicationTitle();
	void Terminate();

	RenderPass DefineRenderPass(const RenderPassDefinitionInfo& info);
	ShaderSetup DefineShaderSetup(const ShaderSetupDefinitionInfo& info);
	TextureSampler DefineTextureSampler(const TextureSamplerDefinitionInfo& info);
	TextureLayout DefineTextureLayout(const TextureLayoutDefinitionInfo& info);

	BufferAttrib DefineBufferAttrib(const BufferAttribDefinitionInfo& info);
	BufferAttrib DefineBufferAttrib(const char* assetPath);
	UniformBuffer DefineUniformBuffer(const UniformBufferDefinitionInfo& info);
	UniformAllocator DefineUniformAllocator(const UniformAllocatorDefinitionInfo& info);

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

	namespace material
	{
		//No need to destroy the material, if it goes out of scope it is automatically freed!
		Ref<Material> Create(const MaterialCreationInfo& info);
		void Upload(const Ref<Material>& material, const void* data);
		uint16_t GetBufferIndex(const Ref<Material>& material);
	}

	namespace skybox
	{
		void BindDefaultCubemap(const Ref<Renderer>& rn);
		void BindCubemap(const Ref<Renderer>& rn, const Ref<Cubemap>& cubemap);
		void Draw(const Ref<Renderer>& rn, const SkyboxInfo& info);
		[[nodiscard]] bool IsDefaultCubemapBound();
	}

	namespace gridlines
	{
		void Draw(const Ref<Renderer>& rn, const GridLinesInfo& info);
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
