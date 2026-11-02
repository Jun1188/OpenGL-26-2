#version 330 core

in vec3 vPos;
in vec3 vColor;
out vec3 outColor;

void main() {
    gl_Position = vec4(vPos, 1.0);
    outColor = vColor;
}
