#ifndef __PHONG_GLSL
#define __PHONG_GLSL

struct PhongLightInfo
{
    float ambient;
    float smoothness;
    vec3 ambientColor;
};

vec3 SolvePhongDirectionalLights(vec3 color, vec3 viewPos, vec3 wPosition, vec3 wNormal, float smoothness)
{
    vec3 result = vec3(0, 0, 0);
    vec3 viewDir = normalize(viewPos - wPosition);

    for (uint i = 0; i < GLOBAL.LIGHT.lightCounts.x; i++)
    {
        DirectionalLight light = GLOBAL.LIGHT.directionalLights[i];

        float diff = max(dot(light.direction, wNormal), 0.0);

        vec3 halfwayDir = normalize(light.direction + viewDir);
        float spec = pow(max(dot(wNormal, halfwayDir), 0.0), smoothness * 128.0);

        vec3 diffuse = diff * color;
        vec3 specular = spec * light.color;

        result += diffuse + specular;
    }

    return result;
}

vec3 SolvePhongPointLights(vec3 color, vec3 wPosition, vec3 wNormal)
{
    vec3 result = vec3(0, 0, 0);
    for (uint i = 0; i < GLOBAL.LIGHT.lightCounts.z; i++)
    {
        PointLight light = GLOBAL.LIGHT.pointLights[i];
        vec3 dirToLight = light.position.xyz - wPosition;
        float atten = 1.0 / dot(dirToLight, dirToLight);
        vec3 diffuseLight = light.color.rgb * atten * max(dot(wNormal, normalize(dirToLight)), 0.0);
        result += (diffuseLight) * color;
    }
    return result;
}

vec3 SolvePhongLights(vec3 color, vec3 viewPos, vec3 wPosition, vec3 wNormal, PhongLightInfo info)
{
    vec3 resut = info.ambient * info.ambientColor;
    resut += SolvePhongDirectionalLights(color, viewPos, wPosition, wNormal, info.smoothness);
    resut += SolvePhongPointLights(color, wPosition, wNormal);
    return resut;
}

#endif