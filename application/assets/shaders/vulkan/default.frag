#version 450

layout(location = 0) in vec2 o_TexCoord;
layout(location = 1) in vec3 o_Color;

layout(set = 1, binding = 0) uniform sampler2D textures[2];
//layout(set = 1, binding = 1) uniform sampler2D overTex;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(textures[1], o_TexCoord) * vec4(o_Color, 1.0f) + texture(textures[0], o_TexCoord) * .3f;
}