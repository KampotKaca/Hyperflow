#include "htexturepack.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    TexturePack::TexturePack(const TexturePackCreationInfo& info)
    {
        creationInfo = info;
        creationInfo.pTextures = (TextureCreationInfo*)utils::Allocate(info.textureCount * sizeof(TextureCreationInfo));
        for (uint32_t i = 0; i < info.textureCount; i++)
        {
            
        }
    }

    TexturePack::~TexturePack()
    {

    }

    namespace inter::rendering
    {
        bool CreateTexturePack_i(TexturePack* texPack)
        {
            if (texPack->handle) return false;

            return true;
        }

        bool DestroyTexturePack_i(TexturePack* texPack)
        {
            if (texPack->handle)
            {

                texPack->handle = nullptr;
                return true;
            }
            return false;
        }
    }
}
