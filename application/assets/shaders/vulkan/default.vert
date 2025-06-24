#version 450
#include "global"

layout(push_constant) uniform PushConstants
{
    mat4 model;
    uint materialIndex;
} PUSH_CONSTANT;

struct Material
{
    vec4 albedo;
    int padding[MAX_MATERIAL_SIZE / 4 - 4 * 4];
};

#include "material"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 o_TexCoord;
layout(location = 1) out vec3 o_Color;

void main()
{
    gl_Position = GLOBAL.CAMERA.viewProj * PUSH_CONSTANT.model * vec4(inPosition, 1.0);
    Material mat = MATERIAL_BUFFER.data[PUSH_CONSTANT.materialIndex];
    o_Color = inColor * vec3(mat.albedo);
    o_TexCoord = inTexCoord;
}