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

layout(location = 0) in vec2 inPosition;
layout(location = 0) out vec3 outPosition;

void main()
{
    outPosition = vec3(inPosition.x * 1000 + GLOBAL.CAMERA.position.x, 0, inPosition.y * 1000 + GLOBAL.CAMERA.position.z);
    gl_Position = GLOBAL.CAMERA.viewProj * vec4(outPosition, 1.0);
}