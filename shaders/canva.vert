#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

out vec2 UV;
out vec2 WorldPos;

uniform mat4 uProjectionView;

void main()
{
    UV = aUV;
    WorldPos = aPos.xy;
    gl_Position = uProjectionView * vec4(aPos, 1.0);
}
