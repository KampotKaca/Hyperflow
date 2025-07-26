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

        const hf::TextureLayoutDefinitionInfo defaultLitLayoutInfo
        {
            .pBindings = &vrMainTexBinding,
            .bindingCount = 1
        };

        APP_TEXTURE_LAYOUTS.default_lit = hf::Define(defaultLitLayoutInfo);
    }
}