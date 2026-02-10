#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in float aPressure;

uniform mat4 uProjectionView;
uniform float uBrushSize;

out float vPressure;
out vec2 vWorldPos;  // Pass world position to fragment shader

void main()
{
    vPressure = aPressure;
    vWorldPos = aPosition;  // Store world position for global pattern

    gl_PointSize = uBrushSize * aPressure;

    gl_Position = uProjectionView * vec4(aPosition, 0.0, 1.0);
}
