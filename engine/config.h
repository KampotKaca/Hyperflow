#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "0.0.0"
#define HF_ENGINE_TITLE "Hyperflow"

#define RENDERER_D3D
#define RENDERER_VULKAN

#ifdef RENDERER_VULKAN
#define VULKAN_API_VERSION VK_API_VERSION_1_0
#endif

#endif //CONFIG_H
