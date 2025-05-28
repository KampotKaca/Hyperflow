#version 450

layout(location = 0) in int inPosition;

void main()
{
    ivec2 pos = ivec2(inPosition >> 16, inPosition & ((1 << 16) - 1));
    gl_Position = vec4(pos.x, pos.y, 0, 0);
}