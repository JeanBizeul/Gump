#version 330 core

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform float uIntensity;

void main()
{
    vec4 color = texture(uTexture, vTexCoord);

    // Invert RGB channels, keep alpha
    vec3 inverted = vec3(1.0) - color.rgb;

    // Mix between original and inverted based on intensity
    vec3 result = mix(color.rgb, inverted, uIntensity);

    FragColor = vec4(result, color.a);
}
