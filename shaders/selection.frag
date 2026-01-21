#version 330 core

in vec2 vTexCoord;
in vec2 vPosition;

out vec4 FragColor;

uniform float uTime;
uniform vec2 uSelectionSize; // Width and height of selection in pixels
uniform float uBorderWidth; // Width and height of selection in pixels

void main()
{
    // Calculate pixel position within selection
    vec2 pixelPos = vTexCoord * uSelectionSize;

    // Calculate distance from each edge
    float distLeft = pixelPos.x;
    float distRight = uSelectionSize.x - pixelPos.x;
    float distTop = pixelPos.y;
    float distBottom = uSelectionSize.y - pixelPos.y;

    // Get minimum distance to any edge
    float distToEdge = min(min(distLeft, distRight), min(distTop, distBottom));

    // Only draw if we're within uBorderWidth pixels of an edge
    if (distToEdge > uBorderWidth) {
        discard; // Don't draw interior
    }

    // Marching ants animation - scale with zoom (via uBorderWidth)
    // When zoomed in, uBorderWidth increases, making ants larger and slower
    // When zoomed out, uBorderWidth decreases, making ants smaller and faster
    float dashLength = uBorderWidth * 1.6; // Dash length scales with border width
    float dashSpeed = uBorderWidth * 6.0;  // Speed scales with border width

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
