#ifndef HYPERFLOW_HMESHINTERNAL_H
#define HYPERFLOW_HMESHINTERNAL_H

#include "hmodelconvertor.h"
#include "hmodelshared.h"
#include <iostream>

namespace ml
{
    void WriteData(ModelInfo* meshInfo, const hf::List<Vertex>& vertices, const hf::List<uint32_t>& indices,
        hf::MeshDataType dataFlags, uint16_t skinDeformationCount, uint16_t blendDeformationCount);
}

#endif //HYPERFLOW_HMESHINTERNAL_H
