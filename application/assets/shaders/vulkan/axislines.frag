#version 450

layout(set = 0, binding = 0) uniform Camera
{
    mat4 model;

    vec3 lookDirection;
    vec3 position;
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} CAMERA;

layout (set = 0, binding = 1) uniform Time
{
    double deltaTime;
    double timeSinceStartup;
} TIME;

layout(set = 1, binding = 0) uniform AxisLineUniform
{
    uint axis;
    vec3 lineThickness;
    vec3 offset;
    vec4 color;
} AXIS_LINES;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(1.0f, 1.0f, 1.0f, 0.2f);
}