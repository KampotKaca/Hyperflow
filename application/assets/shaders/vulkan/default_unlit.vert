#version 450
#include <global.glsl>

layout(location = 0) in vec3 inPosition;

layout(location = 8)  in vec4 instance_model_col0;
layout(location = 9)  in vec4 instance_model_col1;
layout(location = 10) in vec4 instance_model_col2;
layout(location = 11) in vec4 instance_model_col3;
layout(location = 12) in vec4 instance_albedo;

layout(location = 0) out vec3 o_Color;

void main()
{
    mat4 modelMatrix = mat4(instance_model_col0, instance_model_col1, instance_model_col2, instance_model_col3);

    gl_Position = GLOBAL.CAMERA.viewProj * modelMatrix * vec4(inPosition, 1.0);
    o_Color = vec3(instance_albedo);
}