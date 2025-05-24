#include "components/hfreemovecamera3d.h"
#include "hyperflow.h"
#include "glm/gtx/rotate_vector.hpp"

namespace hf
{
    void FreeMoveCamera3D::Update(const Ref<Window>& window, float deltaTime)
    {
        vec3 motion = vec3(0.0f);
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Forward) > 0 && input::IsDownContinues(window, Key::W)) motion.z += 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Backward) > 0 && input::IsDownContinues(window, Key::S)) motion.z -= 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Right) > 0 && input::IsDownContinues(window, Key::D)) motion.x += 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Left) > 0 && input::IsDownContinues(window, Key::D)) motion.x -= 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Up) > 0 && input::IsDownContinues(window, Key::Space)) motion.y += 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Down) > 0 && input::IsDownContinues(window, Key::LeftShift)) motion.y -= 1.0f;

        vec2 lookAmount = vec2(lookFlags & FreeMoveCameraLookAxis::Horizontal, lookFlags & FreeMoveCameraLookAxis::Vertical);
        auto delta = input::GetPointerDelta(window);
        lookAmount.x *= (float)delta.x;
        lookAmount.y *= (float)delta.y;

        motion *= moveSpeed * deltaTime;
        lookAmount *= lookSpeed * deltaTime;

        camera3D.pitch += lookAmount.y;

        auto direction = camera3D.GetDirection();
        vec3 right = glm::cross(direction, camera3D.up);
        camera3D.anchor += right * motion.x;
        camera3D.anchor += camera3D.up * motion.y;
        camera3D.anchor += direction * motion.z;
    }
}
