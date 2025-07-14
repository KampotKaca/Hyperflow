#version 450

const vec3 GAMMA_CORRECTION = vec3(1.0 / 2.2);

layout(set = 0, binding = 0) uniform sampler2D fullscreenTexture;

layout(location = 0) in vec2 outTexcoord;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(pow(texture(fullscreenTexture, outTexcoord).rgb, GAMMA_CORRECTION), 1.0);
}