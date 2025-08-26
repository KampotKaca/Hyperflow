#ifndef HYPERFLOW_HSCENE_H
#define HYPERFLOW_HSCENE_H

#include "hshared.h"

namespace hf
{
    struct Scene
    {
        explicit Scene(const char* assetPath);
        virtual ~Scene();

        virtual void Update() = 0;
        virtual void Render(const Ref<Renderer>& rn) = 0;
    };
}

#endif //HYPERFLOW_HSCENE_H
