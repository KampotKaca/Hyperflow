#version 450

layout(set = 0, binding = 0) uniform Camera
{
    vec3 lookDirection;
    vec3 position;
    mat4 view;
    mat4 invView;
    mat4 proj;
    mat4 invProj;
    mat4 viewProj;

    mat4 padding0;
    mat4 padding1;
    vec4 padding2;
    vec4 padding3;
} CAMERA;

layout (set = 0, binding = 1) uniform Time
{
    double deltaTime;
    double timeSinceStartup;
} TIME;

layout(push_constant) uniform PushConstants
{
    vec4 planeNormal;
    vec4 color;
} PUSH_CONSTANT;

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

bool GetGridColor(float dist, vec2 absIntersection, float scale, float opacity, out vec4 o_Color)
{
    vec2 fraction = (absIntersection - ivec2(absIntersection * scale) / scale) * scale;
    vec2 smoothWidth = PUSH_CONSTANT.planeNormal.w * fwidth(fraction);

    vec2 lines = smoothstep(vec2(0.0), smoothWidth, fraction) *
    smoothstep(vec2(0.0), smoothWidth, vec2(1.0) - fraction);

    if(min(lines.x, lines.y) < 0.5)
    {
        o_Color = PUSH_CONSTANT.color / (1 + dist * opacity);
        return true;
    }
    return false;
}

void main()
{
    vec3 intersection;
    if(RayIntersectsPlane(CAMERA.position, o_RayWorld, vec3(PUSH_CONSTANT.planeNormal), CENTER, intersection))
    {
        float dist = length(intersection - CAMERA.position);
        vec2 absIntersection = abs(intersection.xz);

        if(GetGridColor(dist, absIntersection, 0.01, 0.01, outColor)) return;
        if(GetGridColor(dist, absIntersection, 0.1, 0.075, outColor)) return;
        if(GetGridColor(dist, absIntersection, 1, 0.25, outColor)) return;
    }

    discard;
}