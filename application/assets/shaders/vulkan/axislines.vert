#version 450

layout(location = 0) in vec2 inPosition;

layout(location = 0) out vec2 o_NDCCoord;

void main()
{
    gl_Position = vec4(inPosition, 0, 1.0);
    o_NDCCoord = inPosition;
}