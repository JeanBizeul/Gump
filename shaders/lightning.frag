#version 330 core

uniform vec3 uCubeColor;
uniform vec3 uLightColor;
uniform vec3 uLightPos;
uniform float uShininess;
uniform float uSpecStrenght;

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

void main()
{
    float ambiantStrenght = 0.1;
    vec3 ambiant = ambiantStrenght * uLightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    vec3 viewPos = vec3(3.0f, 3.0f, 6.0f);
    float specularStrength = uSpecStrenght;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    vec3 specular = specularStrength * spec * uLightColor;

    vec3 result = (specular + ambiant + diffuse) * uCubeColor;
    FragColor = vec4(result, 1.0);
}

