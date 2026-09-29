#version 330 core

in vec3 vPos;
in vec3 vColor;
in vec3 vPathPos;
out vec3 outColor;
uniform bool uUsePathPosition;

void main()
{
    gl_Position = vec4(uUsePathPosition ? vPathPos : vPos, 1.0);
    outColor = vColor;
}
