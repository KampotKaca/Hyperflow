#include "hrenderer.h"
#include <hyperflow.h>
#include "hinternal.h"
#include "../config.h"
#include "../../application/appconfig.h"
#include "../rendering/include/hex_renderer.h"

namespace hf
{
    static void ThreadDraw()
    {
        ir::alloc::LoadAllocatorThread_i();

        ir::HF.renderer->threadInfo.isRunning = true;
        uint64_t renderFrameCount = 0;
        while (ir::HF.renderer->threadInfo.isRunning)
        {
            ir::rdr::RendererUpdate_i();
            if (renderFrameCount % 1024 == 0) utils::CollectThreadMemoryCache();

            std::lock_guard lock(ir::HF.renderer->threadInfo.statLock);
            ir::HF.renderer->threadInfo.memoryStatistics = utils::GetThreadMemoryStatistics();
            renderFrameCount++;
        }
        ir::HF.renderer->threadInfo.isRunning = false;
        ir::HF.renderingApi.api.WaitForDevice();

        ir::alloc::UnloadAllocatorThread_i();
    }

    Renderer::Renderer(const RendererEventInfo& eventInfo) : eventInfo(eventInfo)
    {
        threadInfo.size = ir::win::GetSize(ir::HF.window.get());

        for (uint32_t i = 0; i < 3; i++)
        {
            auto ptr = &allPackets[i];
            ptr->preallocate();
            threadInfo.cachedPackets.push_back(ptr);
        }

        ir::rdr::CreateRenderer_i(this);
    }

    Renderer::~Renderer()
    {
        ir::rdr::DestroyRenderer_i(this);
    }

    bool IsRendererRunning() { return ir::HF.renderer->handle; }
    ThreadMemoryStatistics GetRendererMemoryStatistics()
    {
        ThreadMemoryStatistics stats{};
        {
            std::lock_guard lock(ir::HF.renderer->threadInfo.statLock);
            stats = ir::HF.renderer->threadInfo.memoryStatistics;
        }
        return stats;
    }

    RenderingApiType GetApiType()     { return ir::HF.renderingApi.type; }
    RenderingApiType GetBestApiType() { return ir::platform::GetBestRenderingApi(); }

    bool IsValidApi(RenderingApiType targetApi)
    {
        if (targetApi == RenderingApiType::None || targetApi == ir::HF.renderingApi.type) return false;
        return ir::platform::IsValidRenderingApi(targetApi);
    }

    namespace ir::rdr
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

            if (HF.renderer) CreateRenderer_i(HF.renderer.get());
            else HF.renderer = MakeURef<Renderer>(HF.window->rnEventInfo);
            RunRenderThread_i();

            general::DefineStaticResources_i();
            if (HF.lifecycleCallbacks.onRendererLoad) HF.lifecycleCallbacks.onRendererLoad();
        }

        void UnloadCurrentApi_i(bool retainReferences)
        {
            if (HF.renderingApi.type == RenderingApiType::None) return;

            if (HF.renderer)
            {
                DestroyRenderer_i(HF.renderer.get());
                if (!retainReferences) HF.renderer = nullptr;
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

                funcs.allocateFunc = utils::Alloc;
                funcs.allocateAlignedFunc = utils::AllocAligned;
                funcs.deallocateFunc = utils::Deallocate;
                funcs.deallocateAlignedFunc = utils::DeallocateAligned;
                funcs.reallocateFunc = utils::Realloc;

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

            if (HF.window)
            {
                createInfo.vSyncMode = HF.window->vSyncMode;
                createInfo.handle = HF.window->handle;
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

        void RunRenderThread_i()
        {
            HF.renderer->threadInfo.thread = std::thread(ThreadDraw);
        }

        void ResizeRenderer_i(uvec2 size)
        {
            std::lock_guard lock(HF.renderer->threadInfo.threadLock);
            HF.renderer->threadInfo.size = size;
            HF.renderingApi.api.RegisterFrameBufferChange(HF.renderer->handle, size);
        }
    }
}