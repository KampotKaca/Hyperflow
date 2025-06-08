#version 450

layout(set = 0, binding = 0) uniform Camera
{
    vec3 lookDirection;
    vec3 position;
    mat4 view;
    mat4 invView;
    mat4 proj;
    mat4 invProj;
    mat4 viewProj;
} CAMERA;

layout (set = 1, binding = 0) uniform Time
{
    double deltaTime;
    double timeSinceStartup;
} TIME;

layout(push_constant) uniform PushConstants
{
    mat4 model;
} PUSH_CONSTANT;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 o_TexCoord;
layout(location = 1) out vec3 o_Color;

void main()
{
    gl_Position = CAMERA.viewProj * PUSH_CONSTANT.model * vec4(inPosition, 1.0);
    o_Color = inColor;
    o_Color.r *= (sin(float(TIME.timeSinceStartup)) + 1) * 0.5f;
    o_TexCoord = inTexCoord;
}