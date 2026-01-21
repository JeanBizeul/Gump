#version 330 core

in vec2 UV;
in vec2 WorldPos;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform float uTransparency;
uniform vec2 uCanvasSize;

void main()
{
    // Discard fragments outside canvas bounds
    if (WorldPos.x < 0.0 || WorldPos.x > uCanvasSize.x ||
        WorldPos.y < 0.0 || WorldPos.y > uCanvasSize.y) {
        discard;
    }
    
    vec4 texColor = texture(uTexture, UV);
    FragColor = vec4(texColor.rgb, texColor.a * uTransparency);
}
