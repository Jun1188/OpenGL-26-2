#version 330 core

in vec4 vPos;
in vec3 vColor;

out vec3 outColor;
uniform vec4 uMVP;

void main()
{
    gl_Position = uMVP * vPos;
    outColor = vColor;
}
