#version 330 core

in float vPressure;

out vec4 FragColor;

uniform sampler2D uBrushTexture;
uniform vec4 uBrushColor;
uniform float uBrushOpacity;
uniform float uBrushHardness;

void main()
{
    // gl_PointCoord gives us UV coordinates for the point sprite (0,0 to 1,1)
    vec2 uv = gl_PointCoord;

    // Sample the brush texture
    vec4 brushSample = texture(uBrushTexture, uv);

    // Apply hardness by modulating the alpha
    // Hardness controls how sharp the brush edges are
    float alpha = brushSample.a;
    if (uBrushHardness < 1.0) {
        // Calculate distance from center for soft brush
        vec2 center = vec2(0.5, 0.5);
        float dist = length(uv - center) * 2.0; // 0 at center, 1 at edge

        // Mix between hard edge (brushSample.a) and soft edge
        float softAlpha = 1.0 - smoothstep(0.0, 1.0, dist);
        alpha = mix(softAlpha, brushSample.a, uBrushHardness);
    }

    // Apply brush color and opacity
    vec3 color = uBrushColor.rgb;
    alpha *= uBrushColor.a * uBrushOpacity * vPressure;

    // If using an eraser, we'd output different blending here
    // For now, just output the brush color with calculated alpha
    FragColor = vec4(color, alpha);
}