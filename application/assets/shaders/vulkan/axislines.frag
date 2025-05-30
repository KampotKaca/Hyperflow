#version 450

layout(set = 0, binding = 0) uniform Camera
{
    mat4 model;

    vec3 lookDirection;
    vec3 position;
    mat4 view;
    mat4 invView;
    mat4 proj;
    mat4 invProj;
    mat4 viewProj;
} CAMERA;

layout (set = 0, binding = 1) uniform Time
{
    double deltaTime;
    double timeSinceStartup;
} TIME;

layout(set = 1, binding = 0) uniform AxisLineUniform
{
    vec4 planeNormal; //w component is thickness
    vec4 color;
} AXIS_LINES;

layout(location = 0) in vec2 o_NDCCoord;
layout(location = 0) out vec4 outColor;

const vec3 CENTER = vec3(0, 0, 0);

bool RayIntersectsPlane(vec3 rayOrigin, vec3 rayDir, vec3 planeNormal, vec3 planePoint, out vec3 o_Intersection)
{
    float denom = dot(planeNormal, rayDir);
    if (abs(denom) < 1e-6) return false;

    float t = dot(planePoint - rayOrigin, planeNormal) / denom;
    if(t < 0) return false;
    o_Intersection = rayOrigin + t * rayDir;
    return true;
}

void main()
{
    vec4 clipPos = vec4(o_NDCCoord, -1.0, 1.0);
    vec4 viewPos = CAMERA.invProj * clipPos;
    viewPos /= viewPos.w;
    vec3 rayDirView = normalize(viewPos.xyz);

    vec3 rayDirWorld = normalize(mat3(CAMERA.invView) * rayDirView);

    vec3 intersection;
    if(RayIntersectsPlane(CAMERA.position, rayDirWorld, vec3(AXIS_LINES.planeNormal), CENTER, intersection))
    {
        vec3 delta = intersection - CAMERA.position;
        float dist = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        float thickness = clamp(abs(CAMERA.position.y), 3.0, 10.0) * 0.1 * AXIS_LINES.planeNormal.w;
        vec2 absIntersection = abs(intersection.xz);
        vec2 fraction = absIntersection - floor(absIntersection);

        if(fraction.x < thickness || fraction.x > (1.0 - thickness) ||
           fraction.y < thickness || fraction.y > (1.0 - thickness))
        {
//            outColor = AXIS_LINES.color * exp(-dist * 0.05);
            outColor = AXIS_LINES.color * 1 / (1 + dist * 0.005);
            return;
        }
    }

    discard;
}