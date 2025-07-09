#version 450
#include "global"

layout(location = 0) in vec3 o_Color;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = FinalizeColor(vec4(o_Color, 1.0f));
}