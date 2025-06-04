#version 450

layout(set = 0, binding = 0) uniform Camera
{
    mat4 model;

    vec3 lookDirection;
    vec3 position;
    mat4 view;
    mat4 invView;
    mat4 proj;
    mat4 invProj;
    mat4 viewProj;

    mat4 padding0;
    vec4 padding1;
    vec4 padding2;
} CAMERA;

layout (set = 0, binding = 1) uniform Time
{
    double deltaTime;
    double timeSinceStartup;
} TIME;

layout(location = 0) in ivec2 inPosition;
layout(location = 0) out vec3 o_RayWorld;

void main()
{
    gl_Position = vec4(inPosition, 0, 1.0);
    vec4 clipPos = vec4(inPosition, -1.0, 1.0);
    vec4 viewPos = CAMERA.invProj * clipPos;
    viewPos /= viewPos.w;
    vec3 rayDirView = normalize(viewPos.xyz);
    o_RayWorld = normalize(mat3(CAMERA.invView) * rayDirView);
}