#ifndef __MATERIAL_GLSL
#define __MATERIAL_GLSL

layout(std430, set = 1, binding = 0) buffer MaterialBuffer
{
    Material data[];
} MATERIAL_BUFFER;

#endif