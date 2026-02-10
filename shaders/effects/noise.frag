#version 330 core

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform float uAmount;
uniform int uNoiseType;
uniform float uSeed;

// Random number generator
float random(vec2 st) {
    return fract(sin(dot(st.xy + vec2(uSeed, uSeed),
                         vec2(12.9898, 78.233))) * 43758.5453123);
}

void main()
{
    vec4 color = texture(uTexture, vTexCoord);
    
    if (uNoiseType == 0) {
        // White noise (grayscale)
        float noise = random(vTexCoord);
        vec3 noisyColor = mix(color.rgb, vec3(noise), uAmount);
        FragColor = vec4(noisyColor, color.a);
    } else {
        // Colored noise (RGB)
        float noiseR = random(vTexCoord);
        float noiseG = random(vTexCoord + vec2(0.123, 0.456));
        float noiseB = random(vTexCoord + vec2(0.789, 0.012));
        vec3 noise = vec3(noiseR, noiseG, noiseB);
        
        vec3 noisyColor = mix(color.rgb, noise, uAmount);
        FragColor = vec4(noisyColor, color.a);
    }
}
