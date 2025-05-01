#ifndef VKUNIFORMSTORAGE_H
#define VKUNIFORMSTORAGE_H

#include "hvk_shared.h"

namespace hf
{
    struct VkUniformStorage
    {
        VkUniformStorage(const UniformStorageDefinitionInfo& info);
        ~VkUniformStorage();

        VkPipelineLayout layout{};
    };

    bool IsValidStorage(UniformStorage storage);
    const VkUniformStorage& GetStorage(UniformStorage storage);
}

#endif //VKUNIFORMSTORAGE_H
