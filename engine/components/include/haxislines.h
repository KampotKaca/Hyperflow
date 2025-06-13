#ifndef HAXISLINES_H
#define HAXISLINES_H

#include "hshared.h"

namespace hf
{
    //Drawing operation should always be done after everything.
    struct AxisLines
    {
        AxisLines() = default;
        ~AxisLines() = default;

        void Draw(const Ref<Renderer>& rn) const;

        alignas(16) float lineThickness = 3.0f;
        alignas(16) vec4 color{ 0.2f, 0.2f, 0.2f, 0.4f };
    };
}

#endif //HAXISLINES_H
