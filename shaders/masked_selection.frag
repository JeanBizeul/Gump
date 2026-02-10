#version 330 core

in vec2 vTexCoord;
in vec2 vPosition;

out vec4 FragColor;

uniform float uTime;
uniform sampler2D uMaskTexture;  // The selection mask texture
uniform vec2 uMaskSize;          // Size of the mask in pixels
uniform float uBorderWidth;      // Width of the border

void main()
{
    // Sample the mask at this position
    float maskValue = texture(uMaskTexture, vTexCoord).r;

    // If this pixel is not selected, discard it
    if (maskValue < 0.5) {
        discard;
    }

    // Calculate pixel position within selection
    vec2 pixelPos = vTexCoord * uMaskSize;

    // Check neighboring pixels to detect edges
    vec2 texelSize = 1.0 / uMaskSize;
    float left   = texture(uMaskTexture, vTexCoord + vec2(-texelSize.x, 0.0)).r;
    float right  = texture(uMaskTexture, vTexCoord + vec2(texelSize.x, 0.0)).r;
    float top    = texture(uMaskTexture, vTexCoord + vec2(0.0, -texelSize.y)).r;
    float bottom = texture(uMaskTexture, vTexCoord + vec2(0.0, texelSize.y)).r;

    // If all neighbors are selected, this is an interior pixel - discard it
    if (left > 0.5 && right > 0.5 && top > 0.5 && bottom > 0.5) {
        discard;
    }

    // This is an edge pixel - draw marching ants
    float dashLength = uBorderWidth * 1.6;
    float dashSpeed = uBorderWidth * 6.0;

    // Calculate position along perimeter for animation
    float perimeter = pixelPos.x + pixelPos.y;
    float dash = mod(perimeter + uTime * dashSpeed, dashLength * 2.0);
    float dashPattern = step(dashLength, dash);

    // Alternate colors for marching ants
    vec3 color1 = vec3(0.0, 0.0, 0.0); // Black
    vec3 color2 = vec3(1.0, 1.0, 1.0); // White

    vec3 color = mix(color1, color2, dashPattern);
    FragColor = vec4(color, 1.0);
}
