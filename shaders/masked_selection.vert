#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uProjectionView;

out vec2 vTexCoord;
out vec2 vPosition;

void main()
{
    gl_Position = uProjectionView * vec4(aPos, 1.0);
    vTexCoord = aTexCoord;
    vPosition = aPos.xy;
}
