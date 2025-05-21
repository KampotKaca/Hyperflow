#include "hshared.h"
#include "hrenderer.h"

namespace hf
{
    mat4 Camera3D::ToProjectionMat4(const Ref<Renderer>& rn) const
    {
        float aspect = (float)rn->size.x / (float)rn->size.y;
        return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }

    mat4 Camera3D::ToViewProjectionMat4(const Ref<Renderer>& rn) const
    {
        return ToProjectionMat4(rn) * ToViewMat4();
    }
}