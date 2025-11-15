#ifndef __BLINN_PHONG_GLSL
#define __BLINN_PHONG_GLSL

#include <__global.glsl>

struct PhongLightInfo
{
    float ambient;
    float smoothness;
    vec3 ambientColor;
};

const float HIGHLIGHT_SHARPNESS = 512.0;

vec3 SolvePhongDirectionalLights(vec3 wPosition, vec3 wNormal, vec3 viewDir, float smoothness)
{
    vec3 result = vec3(0);

    for (uint i = 0; i < GLOBAL.LIGHT.lightCounts.x; i++)
    {
        DirectionalLight light = GLOBAL.LIGHT.directionalLights[i];

        vec3 dirToLight = -light.direction.xyz;
        float cosAngIncidence = max(dot(wNormal, dirToLight), 0.0);
        vec3 intensity = light.color.rgb * light.color.a;

        vec3 halfAngle = normalize(dirToLight + viewDir);
        float blinnTerm = pow(clamp(dot(wNormal, halfAngle), 0, 1), max(smoothness * HIGHLIGHT_SHARPNESS, 1.0));

        result += (blinnTerm + cosAngIncidence) * intensity;
    }

    return result;
}

vec3 SolvePhongPointLights(vec3 wPosition, vec3 wNormal, vec3 viewDir, float smoothness)
{
    vec3 result = vec3(0);
    for (uint i = 0; i < GLOBAL.LIGHT.lightCounts.z; i++)
    {
        PointLight light = GLOBAL.LIGHT.pointLights[i];
        vec3 dirToLight = light.position.xyz - wPosition;
        float atten = 1.0 / dot(dirToLight, dirToLight);
        dirToLight = normalize(dirToLight);
        float cosAngIncidence = max(dot(wNormal, dirToLight), 0.0);
        vec3 intensity = light.color.rgb * light.color.a * atten;

        vec3 halfAngle = normalize(dirToLight + viewDir);
        float blinnTerm = pow(clamp(dot(wNormal, halfAngle), 0, 1), max(smoothness * HIGHLIGHT_SHARPNESS, 1.0));

        result += (blinnTerm + cosAngIncidence) * intensity;
    }
    return result;
}

vec3 SolvePhongLights(vec3 wPosition, vec3 wNormal, PhongLightInfo info)
{
    vec3 viewDir = normalize(GLOBAL.CAMERA.position - wPosition);
    vec3 resut = info.ambient * info.ambientColor;
    resut += SolvePhongDirectionalLights(wPosition, wNormal, viewDir, info.smoothness);
    resut += SolvePhongPointLights(wPosition, wNormal, viewDir, info.smoothness);
    return resut;
}

#endif