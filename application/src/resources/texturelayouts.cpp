#include "resources/texturelayouts.h"

namespace app
{
    AppTextureLayouts APP_TEXTURE_LAYOUTS;

    void TextureLayoutDefineAll()
    {
        hf::TextureLayoutBindingInfo vrMainTexBinding
        {
            .bindingId = 0,
            .usageFlags = hf::ShaderUsageStage::Default,
            .arraySize = 1,
        };

        const hf::TextureLayoutDefinitionInfo vikingRoomLayoutInfo
        {
            .pBindings = &vrMainTexBinding,
            .bindingCount = 1
        };

        APP_TEXTURE_LAYOUTS.viking_room = hf::DefineTextureLayout(vikingRoomLayoutInfo);
    }
}