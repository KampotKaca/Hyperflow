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

    mat4 padding0;
    vec4 padding1;
    vec4 padding2;
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

layout(location = 0) in vec3 o_RayWorld;
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
    vec3 intersection;
    if(RayIntersectsPlane(CAMERA.position, o_RayWorld, vec3(AXIS_LINES.planeNormal), CENTER, intersection))
    {
        vec3 delta = intersection - CAMERA.position;
        float dist = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        vec2 absIntersection = abs(intersection.xz);

        vec2 fraction = absIntersection - floor(absIntersection);

        vec2 smoothWidth = AXIS_LINES.planeNormal.w * fwidth(fraction);

        vec2 lines = smoothstep(vec2(0.0), smoothWidth, fraction) *
        smoothstep(vec2(0.0), smoothWidth, vec2(1.0) - fraction);

        if(min(lines.x, lines.y) < 0.5)
        {
            outColor = AXIS_LINES.color / (1 + dist * 0.005);
            return;
        }
    }

    discard;
}