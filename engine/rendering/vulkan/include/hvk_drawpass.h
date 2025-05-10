#ifndef HVK_DRAWPASS_H
#define HVK_DRAWPASS_H

#include "hvk_shared.h"

namespace hf
{
    struct VkDrawPass
    {
        VkDrawPass(const DrawPassDefinitionInfo& info);
        ~VkDrawPass();

        VkRenderPass pass{};
        uint32_t attachmentCount = 0;
    };

    bool IsValidDrawPass(DrawPass pass);
    const VkDrawPass& GetDrawPass(DrawPass pass);
}

#endif //HVK_DRAWPASS_H
