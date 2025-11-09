#name phonglighting

struct PhongLightInfo
{
    float ambient;
    float smoothness;
    vec3 specularColor;
};

vec3 GetPhongLighting(vec3 albedo, vec3 viewPos, vec3 viewNormal, PhongLightInfo info)
{
    vec3 n = normalize(viewNormal);
    vec3 v = normalize(-viewPos);
    vec3 result = albedo * info.ambient;
    float shininess = mix(1.0, 256.0, info.smoothness);

    //Directional Lights
    for (uint i = 0; i < GLOBAL.LIGHT.lightCounts.x; i++)
    {
        DirectionalLight light = GLOBAL.LIGHT.directionalLights[i];
        float diff = max(dot(n, -light.direction), 0.0);

        vec3 r = reflect(light.direction, n);
        float spec = (diff > 0.0) ? pow(max(dot(r, v), 0.0), shininess) : 0.0;
        result += (albedo * diff + info.specularColor * spec) * light.color;
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