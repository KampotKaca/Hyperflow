#include "hshader.h"

namespace hf::shader
{
    bool IsAlive(const Ref<Shader>& shader) { return shader->isAlive; }
}