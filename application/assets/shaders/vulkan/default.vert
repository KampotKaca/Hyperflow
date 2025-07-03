#version 450
#include "global"

layout(push_constant) uniform PushConstants
{
    mat4 model;
    vec4 albedo;
    vec4 phongData; //xyz specular color, w smoothness
} PUSH_CONSTANT;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 o_TexCoord;
layout(location = 1) out vec3 o_Color;
layout(location = 2) out vec3 o_ViewNormal;
layout(location = 3) out vec4 o_ViewPosition;

void main()
{
    mat4 modelView     = GLOBAL.CAMERA.view * PUSH_CONSTANT.model;
    mat3 normalMatrix  = transpose(inverse(mat3(modelView)));

    o_Color = vec3(PUSH_CONSTANT.albedo);
    o_TexCoord = inTexCoord;
    o_ViewNormal = normalMatrix * inNormal;
    o_ViewPosition = modelView * vec4(inPosition, 1.0);

    gl_Position = GLOBAL.CAMERA.viewProj * PUSH_CONSTANT.model * vec4(inPosition, 1.0);
}