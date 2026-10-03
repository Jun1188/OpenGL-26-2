#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vWorldNormal;

void main()
{
    vec4 modelPosition = vec4(aPosition, 1.0);
    vec4 worldPosition = uModel * modelPosition;
    vec4 viewPosition = uView * worldPosition;
    gl_Position = uProjection * viewPosition;

    // Inverse transpose also handles non-uniform model scaling.
    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vWorldNormal = normalMatrix * aNormal;
    // Clipping, perspective divide, viewport transform and rasterization
    // follow this shader in OpenGL; do not divide gl_Position by w here.
}
