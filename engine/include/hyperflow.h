#ifndef HYPERFLOW_H
#define HYPERFLOW_H

#include "hshared.h"
#include "hexception.h"
#include "hgenericexception.h"
#include "../components/include/hcomponents.h"

namespace hf
{
	template <BS::opt_t OptFlags = BS::tp::none>
	using ThreadPool = BS::thread_pool<OptFlags>;

	void Preload();
	void Shutdown();

	void Run(const EngineData& engineData);
	Ref<Window> GetMainWindow();
	const std::string& GetApplicationTitle();
	void Terminate();

	RenderPass DefineRenderPass(const RenderPassDefinitionInfo& info);
	ShaderSetup DefineShaderSetup(const ShaderSetupDefinitionInfo& info);
	TextureSampler DefineTextureSampler(const TextureSamplerDefinitionInfo& info);
	TextureLayout DefineTextureLayout(const TextureLayoutDefinitionInfo& info);

	BufferAttrib DefineBufferAttrib(const BufferAttribDefinitionInfo& info);
	BufferAttrib DefineBufferAttrib(const char* assetPath);
	Buffer DefineUniformBuffer(const BufferDefinitionInfo& info);
	Buffer DefineStorageBuffer(const StorageBufferDefinitionInfo& info);
	BufferAllocator DefineBufferAllocator(const BufferAllocatorDefinitionInfo& info);

	Ref<Window> Create(const WindowCreationInfo &data, const Ref<Window> &parent);
	//No need to destroy the material, if it goes out of scope it is automatically freed!
	Ref<Material> Create(const MaterialCreationInfo& info);
	Ref<Mesh> Create(const MeshCreationInfo& info);
	Ref<Shader> Create(const ShaderCreationInfo& info);
	Ref<Texture> Create(const TextureCreationInfo& info);
	Ref<TexturePack> Create(const TexturePackCreationInfo& info);
	Ref<TexturePackAllocator> Create(const TexturePackAllocatorCreationInfo& info);
	Ref<VertBuffer> Create(const VertBufferCreationInfo& info);
	Ref<Cubemap> Create(const CubemapCreationInfo& info);
	Ref<IndexBuffer> Create(const IndexBufferCreationInfo& info);

	void Destroy(const Ref<Mesh>& mesh);
	void Destroy(const Ref<Shader>& shader);
	void Destroy(const Ref<Texture>& tex);
	void Destroy(const Ref<TexturePack>& texPack);
	void Destroy(const Ref<TexturePackAllocator>& tpa);
	void Destroy(const Ref<RuntimeBufferBase>& rbb);
	void Destroy(const Ref<Cubemap>& cb);
	void Destroy(const Ref<Window>& win);

	void DestroyAllWindows();
	void Destroy(const Ref<Cubemap>* pCubemaps, uint32_t count);
	void Destroy(const Ref<RuntimeBufferBase>* pBuffers, uint32_t count);
	void Destroy(const Ref<TexturePackAllocator>* pTexPackAllocators, uint32_t count);
	void Destroy(const Ref<TexturePack>* pPacks, uint32_t count);
	void Destroy(const Ref<Texture>* pTextures, uint32_t count);
	void Destroy(const Ref<Mesh>* pMeshes, uint32_t count);
	void Destroy(const Ref<Shader>* pShaders, uint32_t count);

	Ref<Mesh> CreateMeshAsset(const char* assetPath);
	Ref<Texture> CreateTextureAsset(const char* assetPath);
	Ref<Cubemap> CreateCubemapAsset(const char* assetPath);

	bool IsRunning();
	bool IsRunning(const Ref<Renderer>& rn);
	bool IsRunning(const Ref<Mesh>& mesh);
	bool IsRunning(const Ref<Shader>& shader);
	bool IsRunning(const Ref<Texture>& tex);
	bool IsRunning(const Ref<TexturePack>& texPack);
	bool IsRunning(const Ref<TexturePackAllocator>& tpa);
	bool IsRunning(const Ref<RuntimeBufferBase>& rbb);
	bool IsRunning(const Ref<Cubemap>& cb);

	void Upload(const Ref<Material>& mat, const void* data);
	void Upload(const Ref<VertBuffer>& vb, const VertBufferUploadInfo& info);
	void Upload(const Ref<IndexBuffer>& ib, const IndexBufferUploadInfo& info);

	//Destroy every renderer which is not connected to the window, before you try to change api
	void ChangeApi(RenderingApiType targetApi);
	RenderingApiType GetApiType();
	RenderingApiType GetBestApiType();
	bool IsValidApi(RenderingApiType targetApi);
	uvec2 GetSize(const Ref<Renderer>& rn);
	void Resize(const Ref<Renderer>& rn, uvec2 size);
	void Bind(const Ref<Renderer>& rn, RenderPass pass);

	void Upload_Buffer(const Ref<Renderer>& rn, const BufferUploadInfo& info);
	void Upload_Material(const Ref<Renderer>& rn, const Ref<Material>& material);

	void UploadStart_TexturePack(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack);
	void UploadEnd_TexturePack(const Ref<Renderer>& rn);

	template<typename T>
	void UploadAdd_TexturePackBinding(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<T>& info);

	void Start_RenderPass(const Ref<Renderer>& rn, RenderPass pass);
	void End_RenderPass(const Ref<Renderer>& rn);

	void Start_ShaderSetup(const Ref<Renderer>& rn, ShaderSetup shaderSetup);
	void End_ShaderSetup(const Ref<Renderer>& rn);

	void Start_Shader(const Ref<Renderer>& rn, const ShaderBindingInfo& shaderBindingInfo);
	void End_Shader(const Ref<Renderer>& rn);

	void Start_Material(const Ref<Renderer>& rn, const Ref<Material>& material);
	void End_Material(const Ref<Renderer>& rn);

	void Start_Draw(const Ref<Renderer>& rn);
	void End_Draw(const Ref<Renderer>& rn);

	void Start_BufferSet(const Ref<Renderer>& rn, RenderBindingType bindingType, uint32_t setBindingIndex);
	void End_BufferSet(const Ref<Renderer>& rn);
	void BufferSetAdd_Buffer(const Ref<Renderer>& rn, Buffer buffer);

	void MaterialAdd_TexturePackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack, uint32_t setBindingIndex);

	void DrawAdd_DrawCall(const Ref<Renderer>& rn, const Ref<Mesh>& mesh);
	void DrawAdd_DrawCall(const Ref<Renderer>& rn, const DrawCallInfo& drawCall);

	void DrawAdd_TexturePackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack, uint32_t setBindingIndex);
	void DrawSet_PushConstant(const Ref<Renderer>& rn, const void* data, uint32_t dataSize);

	uint16_t GetBufferIndex(const Ref<Material>& mat);

	MeshStats GetStats(const Ref<Mesh>& mesh);

	void SubmitAllTexturePacks();

	void SubmitAllBuffers();

	bool IsKeyDown(Key key);
	bool IsKeyDown(const Ref<Window>& win, Key key);
	bool IsKeyDownContinues(Key key);
	bool IsKeyDownContinues(const Ref<Window>& win, Key key);
	bool IsKeyUp(Key key);
	bool IsKeyUp(const Ref<Window>& win, Key key);

	bool IsButtonDown(Button button);
	bool IsButtonDown(const Ref<Window>& win, Button button);
	bool IsButtonDownContinues(Button button);
	bool IsButtonDownContinues(const Ref<Window>& win, Button button);
	bool IsButtonUp(Button button);
	bool IsButtonUp(const Ref<Window>& win, Button button);

	KeyState GetKeyState(Key key);
	KeyState GetKeyState(const Ref<Window>& win, Key key);
	ButtonState GetButtonState(Button button);
	ButtonState GetButtonState(const Ref<Window>& win, Button button);
	const std::string& GetWrite();
	const std::string& GetWrite(const Ref<Window>& win);

	vec2 GetPointerPosition();
	vec2 GetPointerPosition(const Ref<Window>& win);
	vec2 GetPointerDelta();
	vec2 GetPointerDelta(const Ref<Window>& win);
	vec2 GetScrollDelta();
	vec2 GetScrollDelta(const Ref<Window>& win);

	const std::string& GetTitle(const Ref<Window>& win);
	ivec2 GetSize(const Ref<Window>& win);
	ivec2 GetPosition(const Ref<Window>& win);
	IRect GetRect(const Ref<Window>& win);
	IRect GetFrameRect(const Ref<Window>& win);
	WindowState GetState(const Ref<Window>& win);
	WindowStyle GetStyle(const Ref<Window>& win);
	WindowPointerState GetPointerState(const Ref<Window>& win);
	void* GetHandle(const Ref<Window>& win);
	Ref<Renderer> GetRenderer(const Ref<Window>& win);
	bool IsClosed(const Ref<Window>& win);
	VsyncMode GetVSyncMode(const Ref<Window>& win);

	void SetTitle(const Ref<Window>& win, const std::string& title);
	void SetSize(const Ref<Window>& win, ivec2 size);
	void SetPosition(const Ref<Window>& win, ivec2 position);
	void SetRect(const Ref<Window>& win, IRect rect);
	void SetState(const Ref<Window>& win, WindowState state);
	void SetPointerState(const Ref<Window>& win, WindowPointerState state);

	void Focus(const Ref<Window>& win);
	void SetVSyncMode(const Ref<Window>& win, VsyncMode mode);
	bool SetIcons(const Ref<Window>& win, const char* folderPath);

	uint64_t GetFrameCount();
	double GetDeltaTime();
	double GetTimePassed();
	double GetAbsoluteTimePassed();
	int16_t GetTargetFrameRate();
	double GetSystemTime();
	int32_t GetFrameRate();
	void SetTargetFrameRate(int16_t targetFrameRate);
	TimeUniformInfo GetTimeUniformInfo();

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
		Ref<Mesh> GetCubeMesh();
		Ref<VertBuffer> GetQuadBuffer();
		Ref<VertBuffer>* GetQuadBufferP();

		BufferAttrib GetQuadBufferAttrib();
		Buffer GetGlobalUniformBuffer();
		Buffer GetMaterialStorageBuffer();
		void BindGlobalUniformBuffer(const Ref<Renderer>& rn);

		BufferAttrib GetCubeBufferAttrib();
		TextureSampler GetCubemapSampler();

		Ref<Mesh> GetCube();
		Ref<Material> GetEmptyMaterial();
	}
}

#endif //HYPERFLOW_H
