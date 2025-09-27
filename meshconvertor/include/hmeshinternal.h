#ifndef HYPERFLOW_HMESHINTERNAL_H
#define HYPERFLOW_HMESHINTERNAL_H

#include "hmeshconvertor.h"
#include "hmeshashared.h"

namespace ml
{
    void WriteData(MeshInfo* meshInfo, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
        hf::MeshDataType dataFlags, uint16_t skinDeformationCount, uint16_t blendDeformationCount);
}

#endif //HYPERFLOW_HMESHINTERNAL_H
