//#name phonglighting

struct PhongLightInfo
{
    float ambient;
    float smoothness;
    vec3 specularColor;
};

vec3 GetPhongLighting(vec3 albedo, vec3 wNormal, PhongLightInfo info)
{
    float intensity = 0;
    vec3 result = albedo * info.ambient;

    //Directional Lights
    for (uint i = 0; i < GLOBAL.LIGHT.lightCounts.x; i++)
    {
        DirectionalLight light = GLOBAL.LIGHT.directionalLights[i];
        float intensity = max(dot(wNormal, light.direction), 0);
        result += info.smoothness * intensity * light.color;
    }

    //Spot Lights
    for (uint i = 0; i < GLOBAL.LIGHT.lightCounts.y; i++)
    {
        SpotLight light = GLOBAL.LIGHT.spotLights[i];
    }

    //Point Lights
    for (uint i = 0; i < GLOBAL.LIGHT.lightCounts.z; i++)
    {
        PointLight light = GLOBAL.LIGHT.pointLights[i];
    }
    return result;
}