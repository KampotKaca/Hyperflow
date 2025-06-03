#ifndef HAXISLINES_H
#define HAXISLINES_H

#include "hshared.h"

namespace hf
{
    struct AxisLines
    {
        AxisLines() = default;
        ~AxisLines() = default;

        static void GetNecessaryUniforms(UniformBuffer* location);

        void Draw(const Ref<Renderer>& rn) const;

        vec3 planeNormal = { 0.0, 1.0, 0.0 };
        float lineThickness = 1.5f;
        vec4 color{ 1.0f, 1.0f, 1.0f, 0.1f };
    };
}

#endif //HAXISLINES_H
