#include "resources/cubemaps.h"

namespace app
{
    AppCubemaps APP_CUBEMAPS;

    void CubemapLoadAll()
    {
        APP_CUBEMAPS.cosmos = hf::Cast<hf::Cubemap>(hf::CreateAsset("cosmos", hf::AssetType::Cubemap));
    }
}