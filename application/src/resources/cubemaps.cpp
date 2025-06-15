#include "resources/cubemaps.h"

namespace app
{
    AppCubemaps APP_CUBEMAPS;

    void CubemapLoadAll()
    {
        APP_CUBEMAPS.cosmos = hf::cubemap::Create("cosmos");
    }
}