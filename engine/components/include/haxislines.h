#ifndef HAXISLINES_H
#define HAXISLINES_H

#include "hshared.h"

namespace hf
{
    struct AxisLines
    {
        AxisLines() = default;
        ~AxisLines() = default;

        static void StartDrawProcess(const Ref<Renderer>& rn);
        static void EndDrawProcess(const Ref<Renderer>& rn);
        void Draw(const Ref<Renderer>& rn) const;

        struct UploadInfo
        {
            alignas(16) vec4 planeNormal = { 0.0, 1.0, 0.0, 1.0 };
            alignas(16) vec4 color{ 1.0f, 1.0f, 1.0f, 0.1f };
        };

        vec3 planeNormal = { 0.0, 1.0, 0.0 };
        float lineThickness = 1.5f;
        vec4 color{ 1.0f, 1.0f, 1.0f, 0.1f };
    };
}

#endif //HAXISLINES_H
