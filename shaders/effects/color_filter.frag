#version 330 core

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 uFilterColor;
uniform float uIntensity;
uniform int uBlendMode;

vec3 multiply(vec3 base, vec3 blend) {
    return base * blend;
}

vec3 screen(vec3 base, vec3 blend) {
    return 1.0 - (1.0 - base) * (1.0 - blend);
}

vec3 overlay(vec3 base, vec3 blend) {
    vec3 result;
    for (int i = 0; i < 3; i++) {
        if (base[i] < 0.5) {
            result[i] = 2.0 * base[i] * blend[i];
        } else {
            result[i] = 1.0 - 2.0 * (1.0 - base[i]) * (1.0 - blend[i]);
        }
    }
    return result;
}

void main()
{
    vec4 color = texture(uTexture, vTexCoord);
    vec3 filtered;
    
    // Apply blend mode
    if (uBlendMode == 0) {
        // Multiply
        filtered = multiply(color.rgb, uFilterColor.rgb);
    } else if (uBlendMode == 1) {
        // Screen
        filtered = screen(color.rgb, uFilterColor.rgb);
    } else {
        // Overlay
        filtered = overlay(color.rgb, uFilterColor.rgb);
    }
    
    // Mix between original and filtered based on intensity
    vec3 result = mix(color.rgb, filtered, uIntensity);
    
    FragColor = vec4(result, color.a);
}
