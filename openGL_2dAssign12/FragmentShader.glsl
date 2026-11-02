#version 330 core

in vec3 outColor;
out vec4 FragColor;
uniform bool uUseOverrideColor;
uniform vec3 uOverrideColor;

void main() {
    vec3 color = uUseOverrideColor ? uOverrideColor : outColor;
    FragColor = vec4(color, 1.0);
}
