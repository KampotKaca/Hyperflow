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

	bool IsRunning(); //Check if the engine is running.

    void Run(const EngineData& engineData); //Run the engine.
    Ref<Window> GetMainWindow(); //Get main window of the application.
    const std::string& GetApplicationTitle(); //Get title of the application.

    Ref<AudioGroup> Get2DAudioGroup(); //Get the Audio Group which handles all the Audio Players.
    Ref<AudioGroup> Get3DAudioGroup(); //Get the Audio Group which handles all the Audio Player 3Ds.

    void SetAudioVolume(float_t volume); //Set the global audio volume.
    float_t GetAudioVolume(); //Get the global audio volume.

    RenderingApiType GetApiType(); //Get current rendering api.
    RenderingApiType GetBestApiType(); //Get best rendering api for the platform.
    bool IsValidApi(RenderingApiType targetApi); //Check if the api can be used on the current platform.

    //When textures are loaded this data needs to get to the gpu memory to be usable by the renderer.
    //Submitting textures one by one is possible, but it slows down the process,
    //so engine stores all your upload requests temporarily and is uploaded all at once when you call submit.
    //Calling this is not mandatory, since this will be called on the next frame, but this might slow down the frame,
    //since rendering cannot be started if all resources are not present.
    //So this function is recomended to be used after you load static textures.
    void SubmitAllTextures();
    //When meshes and other types of objects which require heap of memory are loaded this data needs to get to the gpu memory to be usable by the renderer.
    //Submitting buffers one by one is possible, but it slows down the process,
    //so engine stores all your upload requests temporarily and is uploaded all at once when you call submit.
    //Calling this is not mandatory, since this will be called on the next frame, but this might slow down the frame,
    //since rendering cannot be started if all resources are not present.
    //So this function is recomended to be used after you load meshes or other statis buffers.
    void SubmitAllBuffers();

    bool IsKeyDown(Key key); //Check if key was just pressed on main window.
    bool IsKeyDownContinues(Key key); //Check if key is being pressed across multiple frames on main window.
    bool IsKeyUp(Key key); //Check if key was just released on main window.

    bool IsButtonDown(Button button); //Check if button was just pressed on main window.
    bool IsButtonDownContinues(Button button); //Check if button is being pressed across multiple frames on main window.
    bool IsButtonUp(Button button); //Check if button was just released on main window.

    KeyState GetKeyState(Key key); //Get key state on main window.
    ButtonState GetButtonState(Button button); //Get button state on main window.
    const std::string& GetWrite(); //Get last frame's key writes on main window.

    vec2 GetPointerPosition(); //Get mouse position.
    vec2 GetPointerDelta(); //Get last frames mouse movement on main window.
    vec2 GetScrollDelta(); //Get last frames scroll on main window.

    uint64_t GetFrameCount(); //Get amount of frames since startup.
    double GetDeltaTime(); //Get time passed since last frame in seconds.
    double GetTimePassed(); //Get Scaled time passed since startup in seconds.
    double GetAbsoluteTimePassed(); //Get Unscaled time passed since startup in seconds.
    int16_t GetTargetFrameRate(); //Get target frame rate of the application.
    double GetSystemTime(); //Get system time.
    int32_t GetFrameRate(); //Get current frame rate of the application.
    void SetTargetFrameRate(int16_t targetFrameRate); //Set the target frame rate of the application.

    void* GetEditorApiHandles(); //This function is used to get necessary resources to load an editor.

    void Terminate(); //Terminate the application.
    void UnloadAllScenes(); //Unload all the scenes used by the application.
    void DestroyAllWindows(); //Destroy all the windows used by the application.

    //Load an asset from a meta file.
    //If the asset is already loaded it will not be loaded again, you will just get the Ref to it.
    //Asset api tracks the create and destroy calls, so if you call twice create and once destroy asset will not be destroyed.
    //.meta part is not necessary, this is assumed.
    Ref<void> CreateAsset(const char* assetPath, AssetType type);
    void DestroyAsset(const char* assetPath, AssetType type); //Destroy the loaded asset.
    void DestroyAsset(const std::string_view assetPath, AssetType type); //Destroy the loaded asset.
    Ref<void> GetAsset(const char* assetPath, AssetType type); //Get asset by the local path it was loaded from.
    Ref<void> GetAsset(const std::string_view assetPath, AssetType type); //Get asset by the local path it was loaded from.

    //endregion
    //region Search

    VertexBufferAttribute FindVertexAttribute(const char* id);
    VertexBufferAttribute FindVertexAttribute(std::string_view id);

    TextureLayout FindTextureLayout(const char* id);
    TextureLayout FindTextureLayout(std::string_view id);

    TextureSampler FindTextureSampler(const char* id);
    TextureSampler FindTextureSampler(std::string_view id);

    ShaderLayout FindShaderLayout(const char* id);
    ShaderLayout FindShaderLayout(std::string_view id);

    Buffer FindBuffer(const char* id);
    Buffer FindBuffer(std::string_view id);

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

    uint32_t GetVertexInputModule(const Ref<ShaderLibrary>& lib, const char* name);
    uint32_t GetVertexInputModule(const Ref<ShaderLibrary>& lib, const std::string_view name);
    uint32_t GetPreRasterModule(const Ref<ShaderLibrary>& lib, const char* name);
    uint32_t GetPreRasterModule(const Ref<ShaderLibrary>& lib, const std::string_view name);
    uint32_t GetFragmentModule(const Ref<ShaderLibrary>& lib, const char* name);
    uint32_t GetFragmentModule(const Ref<ShaderLibrary>& lib, const std::string_view name);
    uint32_t GetFragmentOutputModule(const Ref<ShaderLibrary>& lib, const char* name);
    uint32_t GetFragmentOutputModule(const Ref<ShaderLibrary>& lib, const std::string_view name);

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

	ShaderLayout Define(const char* name, const ShaderLayoutDefinitionInfo& info);
    TextureSampler Define(const TextureSamplerDefinitionInfo& info);
    TextureLayout Define(const TextureLayoutDefinitionInfo& info);
    VertexBufferAttribute Define(const VertexBufferAttributeDefinitionInfo& info);
    Buffer Define(const char* name, const BufferDefinitionInfo& info);
    Buffer Define(const char* name, const StorageBufferDefinitionInfo& info);

    VertexBufferAttribute DefineVertexAttributeAsset(const char* assetPath);
    TextureLayout DefineTextureLayoutAsset(const char* assetPath);
    TextureSampler DefineTextureSamplerAsset(const char* assetPath);

    uint32_t GetVertexSize(VertexBufferAttribute attribute);

    //endregion
    //region Audio Clip

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
    Ref<AnimationClip> GetAnim(const Ref<AnimationPlayer>& player);
    void ChangeAnim(const Ref<AnimationPlayer>& player, const Ref<AnimationClip>& anim, float_t startingDuration = -1);

    //endregion
    //region Animation

    void Destroy(const Ref<AnimationClip>& anim);
    void Destroy(const Ref<AnimationClip>* pAnimations, uint32_t count);
    bool IsLoaded(const Ref<AnimationClip>& anim);

    float_t GetDuration(const Ref<AnimationClip>& anim); //Duration of the animation clip
    uint16_t GetFrameRate(const Ref<AnimationClip>& anim); //Frame rate of the animation clip
    uint32_t GetFrameCount(const Ref<AnimationClip>& anim); //Frame count of the animation clip

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

    Ref<Texture> Create(const TextureCreationInfo& info);
    void Destroy(const Ref<Texture>& tex);
    void Destroy(const Ref<Texture>* pTextures, uint32_t count);
	bool IsLoaded(const Ref<Texture>& tex);

    //endregion
    //region Cubemap

    Ref<Cubemap> Create(const CubemapCreationInfo& info);
    void Destroy(const Ref<Cubemap>& cm);
    void Destroy(const Ref<Cubemap>* pCubemaps, uint32_t count);
	bool IsLoaded(const Ref<Cubemap>& cb);

    //endregion

    //Drawing process functions.
    //These functions should never be called outside draw process.
    namespace dp
    {
		void BindGlobalUniformBuffer(const Ref<Renderer>& rn); //Bind the global uniform buffer to the currently bound shader layout.
        void SetDrawCallback(const Ref<Renderer>& rn, void (*callback)(const Ref<Renderer>&, void*)); //This callback will be called from rendering thread after every draw, be careful, incorrect use might crash the program.

        void SetCamera(const Ref<Renderer>& rn, const Camera3DAnchored& camera); //Set the draw camera. Needs to be called early in drawing process.
        void SetCamera(const Ref<Renderer>& rn, const Camera3DFreeLook& camera); //Set the draw camera. Needs to be called early in drawing process.

        void AddLight(const Ref<Renderer>& rn, const DirectionalLight& light); //Add directional light to the renderer.
        void AddLight(const Ref<Renderer>& rn, const SpotLight& light);        //Add Spot light to the renderer.
        void AddLight(const Ref<Renderer>& rn, const PointLight& light);       //Add Point light to the renderer.

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

        //Draw the skybox.
        //Try to call this draw as late as possible to reduce pixel overdraw.
        //This function should be called after render texture binding, since it binds it's own shader layout.
        void Draw(const Ref<Renderer>& rn, const SkyboxInfo& info);
        //Draw xz line grid.
        //This function should be called after render texture binding, since it binds it's own shader layout.
        void Draw(const Ref<Renderer>& rn, const GridLinesInfo& info);

        void SkyboxBindDefaultCubemap(const Ref<Renderer>& rn); //Bind the default cubemap to the skybox.
        void SkyboxBindCubemap(const Ref<Renderer>& rn, const Ref<Cubemap>& cubemap); //Bind the cubemap to the skybox.

        [[nodiscard]] bool SkyboxIsDefaultCubemapBound(); //Check if default cubemap is bound to the skybox.
    }
	namespace utils
	{
		ivec3 ConvertVersion(const char* version); //Convert string version to an integer,
		bool ReadFile(const char* path, std::vector<char>& result); //Get the data of the file.
		bool ReadFile(const std::string& path, std::vector<char>& result); //Get the data of the file.
        bool ReadFile(const std::filesystem::path& path, std::vector<char>& result); //Get the data of the file.
        bool ReadFile(const std::string_view path, std::vector<char>& result); //Get the data of the file.
        bool WriteFile(const char* path, const std::vector<char>& data); //Write the data in the file.
        bool WriteFile(const std::string& path, const std::vector<char>& data); //Write the data in the file.
        bool WriteFile(const std::filesystem::path& path, const std::vector<char>& data); //Write the data in the file.
        bool WriteFile(const std::string_view path, const std::vector<char>& data); //Write the data in the file.
        bool FileExists(const char* path); //Check if file exists.
		bool FileExists(const std::filesystem::path& path); //Check if file exists.

		[[nodiscard]] void* Alloc(std::size_t n); //Allocate n amount of memory.
		[[nodiscard]] void* AllocAligned(std::size_t n, std::size_t align); //Allocate n amount of aligned memory.
		void Deallocate(void* p); //Deallocate the memory.
		void DeallocateAligned(void* p, std::size_t align); //Deallocate the aligned memory.
		void* Realloc(void* p, std::size_t n); //Reallocate the memory.
        void* ReallocAligned(void* p, std::size_t n, std::size_t align); //Reallocate the memory with alignment.
		void* Calloc(std::size_t n, std::size_t size); //Allocate and initialize n amount of memory.

        std::size_t AllocUsableSize(void* ptr);
		void CollectThreadMemoryCache(); //Reduces memory fragmentation.

	    GlobalMemoryStatistics GetGlobalMemoryStatistics(); //Get global memory statistics.
	    ThreadMemoryStatistics GetThreadMemoryStatistics(); //Get memory statistics from the current thread.
	    RendererStatistics GetRendererStatistics(const Ref<Renderer>& rn); //Get rendering thread memory statistics.
	}
	namespace primitives
	{
	    Ref<Mesh> GetMesh(PrimitiveMeshType type); //Get primitive mesh by type.
        Ref<Texture> GetTexture(PrimitiveTextureType type); //Get primitive texture by type.

		Ref<VertexBuffer> GetQuadBuffer(); //Get vertex buffer which contains vertices for a quad.
		Ref<VertexBuffer>* GetQuadBufferP(); //Get vertex buffer which contains vertices for a quad.

		Buffer GetGlobalUniformBuffer(); //Get global uniform buffer, this buffer store camera and light information of the rendered scene.
	}
}

#endif //HYPERFLOW_H
