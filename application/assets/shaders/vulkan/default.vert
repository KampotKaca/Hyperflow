#version 450

layout(set = 0, binding = 0) uniform Camera
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} CAMERA;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 o_TexCoord;

void main()
{
    gl_Position = CAMERA.viewProj * CAMERA.model * vec4(inPosition, 1.0);
    o_TexCoord = inTexCoord;
}