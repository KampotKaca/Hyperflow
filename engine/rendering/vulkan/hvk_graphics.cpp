#include "hvk_graphics.h"
#include "hyperflow.h"
#include "../config.h"

namespace hf
{
    GraphicsData GRAPHICS_DATA;

    extern void GraphicsLoad(const char* appVersion)
    {
        auto engineV = utils::ConvertVersion(VERSION);
        auto appV = utils::ConvertVersion(appVersion);
        auto& appInfo = GRAPHICS_DATA.appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = GetApplicationTitle().c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(appV.x, appV.y, appV.z);
        appInfo.pEngineName = HF_ENGINE_TITLE;
        appInfo.engineVersion = VK_MAKE_VERSION(engineV.x, engineV.y, engineV.z);
        appInfo.apiVersion = VULKAN_API_VERSION;
    }

    extern void GraphicsUnload()
    {

    }

}