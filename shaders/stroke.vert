#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in float aPressure;

uniform mat4 uProjectionView;
uniform float uBrushSize;

out float vPressure;

void main()
{
    vPressure = aPressure;

    gl_PointSize = uBrushSize * aPressure;

    gl_Position = uProjectionView * vec4(aPosition, 0.0, 1.0);
}
