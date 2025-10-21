#include "hrenderer.h"
#include <hyperflow.h>
#include "hinternal.h"
#include "../config.h"
#include "../../application/appconfig.h"
#include "../rendering/include/hex_renderer.h"

namespace hf
{
    static void ThreadDraw(const Ref<Renderer>& rn)
    {
        inter::alloc::LoadAllocatorThread_i();

        rn->threadInfo.isRunning = true;
        uint64_t renderFrameCount = 0;
        while (rn->threadInfo.isRunning)
        {
            inter::rendering::RendererUpdate_i(rn);
            if (renderFrameCount % 1024 == 0) utils::CollectThreadMemoryCache();

            std::lock_guard lock(rn->threadInfo.statLock);
            rn->threadInfo.memoryStatistics = utils::GetThreadMemoryStatistics();
            renderFrameCount++;
        }
        rn->threadInfo.isRunning = false;
        inter::HF.renderingApi.api.WaitForDevice();

        inter::alloc::UnloadAllocatorThread_i();
    }

    Renderer::Renderer(const Window* window, const RendererEventInfo& eventInfo)
        : eventInfo(eventInfo)
    {
        this->window = window;
        threadInfo.size = inter::window::GetSize(window);

        threadInfo.cachedPackets = std::vector<RenderPacket*>(3);
        for (uint32_t i = 0; i < 3; i++)
        {
            threadInfo.cachedPackets[i] = &allPackets[i];
        }

        inter::rendering::CreateRenderer_i(this);
    }

    Renderer::~Renderer()
    {
        inter::rendering::DestroyRenderer_i(this);
    }

    bool IsLoaded(const Ref<Renderer>& rn) { return rn->handle; }
    uvec2 GetSize(const Ref<Renderer>& rn)  { return rn->threadInfo.size; }
    void Resize(const Ref<Renderer>& rn, uvec2 size)
    {
        std::lock_guard lock(rn->threadInfo.threadLock);
        if (rn->window != nullptr) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot resize renderer connected to the window");
        rn->threadInfo.size = size;
        inter::HF.renderingApi.api.RegisterFrameBufferChange(rn->handle, size);
    }
    ThreadMemoryStatistics GetMemoryStatistics(const Ref<Renderer>& rn)
    {
        ThreadMemoryStatistics stats{};
        {
            std::lock_guard lock(rn->threadInfo.statLock);
            stats = rn->threadInfo.memoryStatistics;
        }
        return stats;
    }

    RenderingApiType GetApiType()     { return inter::HF.renderingApi.type; }
    RenderingApiType GetBestApiType() { return inter::platform::GetBestRenderingApi(); }

    bool IsValidApi(RenderingApiType targetApi)
    {
        if (targetApi == RenderingApiType::None || targetApi == inter::HF.renderingApi.type) return false;
        return inter::platform::IsValidRenderingApi(targetApi);
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
                RunRenderThread_i(win->renderer);
            }

            general::DefineStaticResources_i();
            if (HF.lifecycleCallbacks.onRendererLoad) HF.lifecycleCallbacks.onRendererLoad();
        }

        void UnloadCurrentApi_i(bool retainReferences)
        {
            if (HF.renderingApi.type == RenderingApiType::None) return;

            for (const auto& window : HF.windows | std::views::values)
            {
                if (window->renderer)
                {
                    DestroyRenderer_i(window->renderer.get());
                    if (!retainReferences) window->renderer = nullptr;
                }
            }

            platform::UnloadDll(HF.renderingApi.handle);
            HF.renderingApi = RenderingApi{};
            HF.graphicsResources.vertexAttributes.clear();
        }

        void CreateRenderer_i(Renderer* rn)
        {
            if (HF.rendererCount == 0)
            {
                const auto engineV = utils::ConvertVersion(VERSION);
                const auto appV = utils::ConvertVersion(APP_VERSION);

                RendererInternalFunctions_i funcs{};
                funcs.fileExistsFunc = utils::FileExists;
                funcs.readFileFunc = utils::ReadFile;
                funcs.writeFileFunc = utils::WriteFile;

                funcs.allocateFunc = utils::Allocate;
                funcs.allocateAlignedFunc = utils::AllocateAligned;
                funcs.deallocateFunc = utils::Deallocate;
                funcs.deallocateAlignedFunc = utils::DeallocateAligned;
                funcs.reallocateFunc = utils::Reallocate;

                RendererLoadInfo_i loadInfo
                {
                    .appVersion = appV,
                    .engineVersion = engineV,
                    .applicationTitle = HF.appTitle.c_str(),
                    .functions = funcs
                };

                if (HF.renderingApi.type == RenderingApiType::Vulkan)
                    loadInfo.functions.createVulkanSurfaceFunc = platform::CreateVulkanSurface;

                HF.renderingApi.api.Load(loadInfo);
                HF.renderingApi.isLoaded = true;
            }
            HF.rendererCount++;
            RendererInstanceCreationInfo_i createInfo{};
            createInfo.size = rn->threadInfo.size;

            if (rn->window)
            {
                createInfo.vSyncMode = rn->window->vSyncMode;
                createInfo.handle = rn->window->handle;
            }

            createInfo.initCallback = rn->eventInfo.onRendererInitCallback;
            createInfo.shutdownCallback = rn->eventInfo.onRendererShutdownCallback;
            rn->handle = HF.renderingApi.api.CreateInstance(createInfo);
        }

        void DestroyRenderer_i(Renderer* rn)
        {
            if (rn->handle)
            {
                {
                    std::lock_guard lock(HF.deletedResources.syncLock);
                    HF.rendererCount--;
                    rn->threadInfo.isRunning = false;
                }

                rn->threadInfo.renderCondition.notify_all();
                rn->threadInfo.thread.join();

                if (HF.rendererCount == 0)
                {
                    UnloadAllResources_i(IsRunning());
                    CleanMarkedResources_i();
                }

                HF.renderingApi.api.DestroyInstance(rn->handle);

                if (HF.rendererCount == 0)
                {
                    HF.renderingApi.isLoaded = false;
                    HF.renderingApi.api.Unload();
                }
                rn->handle = nullptr;
            }
        }

        void RunRenderThread_i(const Ref<Renderer>& rn)
        {
            rn->threadInfo.thread = std::thread(ThreadDraw, rn);
        }
    }
}