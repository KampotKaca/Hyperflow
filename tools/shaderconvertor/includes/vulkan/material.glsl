//#name material

layout(std430, set = 1, binding = 0) buffer MaterialBuffer
{
    Material data[];
} MATERIAL_BUFFER;