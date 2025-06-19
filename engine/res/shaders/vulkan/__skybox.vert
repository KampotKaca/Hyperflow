#version 450

struct Camera
{
    vec3 lookDirection;
    vec3 position;
    mat4 view;
    mat4 invView;
    mat4 proj;
    mat4 invProj;
    mat4 viewProj;
};

struct Time
{
    double deltaTime;
    double timeSinceStartup;
};

layout(set = 0, binding = 0) uniform Global
{
    Camera CAMERA;
    Time TIME;
} GLOBAL;

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec3 outTexcoord;

void main()
{
    vec4 pos = GLOBAL.CAMERA.proj * mat4(mat3(GLOBAL.CAMERA.view)) * vec4(inPosition, 1.0);
    gl_Position = vec4(pos.xy, pos.w * 0.9999, pos.w);
    outTexcoord = inPosition;
}