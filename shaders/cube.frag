#version 330 core

uniform vec3 uCubeColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(uCubeColor, 1.0);
}
