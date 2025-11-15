//
// Created by ratib on 14.11.2025.
//

#ifndef HYPERFLOW_HSHADERCONVERTOR_H
#define HYPERFLOW_HSHADERCONVERTOR_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <shaderc/shaderc.hpp>
#include <string>
#include "henums.h"
#include "hstrconversion.h"

namespace sh
{
    namespace fs = std::filesystem;

#if RENDERER_VULKAN
    void HandleVulkanShaders(const std::vector<std::string>& rootDirs, char** paths, uint32_t count);
#else
    inline void HandleVulkanShaders(const std::vector<std::string>& rootDirs, char** paths, uint32_t count)
    {
        log_warn_s("[Hyperflow] Unable to process Vulkan shaders if you don't have vulkan renderer enabled!");
    }
#endif

#if RENDERER_D3D
    void HandleDirect3DShaders(const std::vector<std::string>& rootDirs, char** paths, uint32_t count);
#else
    inline void HandleDirect3DShaders(const std::vector<std::string>& rootDirs, char** paths, uint32_t count)
    {
        log_warn_s("[Hyperflow] Unable to process Direct3D shaders if you don't have direct3d renderer enabled!");
    }
#endif
}

#endif //HYPERFLOW_HSHADERCONVERTOR_H
