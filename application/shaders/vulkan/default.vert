#version 450

layout(binding = 0) uniform Camera
{
    vec4 color;
} CAMERA;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec4 o_FragColor;

void main()
{
    gl_Position = vec4(inPosition, 0.0, 1.0);
    o_FragColor = vec4(inColor, 1.0) * CAMERA.color;
}