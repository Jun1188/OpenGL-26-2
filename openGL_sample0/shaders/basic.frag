#version 330 core

in vec3 vWorldNormal;

out vec4 FragColor;

// Direction in which the light travels, expressed in world space.
uniform vec3 uLightDirection;

void main()
{
    vec3 normal = normalize(vWorldNormal);
    vec3 toLight = normalize(-uLightDirection);
    float diffuse = max(dot(normal, toLight), 0.0);
    // Lambert diffuse only: black (0.0) through base gray (0.65).
    // Equal RGB channels keep the result achromatic. No ambient/specular term.
    float brightness = 0.65 * diffuse;
    FragColor = vec4(vec3(brightness), 1.0);
}
