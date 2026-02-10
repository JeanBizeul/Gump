#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

uniform vec2 uUVMin;
uniform vec2 uUVMax;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);

    // Map from [0,1] to the actual UV region
    vTexCoord = mix(uUVMin, uUVMax, aTexCoord);
}
