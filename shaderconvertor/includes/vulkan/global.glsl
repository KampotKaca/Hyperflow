#name global

struct Camera
{
    vec3 lookDirection;
    vec3 position;
    mat4 view;
    mat4 invView;
    mat4 proj;
    mat4 invProj;
    mat4 viewProj;
};

struct Time
{
    double deltaTime;
    double timeSinceStartup;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 color;
    vec3 rangeStats; //x innerAngle, y outerAngle, z range
};

struct PointLight
{
    vec3 position;
    vec4 color; // w is range
};

const uint MAX_DIRECTIONAL_LIGHTS = 4;
const uint MAX_SPOT_LIGHTS = 8;
const uint MAX_POINT_LIGHTS = 8;

struct Light
{
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];
    uvec3 lightCounts; //x directional, y spot, z point
};

layout(set = 0, binding = 0) uniform Global
{
    Camera CAMERA;
    Time TIME;
    Light LIGHT;
} GLOBAL;

vec3 GetPhongLighting(vec3 position, vec3 normal, float smoothness)
{
    vec3 outputColor = vec3(0.5f, 0.5f, 0.5f);
    for (uint i = 0; i < GLOBAL.LIGHT.lightCounts.x; i++)
    {
        DirectionalLight light = GLOBAL.LIGHT.directionalLights[i];
        vec3 r = reflect(-light.direction, normal);
        float diff = max(dot(normal, -light.direction), 0.0);
        float spec = (diff > 0.0) ? pow(max(dot(r, -position), 0.0), smoothness) : 0.0;
        outputColor += (diff + spec) * light.color;
    }

    for (uint i = 0; i < GLOBAL.LIGHT.lightCounts.y; i++)
    {

    }

    for (uint i = 0; i < GLOBAL.LIGHT.lightCounts.z; i++)
    {

    }
    return outputColor;
}

const uint MAX_MATERIAL_SIZE = 1024;