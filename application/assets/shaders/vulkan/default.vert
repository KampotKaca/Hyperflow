#version 450

layout(set = 0, binding = 0) uniform Camera
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} CAMERA;

layout (set = 0, binding = 1) uniform Time
{
    double deltaTime;
    double timeSinceStartup;
} TIME;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 o_TexCoord;
layout(location = 1) out vec3 o_Color;

void main()
{
    gl_Position = CAMERA.viewProj * CAMERA.model * vec4(inPosition, 1.0);
    o_Color = inColor * sin(float(TIME.timeSinceStartup));
    o_TexCoord = inTexCoord;
}