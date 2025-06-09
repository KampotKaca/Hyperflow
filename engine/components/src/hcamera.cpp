#include "hexternal.h"
#include "hcamera3dcore.h"
#include "hcamera3danchored.h"
#include "hcamera3dfreelook.h"

namespace hf
{
    static void UploadUniform(const Ref<Renderer>& rn, const Camera3DCore& core,
                              vec3 lookDirection, vec3 position, const mat4& view);

    mat4 Camera3DCore::ToProjectionMat4(const Ref<Renderer>& rn) const
    {
        const auto& size = rn->threadInfo.size;
        const float aspect = (float)size.x / (float)size.y;
        return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }

    mat4 Camera3DAnchored::ToViewProjectionMat4(const Ref<Renderer>& rn) const
    {
        return core.ToProjectionMat4(rn) * ToViewMat4();
    }

    mat4 Camera3DFreeLook::ToViewProjectionMat4(const Ref<Renderer>& rn) const
    {
        return core.ToProjectionMat4(rn) * ToViewMat4();
    }

    void Camera3DAnchored::UploadInUniform(const Ref<Renderer>& rn) const
    {
        UploadUniform(rn, core, GetDirection(), GetPosition(), ToViewMat4());
    }

    void Camera3DFreeLook::UploadInUniform(const Ref<Renderer>& rn) const
    {
        UploadUniform(rn, core, direction, position, ToViewMat4());
    }

    void Camera3DCore::BindCurrentToUniform(const Ref<Renderer>& rn)
    {
        constexpr UniformBufferBindInfo info
        {
            .bindingType = RenderBindingType::Graphics,
            .setBindingIndex = HF_UNIFORM_CAMERA_SET_INDEX,
            .pUniforms = &inter::HF.staticResources.cameraUniform,
            .uniformCount = 1,
        };

        draw::ShaderSetupAdd_UniformBinding(rn, info);
    }

    void UploadUniform(const Ref<Renderer>& rn, const Camera3DCore& core,
                       const vec3 lookDirection, const vec3 position, const mat4& view)
    {
        static CameraUniform UNIFORM{};
        UNIFORM.position = position;
        UNIFORM.lookDirection = lookDirection;
        UNIFORM.view = view;
        UNIFORM.invView = glm::inverse(view);
        UNIFORM.proj = core.ToProjectionMat4(rn);
        UNIFORM.invProj = glm::inverse(UNIFORM.proj);
        UNIFORM.viewProj = UNIFORM.proj * UNIFORM.view;

        const UniformBufferUpload uniformUpload
        {
            .buffer = inter::HF.staticResources.cameraUniform,
            .offsetInBytes = 0,
            .sizeInBytes = sizeof(CameraUniform),
            .data = &UNIFORM
        };

        draw::UploadUniformPacket(rn, uniformUpload);
    }
}