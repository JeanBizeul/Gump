#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uProjectionView;

void main()
{
    gl_Position = uProjectionView * uModel * vec4(aPos, 1.0);
}
