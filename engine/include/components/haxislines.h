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

        void Draw(const Ref<Renderer>& rn);

        Axis axis = Axis::X | Axis::Z;
        vec3 lineThickness = { 0.1f, 0.1f, 0.1f };
        vec3 offset{};
        vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
    };
}

#endif //HAXISLINES_H
