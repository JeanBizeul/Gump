#version 330 core

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform float uAmount;
uniform int uNoiseType;
uniform float uSeed;

// Improved hash function for better randomness
float hash(vec2 p) {
    p = fract(p * vec2(123.456, 789.123));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

// 2D Value noise function
float valueNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    // Smooth interpolation
    f = f * f * (3.0 - 2.0 * f);

    // Four corners of the grid cell
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    // Bilinear interpolation
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// Perlin-like noise with multiple octaves for more detail
float perlinNoise(vec2 p) {
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;

    // Add multiple octaves for more natural noise
    for (int i = 0; i < 4; i++) {
        value += amplitude * valueNoise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }

    return value;
}

// Simple white noise (but better than before)
float whiteNoise(vec2 p) {
    return hash(p);
}

void main()
{
    vec4 color = texture(uTexture, vTexCoord);

    // Get texture dimensions for pixel-perfect noise
    vec2 texSize = vec2(textureSize(uTexture, 0));
    vec2 pixelCoord = vTexCoord * texSize;

    // Add seed offset for variation
    vec2 seedOffset = vec2(uSeed * 0.1, uSeed * 0.13);

    if (uNoiseType == 0) {
        // White noise (grayscale) - using better random function
        float noise = whiteNoise(pixelCoord + seedOffset);
        vec3 noisyColor = mix(color.rgb, vec3(noise), uAmount);
        FragColor = vec4(noisyColor, color.a);
    } else {
        // Colored noise (RGB) - using Perlin-like noise for more natural look
        float noiseR = perlinNoise((pixelCoord + seedOffset) * 0.5);
        float noiseG = perlinNoise((pixelCoord + seedOffset + vec2(123.45, 678.90)) * 0.5);
        float noiseB = perlinNoise((pixelCoord + seedOffset + vec2(456.78, 901.23)) * 0.5);
        vec3 noise = vec3(noiseR, noiseG, noiseB);

        vec3 noisyColor = mix(color.rgb, noise, uAmount);
        FragColor = vec4(noisyColor, color.a);
    }
}
