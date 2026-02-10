#version 330 core

in vec2 vTexCoord;
in float vOpacity;

out vec4 FragColor;

uniform sampler2D uBrushTexture;  // Brush shape/texture
uniform vec4 uBrushColor;
uniform float uHardness;
uniform bool uEraser;

void main()
{
    // Sample brush texture (typically a radial gradient or custom shape)
    float brushAlpha = texture(uBrushTexture, vTexCoord).r;

    // Apply hardness (makes brush sharper or softer)
    brushAlpha = pow(brushAlpha, 1.0 / max(uHardness, 0.01));

    // Apply opacity
    brushAlpha *= vOpacity;

    if (uEraser) {
        // Eraser mode: output alpha to subtract
        FragColor = vec4(0.0, 0.0, 0.0, brushAlpha);
    } else {
        // Paint mode: output color with alpha
        FragColor = vec4(uBrushColor.rgb, brushAlpha * uBrushColor.a);
    }
}