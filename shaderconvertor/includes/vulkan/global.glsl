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

layout(set = 0, binding = 0) uniform Global
{
    Camera CAMERA;
    Time TIME;
} GLOBAL;

const uint MAX_MATERIAL_SIZE = 1024;