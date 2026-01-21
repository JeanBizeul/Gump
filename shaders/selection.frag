#version 330 core

out vec4 FragColor;

void main()
{
    // Scrolling diagonal stripes pattern
    float stripeWidth = 20.0;
    float speed = 0.5;
    float time = mod(gl_FragCoord.y * speed + gl_FragCoord.x, stripeWidth * 2.0);
    float pattern = step(stripeWidth, time);

    // Alternate between light blue and dark blue
    vec3 color1 = vec3(0.4, 0.6, 0.9); // Light blue
    vec3 color2 = vec3(0.1, 0.2, 0.5); // Dark blue

    vec3 color = mix(color1, color2, pattern);
    FragColor = vec4(color, 1.0);
}
