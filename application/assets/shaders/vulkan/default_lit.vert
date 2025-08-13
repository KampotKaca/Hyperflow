#version 450
#include "global"

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
layout(location = 12) in vec4 instance_albedo;

layout(location = 0) out vec2 o_TexCoord;
layout(location = 1) out vec3 o_Color;
layout(location = 2) out vec3 o_ViewNormal;
layout(location = 3) out vec4 o_ViewPosition;

void main()
{
    mat4 modelMatrix = mat4(instance_model_col0, instance_model_col1, instance_model_col2, instance_model_col3);

    mat4 modelView    = GLOBAL.CAMERA.view * modelMatrix;
    mat3 normalMatrix = transpose(inverse(mat3(modelView)));

    o_Color = vec3(instance_albedo);
    o_TexCoord = inTexCoord;
    o_ViewNormal = normalMatrix * inNormal;
    o_ViewPosition = modelView * vec4(inPosition, 1.0);

    gl_Position = GLOBAL.CAMERA.viewProj * modelMatrix * vec4(inPosition, 1.0);
}