#ifndef HYPERFLOW_H
#define HYPERFLOW_H

#include "../components/include/hcomponents.h"
#include "hshared.h"
#include "hscene.h"

namespace hf
{
    //region Engine

    void Preload();
    void Shutdown();

	bool IsRunning();

    void Run(const EngineData& engineData);
    Ref<Window> GetMainWindow();
    const std::string& GetApplicationTitle();

    Ref<AudioGroup> Get2DAudioGroup();
    Ref<AudioGroup> Get3DAudioGroup();

    void SetAudioVolume(float_t volume);
    float_t GetAudioVolume();

    //Destroy every renderer which is not connected to the window, before you try to change api
    void ChangeApi(RenderingApiType targetApi);
    RenderingApiType GetApiType();
    RenderingApiType GetBestApiType();
    bool IsValidApi(RenderingApiType targetApi);

    void SubmitAllTextures();
    void SubmitAllBuffers();

    bool IsKeyDown(Key key);
    bool IsKeyDownContinues(Key key);
    bool IsKeyUp(Key key);

    bool IsButtonDown(Button button);
    bool IsButtonDownContinues(Button button);
    bool IsButtonUp(Button button);

    KeyState GetKeyState(Key key);
    ButtonState GetButtonState(Button button);
    const std::string& GetWrite();

    vec2 GetPointerPosition();
    vec2 GetPointerDelta();
    vec2 GetScrollDelta();

    uint64_t GetFrameCount();
    double GetDeltaTime();
    double GetTimePassed();
    double GetAbsoluteTimePassed();
    int16_t GetTargetFrameRate();
    double GetSystemTime();
    int32_t GetFrameRate();
    void SetTargetFrameRate(int16_t targetFrameRate);

    void* GetEditorApiHandles();

    void Terminate();
    void UnloadAllScenes();
    void DestroyAllWindows();

    Ref<void> CreateAsset(const char* assetPath, AssetType type);
    void DestroyAsset(const char* assetPath);
    Ref<void> GetAsset(const char* assetPath);

    //endregion
    //region Search

    VertexBufferAttribute FindVertexAttribute(const char* id);
    TextureLayout FindTextureLayout(const char* id);
    TextureSampler FindTextureSampler(const char* id);

    //endregion
    //region Window

	Ref<Window> Create(const WindowCreationInfo &data, const Ref<Window> &parent);
	void Destroy(const Ref<Window>& win);

    bool IsKeyDown(const Ref<Window>& win, Key key);
	bool IsKeyDownContinues(const Ref<Window>& win, Key key);
	bool IsKeyUp(const Ref<Window>& win, Key key);

	bool IsButtonDown(const Ref<Window>& win, Button button);
	bool IsButtonDownContinues(const Ref<Window>& win, Button button);
	bool IsButtonUp(const Ref<Window>& win, Button button);

    KeyState GetKeyState(const Ref<Window>& win, Key key);
    ButtonState GetButtonState(const Ref<Window>& win, Button button);
    const std::string& GetWrite(const Ref<Window>& win);

    vec2 GetPointerPosition(const Ref<Window>& win);
    vec2 GetPointerDelta(const Ref<Window>& win);
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

    //endregion
    //region Material

    //No need to destroy the material, if it goes out of scope it is automatically freed!
    Ref<Material> Create(const MaterialCreationInfo& info);
    void Upload(const Ref<Material>& mat, const void* data);
	uint16_t GetBufferIndex(const Ref<Material>& mat);

    //endregion
    //region Shader Library

	Ref<ShaderLibrary> Create(const ShaderLibraryCreationInfo& info);
	void Destroy(const Ref<ShaderLibrary>& lib);
	void Destroy(const Ref<ShaderLibrary>* pLibraries, uint32_t count);
	bool IsLoaded(const Ref<ShaderLibrary>& lib);

    //endregion
    //region Shader

	Ref<Shader> Create(const ShaderCreationInfo& info);
	void Destroy(const Ref<Shader>& shader);
	void Destroy(const Ref<Shader>* pShaders, uint32_t count);
	bool IsLoaded(const Ref<Shader>& shader);

    //endregion
    //region RenderTexture

	Ref<RenderTexture> Create(const RenderTextureCreationInfo& info);
	void Destroy(const Ref<RenderTexture>& tex);
	void Destroy(const Ref<RenderTexture>* pTextures, uint32_t count);
	bool IsLoaded(const Ref<RenderTexture>& tex);

    //endregion
    //region Texture Pack

	Ref<TexturePack> Create(const TexturePackCreationInfo& info);
	void Destroy(const Ref<TexturePack>& texPack);
	void Destroy(const Ref<TexturePack>* pPacks, uint32_t count);
	bool IsLoaded(const Ref<TexturePack>& texPack);

    //endregion
    //region Buffers

	Ref<VertexBuffer> Create(const VertexBufferCreationInfo& info);
	Ref<IndexBuffer> Create(const IndexBufferCreationInfo& info);
	void Destroy(const Ref<RuntimeBufferBase>& rbb);
	void Destroy(const Ref<RuntimeBufferBase>* pBuffers, uint32_t count);
	bool IsLoaded(const Ref<RuntimeBufferBase>& rbb);

    //endregion
    //region Defines

	ShaderLayout Define(const ShaderLayoutDefinitionInfo& info);
    TextureSampler Define(const TextureSamplerDefinitionInfo& info);
    TextureLayout Define(const TextureLayoutDefinitionInfo& info);
    VertexBufferAttribute Define(const VertexBufferAttributeDefinitionInfo& info);
    Buffer Define(const BufferDefinitionInfo& info);
    Buffer Define(const StorageBufferDefinitionInfo& info);

    VertexBufferAttribute DefineVertexAttributeAsset(const char* assetPath);
    TextureLayout DefineTextureLayoutAsset(const char* assetPath);
    TextureSampler DefineTextureSamplerAsset(const char* assetPath);

    uint32_t GetVertexSize(VertexBufferAttribute attribute);

    //endregion
    //region Audio Clip

	Ref<AudioClip> Create(const AudioClipCreationInfo& info);
	void Destroy(const Ref<AudioClip>& clip);
	void Destroy(const Ref<AudioClip>* pClips, uint32_t count);
	bool IsLoaded(const Ref<AudioClip>& clip);

    uint64_t GetFrameCount(const Ref<AudioClip>& clip);
    uint32_t GetChannels(const Ref<AudioClip>& clip);
    uint32_t GetSampleRate(const Ref<AudioClip>& clip);

    //endregion
    //region Audio Player

	Ref<AudioPlayer> Create(const AudioPlayerCreationInfo& info);
	void Destroy(const Ref<AudioPlayer>& player);
	void Destroy(const Ref<AudioPlayer>* pPlayers, uint32_t count);
	bool IsLoaded(const Ref<AudioPlayer>& player);

    void ChangeClip(const Ref<AudioPlayer>& player, const Ref<AudioClip>& clip, float_t startingDuration = -1);
	void Play(const Ref<AudioPlayer>& player);
	void Pause(const Ref<AudioPlayer>& player);
	void SetVolume(const Ref<AudioPlayer>& player, float_t volume);
	void SetPitch(const Ref<AudioPlayer>& player, float_t pitch);
	void SetLoopingMode(const Ref<AudioPlayer>& player, bool loopingEnabled);
	void Seek(const Ref<AudioPlayer>& player, float_t positionInSeconds);
	void SeekPercent(const Ref<AudioPlayer>& player, float_t position);
	float_t GetPitch(const Ref<AudioPlayer>& player);
	float_t GetVolume(const Ref<AudioPlayer>& player);
    double_t GetPlayedInSeconds(const Ref<AudioPlayer>& player);
    double_t GetPlayedPercent(const Ref<AudioPlayer>& player);
	bool IsLoopingEnabled(const Ref<AudioPlayer>& player);
    bool IsPlaying(const Ref<AudioPlayer>& player);

    //endregion
    //region Audio Player 3D

	Ref<AudioPlayer3D> Create(const AudioPlayer3DCreationInfo& info);
    void Destroy(const Ref<AudioPlayer3D>& player);
    void Destroy(const Ref<AudioPlayer3D>* pPlayers, uint32_t count);
	bool IsLoaded(const Ref<AudioPlayer3D>& player);

    void ChangeClip(const Ref<AudioPlayer3D>& player, const Ref<AudioClip>& clip, float_t startingDuration = -1);
    void Play(const Ref<AudioPlayer3D>& player);
	void Pause(const Ref<AudioPlayer3D>& player);
	void SetVolume(const Ref<AudioPlayer3D>& player, float_t volume);
	void SetPitch(const Ref<AudioPlayer3D>& player, float_t pitch);
	void SetLoopingMode(const Ref<AudioPlayer3D>& player, bool loopingEnabled);
	void Seek(const Ref<AudioPlayer3D>& player, float_t positionInSeconds);
	void SeekPercent(const Ref<AudioPlayer3D>& player, float_t position);
	float_t GetPitch(const Ref<AudioPlayer3D>& player);
	float_t GetVolume(const Ref<AudioPlayer3D>& player);
    double_t GetPlayedInSeconds(const Ref<AudioPlayer3D>& player);
    double_t GetPlayedPercent(const Ref<AudioPlayer3D>& player);
    bool IsLoopingEnabled(const Ref<AudioPlayer3D>& player);
    bool IsPlaying(const Ref<AudioPlayer3D>& player);
    void SetRange(const Ref<AudioPlayer3D>& player, float_t maxRange, float_t falloff);
    void SetAttenuationModel(const Ref<AudioPlayer3D>& player, Audio3DAttenuationModel atten);
    void SetVelocity(const Ref<AudioPlayer3D>& pl, vec3 velocity);
    void Set(const Ref<AudioPlayer3D>& pl, const AudioCone& cone);
    vec3 GetVelocity(const Ref<AudioPlayer3D>& pl);
    AudioCone GetCone(const Ref<AudioPlayer3D>& pl);
    //X -> near, Y -> far
    vec2 GetRange(const Ref<AudioPlayer3D>& player);
    Audio3DAttenuationModel GetAttenuationModel(const Ref<AudioPlayer3D>& player);

    //endregion
    //region Audio Listener

    //Audio listener cannot be destroyed, it is fully attached to the audio engine.
    //if you set to have certain number of listeners, you must create them.
    //this object just gives you access to the listener, in reality it was already created.
    Ref<AudioListener> Create(const AudioListenerCreationInfo& info);

    void Enable(const Ref<AudioListener>& ls, bool enable);
    void SetVelocity(const Ref<AudioListener>& ls, vec3 velocity);
    void Set(const Ref<AudioListener>& ls, const AudioCone& cone);
    bool IsEnabled(const Ref<AudioListener>& ls);
    vec3 GetVelocity(const Ref<AudioListener>& ls);
    AudioCone GetCone(const Ref<AudioListener>& ls);

    //endregion
    //region Audio Group

    Ref<AudioGroup> Create(const AudioGroupCreationInfo& info);
	bool IsLoaded(const Ref<AudioGroup>& group);

    void Enable(const Ref<AudioGroup>& group, bool enable);
    void SetVolume(const Ref<AudioGroup>& group, float_t volume);
    void SetPitch(const Ref<AudioGroup>& group, float_t pitch);
    float_t GetPitch(const Ref<AudioGroup>& group);
    float_t GetVolume(const Ref<AudioGroup>& group);
    bool IsEnabled(const Ref<AudioGroup>& group);

    //endregion
    //region Renderer

	bool IsLoaded(const Ref<Renderer>& rn);
    uvec2 GetSize(const Ref<Renderer>& rn);
    void Resize(const Ref<Renderer>& rn, uvec2 size);
    ThreadMemoryStatistics GetMemoryStatistics(const Ref<Renderer>& rn);

    void Upload(const Ref<Renderer>& rn, const Ref<VertexBuffer>& vb, const VertBufferUploadInfo& info); //Renderer is optional!
    void Upload(const Ref<Renderer>& rn, const Ref<IndexBuffer>& ib, const IndexBufferUploadInfo& info); //Renderer is optional!

    //endregion
    //region Scene

    void LoadSceneBase(const Ref<Scene>& scene);
    void UnloadScene(const Ref<Scene>& scene);

    template<typename T, typename... Args>
    Ref<T> LoadScene(Args&&... args)
    {
        auto scene = MakeRef<T>(std::forward<Args>(args)...);
        LoadSceneBase(scene);
        return scene;
    }

    //endregion
    //region Animation Player

    void Play(const Ref<AnimationPlayer>& player);
    void Pause(const Ref<AnimationPlayer>& player);
    void Seek(const Ref<AnimationPlayer>& player, float_t positionInSeconds);
    void SeekPercent(const Ref<AnimationPlayer>& player, float_t position);
    double_t GetPlayedInSeconds(const Ref<AnimationPlayer>& player);
    double_t GetPlayedPercent(const Ref<AnimationPlayer>& player);
    bool IsPlaying(const Ref<AnimationPlayer>& player);
    Ref<Animation> GetAnim(const Ref<AnimationPlayer>& player);
    void ChangeAnim(const Ref<AnimationPlayer>& player, const Ref<Animation>& anim, float_t startingDuration = -1);

    //endregion
    //region Model

	bool IsLoaded(const Ref<Model>& model);
    uint32_t GetMeshCount(const Ref<Model>& model);
    Ref<Mesh> GetMesh(const Ref<Model>& model, uint32_t index);

    //endregion
    //region Mesh

    Ref<Mesh> Create(const MeshCreationInfo& info);
    void Destroy(const Ref<Mesh>& mesh);
    void Destroy(const Ref<Mesh>* pMeshes, uint32_t count);
    bool IsLoaded(const Ref<Mesh>& mesh);

    MeshStats GetStats(const Ref<Mesh>& mesh);
    uint32_t GetMeshSkinCount(const Ref<Mesh>& mesh);
    const BoundingVolume& GetMeshBoundingVolume(const Ref<Mesh>& mesh);

    //endregion
    //region Texture

	bool IsLoaded(const Ref<Texture>& tex);

    //endregion
    //region Cubemap

	bool IsLoaded(const Ref<Cubemap>& cb);

    //endregion

    //Drawing process functions.
    //These functions should never be called outside draw process.
    namespace dp
    {
        void SetDrawCallback(const Ref<Renderer>& rn, void (*callback)(const Ref<Renderer>&, void*));

        void SetCamera(const Ref<Renderer>& rn, const Camera3DAnchored& camera);
        void SetCamera(const Ref<Renderer>& rn, const Camera3DFreeLook& camera);

        void AddLight(const Ref<Renderer>& rn, const DirectionalLight& light);
        void AddLight(const Ref<Renderer>& rn, const SpotLight& light);
        void AddLight(const Ref<Renderer>& rn, const PointLight& light);

        void UploadBuffer(const Ref<Renderer>& rn, const BufferUploadInfo& info);
	    void UploadMat(const Ref<Renderer>& rn, const Ref<Material>& mat);

	    void UploadStartTexPack(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack);
	    void UploadEndTexPack(const Ref<Renderer>& rn);

        void UploadAddTexPackBinding(const Ref<Renderer>& rn, uint32_t bindingIndex, const Ref<Texture>& texture, uint32_t textureIndex = 0);
        void UploadAddTexPackBinding(const Ref<Renderer>& rn, uint32_t bindingIndex, const Ref<Cubemap>& cubemap, uint32_t textureIndex = 0);
        void UploadAddTexPackBinding(const Ref<Renderer>& rn, uint32_t bindingIndex, const Ref<RenderTexture>& rt, uint32_t attachmentIndex);

        void UploadAddTexPackBindings(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<Texture>& info);
        void UploadAddTexPackBindings(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<Cubemap>& info);
        void UploadAddTexPackBindings(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<RenderTexture>& info);

        void AddRenderTexDependency(const Ref<Renderer>& rn, const RenderAttachmentDependencyInfo& info);

        void StartRenderTex(const Ref<Renderer>& rn, const Ref<RenderTexture>& rt);
        void EndRenderTex(const Ref<Renderer>& rn);

	    void StartShaderLayout(const Ref<Renderer>& rn, ShaderLayout layout);
	    void EndShaderLayout(const Ref<Renderer>& rn);

	    void StartShader(const Ref<Renderer>& rn, const Ref<Shader>& shader);
	    void EndShader(const Ref<Renderer>& rn);

        //material can be null, when you do not use it in the shader!
	    void StartMat(const Ref<Renderer>& rn, const Ref<Material>& material);
	    void EndMat(const Ref<Renderer>& rn);

	    void StartDrawGroup(const Ref<Renderer>& rn);
	    void EndDrawGroup(const Ref<Renderer>& rn);

	    void StartBufferSet(const Ref<Renderer>& rn, RenderBindingType bindingType, uint32_t setBindingIndex);
	    void EndBufferSet(const Ref<Renderer>& rn);
	    void BufferSetAddBuffer(const Ref<Renderer>& rn, Buffer buffer);

	    void MatAddTexPackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack, uint32_t setBindingIndex);

	    void StartDrawCall(const Ref<Renderer>& rn, const Ref<IndexBuffer>& indexBuffer);
        void StartDrawCall(const Ref<Renderer>& rn, const Ref<Mesh>& mesh);
        void StartDrawCall(const Ref<Renderer>& rn, const Ref<Mesh>& mesh, const Ref<Armature>& armature, uint32_t skinIndex);
        void EndDrawCall(const Ref<Renderer>& rn);

	    void DrawGroupAddTexPackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack, uint32_t setBindingIndex);
        void DrawGroupAddDrawCall(const Ref<Renderer>& rn, const Ref<IndexBuffer>& indexBuffer, const Ref<VertexBuffer>& vertexBuffer);
        void DrawGroupAddDrawCall(const Ref<Renderer>& rn, const Ref<Mesh>& mesh);

	    void DrawGroupSetPushConstant(const Ref<Renderer>& rn, const void* data, uint32_t dataSize);
	    void DrawAddInstance(const Ref<Renderer>& rn, const void* data, uint32_t dataSize, const VolumeTransform& volume);
	    void DrawAddVertBuffer(const Ref<Renderer>& rn, const Ref<VertexBuffer>& vb);

	    template<typename T>
	    void DrawGroupSetPushConstant(const Ref<Renderer>& rn, const T& data)
	    {
	    	DrawGroupSetPushConstant(rn, &data, sizeof(T));
	    }

        template<typename T>
        void DrawAddInstance(const Ref<Renderer>& rn, const T& data, const VolumeTransform& volume)
	    {
	        DrawAddInstance(rn, &data, sizeof(T), volume);
	    }
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
		bool ReadFile(const std::string& filename, std::vector<char>& result);
		bool WriteFile(const std::string& filename, const std::vector<char>& result);
		bool FileExists(const char* path);

		[[nodiscard]] void* Allocate(std::size_t n);
		[[nodiscard]] void* AllocateAligned(std::size_t n, std::align_val_t align);
		void Deallocate(void* p);
		void DeallocateAligned(void* p, std::align_val_t align);
		void* Reallocate(void* p, std::size_t n);
		void CollectThreadMemoryCache(); //Reduces fragmentation.

	    GlobalMemoryStatistics GetGlobalMemoryStatistics();
	    ThreadMemoryStatistics GetThreadMemoryStatistics();
	    RendererStatistics GetRendererStatistics(const Ref<Renderer>& rn);

	    void ReadVertexInputModule   (const char* assetPath, ShaderLibraryVertexInputModuleInfo& result);
	    void ReadPreRasterModule     (const char* assetPath, ShaderLayout layout, ShaderLibraryPreRasterModuleInfo& result);
	    void ReadFragmentModule      (const char* assetPath, ShaderLayout layout, ShaderLibraryFragmentModuleInfo& result);
	    void ReadFragmentOutputModule(const char* assetPath, ShaderLibraryFragmentOutputModuleInfo& result);
	}
	namespace primitives
	{
	    Ref<Mesh> GetMesh(PrimitiveMeshType type);
        Ref<Texture> GetTexture(PrimitiveTextureType type);

		Ref<VertexBuffer> GetQuadBuffer();
		Ref<VertexBuffer>* GetQuadBufferP();

		Buffer GetGlobalUniformBuffer();
		void BindGlobalUniformBuffer(const Ref<Renderer>& rn);
	}
}

#endif //HYPERFLOW_H
