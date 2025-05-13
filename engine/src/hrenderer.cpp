#include "hrenderer.h"
#include <hyperflow.h>
#include "hinternal.h"
#include "../config.h"
#include "../../application/appconfig.h"
#include "../rendering/include/hex_renderer.h"

namespace hf
{
    Renderer::Renderer(uvec2 initialSize)
    {
        size = initialSize;
        inter::rendering::CreateRenderer_i(this);
    }

    Renderer::Renderer(const Window* window)
    {
        this->window = window;
        size = window->rect.size;
        inter::rendering::CreateRenderer_i(this);
    }

    Renderer::~Renderer()
    {
        inter::rendering::DestroyRenderer_i(this);
    }

    namespace renderer
    {
        Ref<Renderer> Create(uvec2 size)
        {
            return MakeRef<Renderer>(size);
        }

        void Destroy(const Ref<Renderer>& rn)
        {
            inter::rendering::DestroyRenderer_i(rn.get());
        }

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

        void Draw(const Ref<Renderer>& rn, const DrawCallInfo& info)
        {
            if (info.bufferCount > MAX_NUM_BUFFER_CACHE)
                throw GENERIC_EXCEPT("[Hyperflow]", "Trying to draw too many buffers at once, max is %i", MAX_NUM_BUFFER_CACHE);

            for (uint32_t i = 0; i < info.bufferCount; i++)
                rn->vertBufferCache[i] = info.pVertBuffers[i]->handle;

            inter::rendering::DrawCallInfo drawInfo
            {
                .pVertBuffers = rn->vertBufferCache,
                .bufferCount = info.bufferCount,
                .indexBuffer = info.indexBuffer ? info.indexBuffer->handle : nullptr,
                .instanceCount = info.instanceCount
            };

            inter::HF.renderingApi.api.Draw(rn->handle, drawInfo);
        }

        void UnloadAllResources(bool internalOnly)
        {
            if (inter::HF.renderingApi.isLoaded) inter::HF.renderingApi.api.WaitForRendering();
            vertbuffer::DestroyAll(internalOnly);
            indexbuffer::DestroyAll(internalOnly);
            shader::DestroyAll(internalOnly);

            texture::DestroyAll(internalOnly);

            texturepackallocator::DestroyAll(internalOnly);
            texturepack::DestroyAll(internalOnly);
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

            for (auto& win : HF.windows)
            {
                if (win->renderer) CreateRenderer_i(win->renderer.get());
                else win->renderer = MakeRef<Renderer>(win.get());
                HF.renderingApi.api.PostInstanceLoad(win->renderer->handle, win->onPassCreationCallback());
            }

            if (HF.lifecycleCallbacks.onRendererLoad) HF.lifecycleCallbacks.onRendererLoad();

            for (auto& shader : std::ranges::views::values(HF.graphicsResources.shaders)) CreateShader_i(shader.get());
            for (auto& vertBuffer : std::ranges::views::values(HF.graphicsResources.vertBuffers)) CreateVertBuffer_i(vertBuffer.get());
            for (auto& indexBuffer : std::ranges::views::values(HF.graphicsResources.indexBuffers)) CreateIndexBuffer_i(indexBuffer.get());
            for (auto& texture : std::ranges::views::values(HF.graphicsResources.textures)) CreateTexture_i(texture.get());
            for (auto& texPack : std::ranges::views::values(HF.graphicsResources.texturePacks)) CreateTexturePack_i(texPack.get());
            texture::SubmitAll();

            for (auto& texPackAllocator : std::ranges::views::values(HF.graphicsResources.texturePackAllocators)) CreateTexturePackAllocator_i(texPackAllocator.get());
        }

        void UnloadCurrentApi_i(bool retainReferences)
        {
            if (HF.renderingApi.type == RenderingApiType::None) return;

            for (auto& window : HF.windows)
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
                    .platformInstance = platform::GetPlatformInstance(),
                    .getFuncFromDll = platform::GetFuncPtr,
                };

                if (HF.renderingApi.type == RenderingApiType::Vulkan)
                {
                    HF.renderingApi.additionalDll = platform::LoadDll(VK_DLL);
                    loadInfo.platformDll = HF.renderingApi.additionalDll;
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
                createInfo.vSyncOn = rn->window->vSyncIsOn;
                createInfo.handle = window::GetWindowHandle(rn->window);
            }

            rn->handle = HF.renderingApi.api.CreateInstance(createInfo);
        }

        void DestroyRenderer_i(Renderer* rn)
        {
            if (rn->handle)
            {
                if (HF.rendererCount == 1)
                {
                    renderer::UnloadAllResources(IsRunning());
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

        void Begin(const Ref<Renderer>& rn, RenderPass pass)
        {
            inter::HF.renderingApi.api.BeginRenderPass(rn->handle, pass);
        }

        void End(const Ref<Renderer>& rn)
        {
            inter::HF.renderingApi.api.EndRenderPass(rn->handle);
        }
    }
}