#version 450

layout(location = 0) in vec4 o_FragColor;
layout(location = 1) in vec2 o_TexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(texSampler, o_TexCoord) + o_FragColor * .3f;
}