#ifndef HYPERFLOW_HMODEL_H
#define HYPERFLOW_HMODEL_H

#include "hshared.h"

namespace hf
{
    struct Model
    {
        explicit Model(const ModelCreationInfo& info);
        ~Model();

        std::string filePath{};
        bool isLoaded = false;

        List<Ref<Mesh>> meshes{};
        MeshStats meshStats{};
    };
}

#endif //HYPERFLOW_HMODEL_H