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
        inter::rendering::CreateRenderer(this);
    }

    Renderer::Renderer(const Window* window)
    {
        windowHandle = inter::window::GetWindowHandle(window);
        size = window->rect.size;
        inter::rendering::CreateRenderer(this);
    }

    Renderer::~Renderer()
    {
        inter::rendering::DestroyRenderer(this);
    }

    namespace renderer
    {
        Ref<Renderer> Create(uvec2 size)
        {
            return MakeRef<Renderer>(size);
        }

        void Destroy(const Ref<Renderer>& rn)
        {
            inter::rendering::DestroyRenderer(rn.get());
        }

        bool IsRunning(const Ref<Renderer>& rn) { return rn->handle; }

        bool IsApiSupported(RenderingApiType targetApi)
        {
            return true;
        }

        void QuerySupportedApis(std::vector<RenderingApiType>& apis)
        {

        }

        void ChangeApi(RenderingApiType targetApi)
        {
            if (targetApi == inter::HF.renderingApi.type) return;
            inter::rendering::UnloadCurrentApi(false);
            inter::rendering::LoadApi(targetApi);
        }

        void Resize(const Ref<Renderer>& rn, uvec2 size)
        {
            if (rn->windowHandle != nullptr) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot resize renderer connected to the window");
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

        void UnloadAllResources()
        {
            if (inter::HF.renderingApi.isLoaded) inter::HF.renderingApi.api.WaitForRendering();
            vertbuffer::DestroyAll();
            indexbuffer::DestroyAll();
            shader::DestroyAll();
        }
    }

    namespace inter::rendering
    {
        void LoadApi(RenderingApiType api)
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
                if (win->renderer) CreateRenderer(win->renderer.get());
                else win->renderer = MakeRef<Renderer>(win.get());
            }
        }

        void UnloadCurrentApi(bool retainReferences)
        {
            if (HF.renderingApi.type == RenderingApiType::None) return;

            for (auto& window : HF.windows)
            {
                if (window->renderer)
                {
                    DestroyRenderer(window->renderer.get());
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

        void CreateRenderer(Renderer* rn)
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
                .handle = rn->windowHandle,
                .size = rn->size,
            };
            rn->handle = HF.renderingApi.api.CreateInstance(createInfo);
        }

        void DestroyRenderer(Renderer* rn)
        {
            if (rn->handle)
            {
                if (HF.rendererCount == 1)
                {
                    renderer::UnloadAllResources();
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
}