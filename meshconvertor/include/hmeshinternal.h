#ifndef HYPERFLOW_HMESHINTERNAL_H
#define HYPERFLOW_HMESHINTERNAL_H

#include "hmeshashared.h"
#include "hmeshconvertor.h"

namespace ml
{
    void WriteData(MeshInfo* meshInfo, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
        SubMeshHeader::DataType dataType, hf::MeshDataType dataFlags);
}

#endif //HYPERFLOW_HMESHINTERNAL_H
