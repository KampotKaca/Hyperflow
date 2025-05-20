#version 450

layout(location = 0) in vec2 o_TexCoord;
layout(location = 1) in vec3 o_Color;

layout(set = 1, binding = 0) uniform sampler2D mainTex;
layout(set = 1, binding = 1) uniform sampler2D overTex;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(overTex, o_TexCoord) * vec4(o_Color, 1.0f) + texture(mainTex, o_TexCoord) * .3f;
}