#version 330 core

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform float uRadius;
uniform int uQuality;

// Simple box blur with adjustable radius and quality
void main()
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    vec4 result = vec4(0.0);
    float totalWeight = 0.0;
    
    // Sample in a circular pattern
    for (int x = -uQuality; x <= uQuality; x++) {
        for (int y = -uQuality; y <= uQuality; y++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize * uRadius;
            float dist = length(vec2(x, y));
            
            // Only sample within circular radius
            if (dist <= float(uQuality)) {
                // Gaussian-like falloff
                float weight = exp(-dist * dist / (2.0 * float(uQuality) * float(uQuality)));
                result += texture(uTexture, vTexCoord + offset) * weight;
                totalWeight += weight;
            }
        }
    }
    
    FragColor = result / totalWeight;
}
