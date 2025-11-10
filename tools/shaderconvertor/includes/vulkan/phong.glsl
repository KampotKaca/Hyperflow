//#name phonglighting

struct PhongLightInfo
{
    float ambient;
    float smoothness;
    vec3 specularColor;
};

vec3 GetPhongLighting(vec3 color, vec3 viewPos, vec3 fragPos, vec3 wNormal, PhongLightInfo info)
{
    vec3 result;
    vec3 viewDir = normalize(viewPos - fragPos);
    //Directional Lights
    for (uint i = 0; i < GLOBAL.LIGHT.lightCounts.x; i++)
    {
        DirectionalLight light = GLOBAL.LIGHT.directionalLights[i];

        float diff = max(dot(light.direction, wNormal), 0.0);

        vec3 halfwayDir = normalize(light.direction + viewDir);
        float spec = pow(max(dot(wNormal, halfwayDir), 0.0), info.smoothness * 32.0);

        vec3 ambient = info.ambient * color;
        vec3 diffuse = diff * color;
        vec3 specular = spec * light.color;

        result += ambient + diffuse + specular;
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