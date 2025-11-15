#version 450
#include <global.glsl>

layout(push_constant) uniform PushConstants
{
    vec2 drawData;
    vec4 color;
} PUSH_CONSTANT;

layout(location = 0) in vec3 outPosition;
layout(location = 0) out vec4 outColor;

bool GetGridColor(float dist, vec2 absIntersection, float scale, float opacity, out vec4 o_Color)
{
    vec2 fraction = (absIntersection - ivec2(absIntersection * scale) / scale) * scale;
    vec2 smoothWidth = PUSH_CONSTANT.drawData.x * fwidth(fraction);

    vec2 lines = smoothstep(vec2(0.0), smoothWidth, fraction) *
                 smoothstep(vec2(0.0), smoothWidth, vec2(1.0) - fraction);

    if(min(lines.x, lines.y) < 0.5)
    {
        o_Color = PUSH_CONSTANT.color / (1 + dist * opacity);
        return true;
    }
    return false;
}

void main()
{
    float dist = length(outPosition - GLOBAL.CAMERA.position);
    vec2 absIntersection = abs(outPosition.xz);
    if(GetGridColor(dist, absIntersection, 0.01, 0.01, outColor)) return;
    if(GetGridColor(dist, absIntersection, 0.1, 0.075, outColor)) return;
    if(GetGridColor(dist, absIntersection, 1, 0.25, outColor)) return;

    discard;
}