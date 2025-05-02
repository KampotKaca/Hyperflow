#version 450

layout(location = 0) in vec4 o_FragColor;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = o_FragColor;
}