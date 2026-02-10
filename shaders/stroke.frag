#version 330 core

in float vPressure;

out vec4 FragColor;

uniform sampler2D uBrushTexture;
uniform vec4 uBrushColor;
uniform float uBrushOpacity;
uniform float uBrushHardness;
uniform int uBrushShape; // 0 = circle, 1 = square

void main()
{
    // gl_PointCoord gives us UV coordinates for the point sprite (0,0 to 1,1)
    vec2 uv = gl_PointCoord;

    float alpha = 0.0;

    if (uBrushShape == 0) {
        // Circle brush (procedural)
        vec2 center = vec2(0.5, 0.5);
        float dist = length(uv - center) * 2.0; // 0 at center, 1 at edge

        if (uBrushHardness >= 1.0) {
            // Hard circle - sharp edge
            alpha = step(dist, 1.0);
        } else {
            // Soft circle - smooth falloff
            float edge = mix(0.0, 1.0, uBrushHardness);
            alpha = 1.0 - smoothstep(edge, 1.0, dist);
        }
    } else if (uBrushShape == 1) {
        // Square brush (procedural)
        vec2 center = vec2(0.5, 0.5);
        vec2 d = abs(uv - center) * 2.0; // Distance from center in each axis
        float dist = max(d.x, d.y); // Box distance (0 at center, 1 at edge)

        if (uBrushHardness >= 1.0) {
            // Hard square - sharp edge
            alpha = step(dist, 1.0);
        } else {
            // Soft square - smooth falloff
            float edge = mix(0.0, 1.0, uBrushHardness);
            alpha = 1.0 - smoothstep(edge, 1.0, dist);
        }
    } else {
        // Texture-based brush (fallback to original behavior)
        vec4 brushSample = texture(uBrushTexture, uv);
        alpha = brushSample.a;

        if (uBrushHardness < 1.0) {
            vec2 center = vec2(0.5, 0.5);
            float dist = length(uv - center) * 2.0;
            float softAlpha = 1.0 - smoothstep(0.0, 1.0, dist);
            alpha = mix(softAlpha, brushSample.a, uBrushHardness);
        }
    }

    // Apply brush color and opacity
    vec3 color = uBrushColor.rgb;
    alpha *= uBrushColor.a * uBrushOpacity * vPressure;

    FragColor = vec4(color, alpha);
}