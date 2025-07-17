#ifndef HYPERFLOW_H
#define HYPERFLOW_H

#include "../components/include/hcomponents.h"
#include "hshared.h"

namespace hf
{
	void Preload();
	void Shutdown();

	void Run(const EngineData& engineData);
	Ref<Window> GetMainWindow();
	const std::string& GetApplicationTitle();
	void Terminate();

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
	Ref<RenderTexture> Create(const RenderTextureCreationInfo& info);
	Ref<TexturePack> Create(const TexturePackCreationInfo& info);
	Ref<TexturePackAllocator> Create(const TexturePackAllocatorCreationInfo& info);
	Ref<VertBuffer> Create(const VertBufferCreationInfo& info);
	Ref<Cubemap> Create(const CubemapCreationInfo& info);
	Ref<IndexBuffer> Create(const IndexBufferCreationInfo& info);
	Ref<AudioClip> Create(const AudioClipCreationInfo& info);
	Ref<AudioPlayer> Create(const AudioPlayerCreationInfo& info);
	Ref<AudioPlayer3D> Create(const AudioPlayer3DCreationInfo& info);

	void Destroy(const Ref<Mesh>& mesh);
	void Destroy(const Ref<Shader>& shader);
	void Destroy(const Ref<Texture>& tex);
	void Destroy(const Ref<RenderTexture>& tex);
	void Destroy(const Ref<TexturePack>& texPack);
	void Destroy(const Ref<TexturePackAllocator>& tpa);
	void Destroy(const Ref<RuntimeBufferBase>& rbb);
	void Destroy(const Ref<Cubemap>& cb);
	void Destroy(const Ref<Window>& win);
	void Destroy(const Ref<AudioClip>& clip);
	void Destroy(const Ref<AudioPlayer>& player);
	void Destroy(const Ref<AudioPlayer3D>& player);

	void DestroyAllWindows();
	void Destroy(const Ref<Cubemap>* pCubemaps, uint32_t count);
	void Destroy(const Ref<RuntimeBufferBase>* pBuffers, uint32_t count);
	void Destroy(const Ref<TexturePackAllocator>* pTexPackAllocators, uint32_t count);
	void Destroy(const Ref<TexturePack>* pPacks, uint32_t count);
	void Destroy(const Ref<Texture>* pTextures, uint32_t count);
	void Destroy(const Ref<RenderTexture>* pTextures, uint32_t count);
	void Destroy(const Ref<Mesh>* pMeshes, uint32_t count);
	void Destroy(const Ref<Shader>* pShaders, uint32_t count);
	void Destroy(const Ref<AudioClip>* pClips, uint32_t count);
	void Destroy(const Ref<AudioPlayer>* pPlayers, uint32_t count);
	void Destroy(const Ref<AudioPlayer3D>* pPlayers, uint32_t count);

	Ref<Mesh> CreateMeshAsset(const char* assetPath);
	Ref<Texture> CreateTextureAsset(const char* assetPath);
	Ref<Cubemap> CreateCubemapAsset(const char* assetPath);

	bool IsRunning();
	bool IsLoaded(const Ref<Renderer>& rn);
	bool IsLoaded(const Ref<Mesh>& mesh);
	bool IsLoaded(const Ref<Shader>& shader);
	bool IsLoaded(const Ref<Texture>& tex);
	bool IsLoaded(const Ref<RenderTexture>& tex);
	bool IsLoaded(const Ref<TexturePack>& texPack);
	bool IsLoaded(const Ref<TexturePackAllocator>& tpa);
	bool IsLoaded(const Ref<RuntimeBufferBase>& rbb);
	bool IsLoaded(const Ref<Cubemap>& cb);
	bool IsLoaded(const Ref<AudioClip>& clip);
	bool IsLoaded(const Ref<AudioPlayer>& player);
	bool IsLoaded(const Ref<AudioPlayer3D>& player);

	void Upload(const Ref<Material>& mat, const void* data);
	void Upload(const Ref<VertBuffer>& vb, const VertBufferUploadInfo& info);
	void Upload(const Ref<IndexBuffer>& ib, const IndexBufferUploadInfo& info);

	void ChangeClip(const Ref<AudioPlayer>& player, const Ref<AudioClip>& clip, double_t startingDuration = -1);
	void ChangeClip(const Ref<AudioPlayer3D>& player, const Ref<AudioClip>& clip, double_t startingDuration = -1);

	void Play(const Ref<AudioPlayer>& player);
	void Play(const Ref<AudioPlayer3D>& player);

	void Pause(const Ref<AudioPlayer>& player);
	void Pause(const Ref<AudioPlayer3D>& player);

	void SetVolume(const Ref<AudioPlayer>& player, float_t volume);
	void SetVolume(const Ref<AudioPlayer3D>& player, float_t volume);

	void SetPitch(const Ref<AudioPlayer>& player, float_t pitch);
	void SetPitch(const Ref<AudioPlayer3D>& player, float_t pitch);

	void SetLoopingMode(const Ref<AudioPlayer>& player, bool loopingEnabled);
	void SetLoopingMode(const Ref<AudioPlayer3D>& player, bool loopingEnabled);

	void Seek(const Ref<AudioPlayer>& player, double_t positionInSeconds);
	void Seek(const Ref<AudioPlayer3D>& player, double_t positionInSeconds);

	void SeekPercent(const Ref<AudioPlayer>& player, double_t position);
	void SeekPercent(const Ref<AudioPlayer3D>& player, double_t position);

	float_t GetPitch(const Ref<AudioPlayer>& player);
	float_t GetPitch(const Ref<AudioPlayer3D>& player);

	float_t GetVolume(const Ref<AudioPlayer>& player);
	float_t GetVolume(const Ref<AudioPlayer3D>& player);

    double_t GetPlayedInSeconds(const Ref<AudioPlayer>& player);
    double_t GetPlayedInSeconds(const Ref<AudioPlayer3D>& player);

    double_t GetPlayedPercent(const Ref<AudioPlayer>& player);
    double_t GetPlayedPercent(const Ref<AudioPlayer3D>& player);

	bool IsLoopingEnabled(const Ref<AudioPlayer>& player);
	bool IsLoopingEnabled(const Ref<AudioPlayer3D>& player);

    bool IsPlaying(const Ref<AudioPlayer>& player);
    bool IsPlaying(const Ref<AudioPlayer3D>& player);

    void SetMinDistance(const Ref<AudioPlayer3D>& player, float_t minDistance);
    void SetMaxDistance(const Ref<AudioPlayer3D>& player, float_t maxDistance);
    void SetAttenuationModel(const Ref<AudioPlayer3D>& player, Audio3DAttenuationModel atten);
    void SetPosition(const Ref<AudioPlayer3D>& player, vec3 position);
    void SetDirection(const Ref<AudioPlayer3D>& player, vec3 direction);

    float_t GetMinDistance(const Ref<AudioPlayer3D>& player);
    float_t GetMaxDistance(const Ref<AudioPlayer3D>& player);
    Audio3DAttenuationModel GetAttenuationModel(const Ref<AudioPlayer3D>& player);
    vec3 GetPosition(const Ref<AudioPlayer3D>& player);
    vec3 GetDirection(const Ref<AudioPlayer3D>& player);

	uint64_t GetFrameCount(const Ref<AudioClip>& clip);
	uint32_t GetChannels(const Ref<AudioClip>& clip);
	uint32_t GetSampleRate(const Ref<AudioClip>& clip);

	//Destroy every renderer which is not connected to the window, before you try to change api
	void ChangeApi(RenderingApiType targetApi);
	RenderingApiType GetApiType();
	RenderingApiType GetBestApiType();
	bool IsValidApi(RenderingApiType targetApi);
	uvec2 GetSize(const Ref<Renderer>& rn);
	void Resize(const Ref<Renderer>& rn, uvec2 size);

    //region Draw Process
    void Set_DrawCallback(const Ref<Renderer>& rn, void (*callback)(const Ref<Renderer>&, void*));
    void Upload_Buffer(const Ref<Renderer>& rn, const BufferUploadInfo& info);
	void Upload_Material(const Ref<Renderer>& rn, const Ref<Material>& material);

	void UploadStart_TexturePack(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack);
	void UploadEnd_TexturePack(const Ref<Renderer>& rn);

    void UploadAdd_TexturePackBinding(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<Texture>& info);
    void UploadAdd_TexturePackBinding(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<Cubemap>& info);
    void UploadAdd_TexturePackBinding(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<RenderTexture>& info);

    void Add_RenderTextureDependency(const Ref<Renderer>& rn, const RenderAttachmentDependencyInfo& info);

    void Start_RenderTexture(const Ref<Renderer>& rn, const Ref<RenderTexture>& rt);
    void End_RenderTexture(const Ref<Renderer>& rn);

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

	template<typename T>
	void DrawSet_PushConstant(const Ref<Renderer>& rn, const T& data)
	{
		DrawSet_PushConstant(rn, &data, sizeof(T));
	}
    //endregion

	uint16_t GetBufferIndex(const Ref<Material>& mat);

	MeshStats GetStats(const Ref<Mesh>& mesh);

	void SubmitAllTextures();

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

    void* GetEditorApiHandles();

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
		Ref<Mesh> GetPlane();
		Ref<Mesh> GetSphere();

		Ref<Material> GetEmptyMaterial();
	}
}

#endif //HYPERFLOW_H
