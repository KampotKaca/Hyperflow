#version 450
#include <global.glsl>
#include <phong.glsl>

layout(push_constant) uniform PushConstants
{
    vec4 phongData; //xyz specular color, w smoothness
} PUSH_CONSTANT;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 8)  in vec4 instance_model_col0;
layout(location = 9)  in vec4 instance_model_col1;
layout(location = 10) in vec4 instance_model_col2;
layout(location = 11) in vec4 instance_model_col3;
layout(location = 12) in vec4 instance_normal_col0;
layout(location = 13) in vec4 instance_normal_col1;
layout(location = 14) in vec4 instance_normal_col2;
layout(location = 15) in vec4 instance_albedo;

layout(location = 0) out vec2 o_TexCoord;
layout(location = 1) out vec3 o_Color;
layout(location = 2) out vec3 o_WNormal;
layout(location = 3) out vec3 o_FragPosition;

void main()
{
    mat4 modelMatrix = mat4(instance_model_col0, instance_model_col1, instance_model_col2, instance_model_col3);
    mat3 normalMatrix = mat3(instance_normal_col0.xyz, instance_normal_col1.xyz, instance_normal_col2.xyz);

    mat4 modelView    = GLOBAL.CAMERA.view * modelMatrix;
    vec4 pos = GLOBAL.CAMERA.viewProj * modelMatrix * vec4(inPosition, 1.0);

    o_Color = vec3(instance_albedo);
    o_TexCoord = inTexCoord;
    o_WNormal = normalize(normalMatrix * inNormal);
    o_FragPosition = pos.xyz;

    gl_Position = pos;
}