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

layout(set = 1, binding = 0) uniform samplerCube skyboxSampler;

layout(location = 0) in vec3 outTexcoord;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(skyboxSampler, outTexcoord);
}