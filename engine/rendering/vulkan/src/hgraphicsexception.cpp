#include "hgraphicsexception.h"
#include <sstream>

namespace hf
{
    GraphicsException::GraphicsException(int32_t lineNum, const char* file, VkResult errorCode)
        : HyperException(lineNum, file), m_ErrorCode(errorCode) { }

    const char* GraphicsException::what() const noexcept
    {
        std::ostringstream oss;
        oss << GetType() << std::endl
            << "[Error Code] " << std::hex << GetErrorCode() << std::endl
            << "[Description] " << GetErrorString();

        m_WhatBuffer = oss.str();
        return m_WhatBuffer.c_str();
    }

    std::string GraphicsException::TranslateErrorCode(VkResult result)
    {
        switch (result)
        {
            case VK_SUCCESS:                                    return "Command successfully completed.";
            case VK_NOT_READY:                                  return "A fence or query has not yet completed.";
            case VK_TIMEOUT:                                    return "A wait operation has not completed in the specified time.";
            case VK_EVENT_SET:                                  return "An event is signaled.";
            case VK_EVENT_RESET:                                return "An event is unsignaled.";
            case VK_INCOMPLETE:                                 return "A return array was too small for the result.";
            case VK_ERROR_OUT_OF_HOST_MEMORY:                   return "Host memory allocation failed.";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:                 return "Device memory allocation failed.";
            case VK_ERROR_INITIALIZATION_FAILED:                return "Initialization of an object failed.";
            case VK_ERROR_DEVICE_LOST:                          return "The logical or physical device has been lost.";
            case VK_ERROR_MEMORY_MAP_FAILED:                    return "Mapping a memory object has failed.";
            case VK_ERROR_LAYER_NOT_PRESENT:                    return "A requested layer is not present.";
            case VK_ERROR_EXTENSION_NOT_PRESENT:                return "A requested extension is not supported.";
            case VK_ERROR_FEATURE_NOT_PRESENT:                  return "A requested feature is not supported.";
            case VK_ERROR_INCOMPATIBLE_DRIVER:                  return "The requested version is not supported by the driver.";
            case VK_ERROR_TOO_MANY_OBJECTS:                     return "Too many objects of the type have been created.";
            case VK_ERROR_FORMAT_NOT_SUPPORTED:                 return "The requested format is not supported.";
            case VK_ERROR_FRAGMENTED_POOL:                      return "A pool allocation has failed due to fragmentation.";
            case VK_ERROR_SURFACE_LOST_KHR:                     return "A surface is no longer available.";
            case VK_ERROR_OUT_OF_DATE_KHR:                      return "The swapchain is out of date.";
            case VK_SUBOPTIMAL_KHR:                             return "The swapchain no longer matches the surface.";
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:             return "The requested window is already in use.";
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:             return "The display is incompatible.";
            case VK_ERROR_VALIDATION_FAILED_EXT:                return "A validation layer error was encountered.";
            case VK_ERROR_INVALID_SHADER_NV:                    return "A provided shader is invalid.";
            case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:     return "Video decode parameters are invalid.";
            case VK_ERROR_FRAGMENTATION_EXT:                    return "A descriptor pool allocation failed due to fragmentation.";
            case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:  return "Full-screen mode was lost.";
            case VK_ERROR_OUT_OF_POOL_MEMORY:                   return "Pool memory allocation failed.";
            case VK_ERROR_UNKNOWN:                              return "An unknown error has occurred.";
            default:                                            return "Unknown VkResult error.";
        }
    }

    VkResult GraphicsException::GetErrorCode() const noexcept { return m_ErrorCode; }
    const char* GraphicsException::GetType() const noexcept { return "[Vulkan Exception]"; }
    std::string GraphicsException::GetErrorString() const noexcept { return TranslateErrorCode(m_ErrorCode); }
}