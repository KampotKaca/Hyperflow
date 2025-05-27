#ifndef FREEMOVECAMERA3D_H
#define FREEMOVECAMERA3D_H

#include "hcamera3dfreelook.h"

namespace hf
{
    enum class FreeMoveCameraDirection
    {
        Forward = 1 << 0, Backward = 1 << 1,
        Right = 1 << 2, Left = 1 << 3,
        Up = 1 << 4, Down = 1 << 5,
        XAxis = Right | Left,
        YAxis = Up | Down,
        ZAxis = Forward | Backward,
        All = Forward | Backward | Right | Left | Up | Down,
    };
    enum class FreeMoveCameraLookAxis
    {
        Horizontal = 1 << 0, Vertical = 1 << 1,
        All = Horizontal | Vertical,
    };

    DEFINE_ENUM_FLAGS(FreeMoveCameraDirection)
    DEFINE_ENUM_FLAGS(FreeMoveCameraLookAxis)

    struct FreeMoveCamera3D
    {
        FreeMoveCamera3D() = default;
        ~FreeMoveCamera3D() = default;

        void Update(const Ref<Window>& window, float deltaTime);

        Camera3DFreeLook camera3D{};
        FreeMoveCameraDirection moveFlags = FreeMoveCameraDirection::All;
        FreeMoveCameraLookAxis lookFlags = FreeMoveCameraLookAxis::All;
        float lookSpeed = 20.0f;
        float moveSpeed = 20.0f;

        vec2 zoomLimits = { 2, 1000 };
        vec2 pitchLimits = { -89, 89 };
    };
}

#endif //FREEMOVECAMERA3D_H
