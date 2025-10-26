#ifndef HYPERFLOW_HSCENE_H
#define HYPERFLOW_HSCENE_H

#include "hshared.h"

namespace hf
{
    struct Scene
    {
        explicit Scene(const AssetLocation* assets, uint32_t assetCount);
        virtual ~Scene();

        virtual void Update() = 0;
        virtual void Render(const Ref<Renderer>& rn) = 0;

        List<AssetLocation> assets{};
        bool isLoaded = false;
    };
}

#endif //HYPERFLOW_HSCENE_H
