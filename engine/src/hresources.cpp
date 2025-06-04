#include "hinternal.h"
#include "hshared.h"
#include "hyperflow.h"

namespace hf::resources
{
    BufferAttrib GetQuadBufferAttrib() { return inter::HF.staticResources.quadAttrib; }
    TextureLayout GetEmptyTextureLayout() { return inter::HF.staticResources.emptyLayout; }

    BufferAttrib GetCubeBufferAttrib() { return inter::HF.staticResources.cubeAttrib; }
    TextureSampler GetCubemapSampler() { return inter::HF.staticResources.cubemapSampler; }
    Ref<VertBuffer> GetCubeVertices() { return inter::HF.staticResources.cubeVertices; }
    Ref<IndexBuffer> GetCubeIndices() { return inter::HF.staticResources.cubeIndices; }
}