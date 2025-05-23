#include "resources/texturelayouts.h"

namespace app
{
    AppTextureLayouts APP_TEXTURE_LAYOUTS;

    void TextureLayoutDefineAll()
    {
        hf::TextureLayoutBindingInfo viking_room_binding
        {
            .bindingId = 0,
            .usageFlags = hf::ShaderUsageStage::Default,
            .arraySize = 1,
        };

        hf::TextureLayoutDefinitionInfo default_shader_layout_infp
        {
            .pBindings = &viking_room_binding,
            .bindingCount = 1
        };

        APP_TEXTURE_LAYOUTS.viking_room_layout = hf::texturelayout::Define(default_shader_layout_infp);
    }
}