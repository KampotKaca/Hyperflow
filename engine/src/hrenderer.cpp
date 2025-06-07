#include "hrenderer.h"
#include <hyperflow.h>
#include "hinternal.h"
#include "../config.h"
#include "../../application/appconfig.h"
#include "../rendering/include/hex_renderer.h"

namespace hf
{
    Renderer::Renderer(const Window* window, const RendererEventInfo& eventInfo)
        : eventInfo(eventInfo)
    {
        this->window = window;
        size = inter::window::GetSize(window);
        inter::rendering::CreateRenderer_i(this);
    }

    Renderer::~Renderer()
    {
        inter::rendering::DestroyRenderer_i(this);
    }

    namespace renderer
    {
        bool IsRunning(const Ref<Renderer>& rn) { return rn->handle; }

        void ChangeApi(RenderingApiType targetApi)
        {
            if (!IsValidApi(targetApi))
            {
                LOG_ERROR("[Hyperflow] %s", "Invalid render Api to load");
                return;
            }
            inter::rendering::UnloadCurrentApi_i(true);
            inter::rendering::LoadApi_i(targetApi);
        }

        RenderingApiType GetApiType() { return inter::HF.renderingApi.type; }
        RenderingApiType GetBestApiType() { return inter::platform::GetBestRenderingApi(); }
        bool IsValidApi(RenderingApiType targetApi)
        {
            if (targetApi == RenderingApiType::None ||
                targetApi == inter::HF.renderingApi.type) return false;

            return inter::platform::IsValidRenderingApi(targetApi);
        }
        uvec2 GetSize(const Ref<Renderer>& rn) { return rn->size; }

        void Resize(const Ref<Renderer>& rn, uvec2 size)
        {
            if (rn->window != nullptr) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot resize renderer connected to the window");
            rn->size = size;
            inter::HF.renderingApi.api.RegisterFrameBufferChange(rn->handle, size);
        }
    }

    namespace inter::rendering
    {
        void LoadApi_i(RenderingApiType api)
        {
            if (HF.renderingApi.type != RenderingApiType::None) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot load multiple rendering APIs, need unload current one first");
            RenderingApi newApi{};
            switch (api)
            {
                case RenderingApiType::None: throw GENERIC_EXCEPT("[Hyperflow]", "Cannot run the engine without rendering");
                case RenderingApiType::Vulkan:
                    newApi.handle = platform::LoadDll("vk");
                    break;
                case RenderingApiType::Direct3D:
                    newApi.handle = platform::LoadDll("d3d");
                    break;
                default: break;
            }

            if (!newApi.handle) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot load rendering API");
            auto func = (RendererAPI*(*)())platform::GetFuncPtr(newApi.handle, "GetAPI");
            if (!func) throw GENERIC_EXCEPT("[Hyperflow]", "Unable to fund GetAPI function in the rendering dll");
            newApi.api = *func();
            newApi.type = api;
            HF.renderingApi = newApi;

            for (auto& win : HF.windows | std::views::values)
            {
                if (win->renderer) CreateRenderer_i(win->renderer.get());
                else win->renderer = MakeRef<Renderer>(win.get(), win->rnEventInfo);
                auto rn = win->renderer;
                HF.renderingApi.api.PostInstanceLoad(rn->handle, rn->eventInfo.onPassCreationCallback(rn));
            }

            DefineStaticResources_i();
            if (HF.lifecycleCallbacks.onRendererLoad) HF.lifecycleCallbacks.onRendererLoad();

            StorageBufferCreationInfo materialStorageInfo
            {
                .bindingId = 0,
                .elementSizeInBytes = RENDERING_MAX_MATERIAL_MEMORY_BADGET,
                .elementCount = 64 * 64 * RENDERING_MAX_MATERIAL_OCTREE_COUNT,
                .memoryType = BufferMemoryType::WriteOnly,
                .usageFlags = BufferUsageType::All,
                .data = nullptr
            };

            HF.graphicsResources.materialDataStorage = storagebuffer::Create(materialStorageInfo);

            for (auto& shader : std::ranges::views::values(HF.graphicsResources.shaders)) CreateShader_i(shader.get());

            for (auto& vertBuffer : std::ranges::views::values(HF.graphicsResources.vertBuffers)) CreateVertBuffer_i(vertBuffer.get());
            for (auto& indexBuffer : std::ranges::views::values(HF.graphicsResources.indexBuffers)) CreateIndexBuffer_i(indexBuffer.get());
            for (auto& storageBuffer : std::ranges::views::values(HF.graphicsResources.storageBuffers)) CreateStorageBuffer_i(storageBuffer.get());
            for (auto& mesh : std::ranges::views::values(HF.graphicsResources.meshes)) CreateMesh_i(mesh.get());
            buffer::SubmitAll();

            for (auto& texture : std::ranges::views::values(HF.graphicsResources.textures)) CreateTexture_i(texture.get());
            for (auto& cubemap : std::ranges::views::values(HF.graphicsResources.cubemaps)) CreateCubemap_i(cubemap.get());
            for (auto& texPack : std::ranges::views::values(HF.graphicsResources.texturePacks)) CreateTexturePack_i(texPack.get());
            texture::SubmitAll();

            for (auto& texPackAllocator : std::ranges::views::values(HF.graphicsResources.texturePackAllocators)) CreateTexturePackAllocator_i(texPackAllocator.get());
        }

        void UnloadCurrentApi_i(bool retainReferences)
        {
            if (HF.renderingApi.type == RenderingApiType::None) return;

            for (auto& window : HF.windows | std::views::values)
            {
                if (window->renderer)
                {
                    DestroyRenderer_i(window->renderer.get());
                    if (!retainReferences) window->renderer = nullptr;
                }
            }

            platform::UnloadDll(HF.renderingApi.handle);

            HF.renderingApi = RenderingApi
            {
                .type = RenderingApiType::None,
                .handle = nullptr,
            };

            HF.graphicsResources.bufferAttribs.clear();
        }

        void DefineStaticResources_i()
        {
            //Quat Attribute
            {
                BufferAttribFormat formats[]
                {
                    { .type = BufferDataType::I32, .size = 2, }
                };

                BufferAttribDefinitionInfo attribInfo
                {
                    .bindingId = 0,
                    .formatCount = 1,
                    .pFormats = formats
                };

                HF.staticResources.quadAttrib = bufferattrib::Define(attribInfo);
            }

            //Empty Texture Layout
            {
                TextureLayoutDefinitionInfo layoutInfo
                {
                    .pBindings = nullptr,
                    .bindingCount = 0
                };

                HF.staticResources.emptyLayout = texturelayout::Define(layoutInfo);
            }

            //Cubemap Sampler
            {
                TextureSamplerDefinitionInfo samplerInfo
                {
                    .filter = TextureFilter::Bilinear,
                    .anisotropicFilter = TextureAnisotropicFilter::X16,
                    .repeatMode = TextureRepeatMode::ClampToEdge,
                    .useNormalizedCoordinates = true,
                    .comparison = ComparisonOperation::Never,
                };

                HF.staticResources.cubemapSampler = hf::texturesampler::Define(samplerInfo);
            }

            //Cubemap Attrib
            {
                BufferAttribFormat formats[]
                {
                    { .type = BufferDataType::I32, .size = 3, }
                };

                BufferAttribDefinitionInfo attribInfo
                {
                    .bindingId = 0,
                    .formatCount = 1,
                    .pFormats = formats
                };

                HF.staticResources.cubeAttrib = bufferattrib::Define(attribInfo);
            }
        }

        void LoadStaticResources_i()
        {
            //Quad Vertices
            {
                ivec2 vertices[6]
                {
                    { -1, -1 }, { 1, 1 }, { -1, 1 },
                    { -1, -1 }, { 1, -1 }, { 1, 1 },
                };

                VertBufferCreationInfo bufferInfo
                {
                    .bufferAttrib = HF.staticResources.quadAttrib,
                    .memoryType = BufferMemoryType::Static,
                    .usageFlags = BufferUsageType::All,
                    .vertexCount = 6,
                    .pVertices = vertices
                };

                HF.staticResources.quadBuffer = vertbuffer::Create(bufferInfo);
            }

            //Cube Vertices
            {
                ivec3 vertices[8]
                {
                    { -1, -1, -1 }, { -1, -1, 1 }, { 1, -1, 1 }, { 1, -1, -1 },
                    { -1, 1, -1 }, { -1, 1, 1 }, { 1, 1, 1 }, { 1, 1, -1 },
                };

                VertBufferCreationInfo bufferInfo
                {
                    .bufferAttrib = HF.staticResources.cubeAttrib,
                    .memoryType = BufferMemoryType::Static,
                    .usageFlags = BufferUsageType::All,
                    .vertexCount = 8,
                    .pVertices = vertices
                };

                HF.staticResources.cubeVertices = vertbuffer::Create(bufferInfo);
            }

            //Cube Indices
            {
                uint8_t indices[36]
                {
                    0, 1, 2,   2, 3, 0,  //Bottom
                    4, 6, 5,   4, 7, 6,  //Top
                    1, 5, 6,   6, 2, 1,  //Front
                    0, 7, 3,   0, 4, 7,  //Back
                    0, 5, 1,   0, 4, 5,  //Left
                    3, 6, 2,   3, 7, 6   //Right
                };

                IndexBufferCreationInfo bufferInfo
                {
                    .indexFormat = BufferDataType::U8,
                    .memoryType = BufferMemoryType::Static,
                    .usageFlags = BufferUsageType::All,
                    .indexCount = 36,
                    .pIndices = indices
                };

                HF.staticResources.cubeIndices = indexbuffer::Create(bufferInfo);
            }
        }

        void CreateRenderer_i(Renderer* rn)
        {
            if (HF.rendererCount == 0)
            {
                auto engineV = utils::ConvertVersion(VERSION);
                auto appV = utils::ConvertVersion(APP_VERSION);

                RendererLoadInfo loadInfo
                {
                    .appVersion = appV,
                    .engineVersion = engineV,
                    .applicationTitle = HF.appTitle.c_str(),
                    .getFuncFromDll = platform::GetFuncPtr,
                };

                if (HF.renderingApi.type == RenderingApiType::Vulkan)
                {
                    HF.renderingApi.additionalDll = platform::LoadDll(VK_DLL);
                    loadInfo.platformDll = HF.renderingApi.additionalDll;
                    loadInfo.createVulkanSurfaceFunc = platform::CreateVulkanSurface;
                }

                HF.renderingApi.api.Load(loadInfo);
                HF.renderingApi.isLoaded = true;
            }
            HF.rendererCount++;
            RendererInstanceCreationInfo createInfo
            {
                .size = rn->size,
            };

            if (rn->window)
            {
                createInfo.vSyncMode = rn->window->vSyncMode;
                createInfo.handle = rn->window->handle;
            }

            rn->handle = HF.renderingApi.api.CreateInstance(createInfo);
        }

        void DestroyRenderer_i(Renderer* rn)
        {
            if (rn->handle)
            {
                if (HF.rendererCount == 1)
                {
                    UnloadAllResources_i(IsRunning());
                }

                HF.renderingApi.api.WaitForRendering();
                HF.renderingApi.api.DestroyInstance(rn->handle);
                rn->handle = nullptr;
                HF.rendererCount--;
                if (HF.rendererCount == 0)
                {
                    HF.renderingApi.isLoaded = false;
                    HF.renderingApi.api.Unload();
                    if (HF.renderingApi.additionalDll)
                    {
                        platform::UnloadDll(HF.renderingApi.additionalDll);
                        HF.renderingApi.additionalDll = nullptr;
                    }
                }
            }
        }

        void UnloadAllResources_i(bool internalOnly)
        {
            if (HF.renderingApi.isLoaded) HF.renderingApi.api.WaitForRendering();
            DestroyAllVertBuffers_i(internalOnly);
            DestroyAllIndexBuffers_i(internalOnly);
            DestroyAllStorageBuffers_i(internalOnly);
            DestroyAllMeshes_i(internalOnly);
            DestroyAllShaders_i(internalOnly);

            DestroyAllTextures_i(internalOnly);
            DestroyAllCubemaps_i(internalOnly);

            DestroyAllTexturePackAllocators_i(internalOnly);
            DestroyAllTexturePacks_i(internalOnly);
        }
    }

    namespace renderpass
    {
        RenderPass Define(const RenderPassDefinitionInfo& info)
        {
            return inter::HF.renderingApi.api.DefineRenderPass(info);
        }

        void Bind(const Ref<Renderer>& rn, RenderPass pass)
        {
            inter::HF.renderingApi.api.BindRenderPass(rn->handle, pass);
        }
    }
}