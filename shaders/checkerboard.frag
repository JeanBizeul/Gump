#version 330 core

in vec2 WorldPos;

out vec4 FragColor;

uniform vec2 uCanvasSize;

void main()
{
    // Checkerboard pattern (16x16 pixel squares)
    float checkerSize = 16.0;
    vec2 coord = floor(WorldPos / checkerSize);
    float pattern = mod(coord.x + coord.y, 2.0);

    // Alternate between light gray and white
    vec3 color1 = vec3(0.8, 0.8, 0.8); // Light gray
    vec3 color2 = vec3(1.0, 1.0, 1.0); // White

    vec3 color = mix(color1, color2, pattern);
    FragColor = vec4(color, 1.0);
}
