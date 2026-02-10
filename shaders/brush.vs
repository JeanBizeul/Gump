#version 330 core
layout (location = 0) in vec3 aPos;      // Quad position
layout (location = 1) in vec2 aTexCoord; // Quad UV

// Per-instance data (one dab per instance)
layout (location = 2) in vec2 aDabPosition;  // World position of brush dab
layout (location = 3) in float aDabSize;     // Size of this dab
layout (location = 4) in float aDabOpacity;  // Opacity of this dab

out vec2 vTexCoord;
out float vOpacity;

uniform mat4 uProjectionView;

void main()
{
    // Scale quad by dab size and translate to dab position
    vec3 worldPos = vec3(aPos.xy * aDabSize + aDabPosition, 0.0);
    gl_Position = uProjectionView * vec4(worldPos, 1.0);

    vTexCoord = aTexCoord;
    vOpacity = aDabOpacity;
}