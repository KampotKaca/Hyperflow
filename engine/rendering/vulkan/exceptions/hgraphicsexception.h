#ifndef HVK_EXCEPTION_H
#define HVK_EXCEPTION_H

#include "hexception.h"
#include "hshared.h"
#include <vulkan/vulkan.h>

namespace hf
{
    class GraphicsException : public HyperException
    {
    public:
        GraphicsException(int32_t lineNum, const char* file, VkResult errorCode);

        const char* what() const noexcept override;

        static std::string TranslateErrorCode(VkResult result);

        VkResult GetErrorCode() const noexcept;
        const char* GetType() const noexcept override;
        std::string GetErrorString() const noexcept;
    private:
        VkResult m_ErrorCode;
    };

#define VK_EXCEPT(hr) GraphicsException(__LINE__, __FILE__, hr)
#define VK_HANDLE_EXCEPT(x) \
        {\
            VkResult res = x;\
            if(res != 0) throw GraphicsException(__LINE__, __FILE__, res);\
        }
}

#endif //HVK_EXCEPTION_H
