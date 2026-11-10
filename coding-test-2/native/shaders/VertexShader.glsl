#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
uniform mat4 model, view, projection;
uniform mat3 normalMatrix;
out vec3 worldPosition;
out vec3 worldNormal;
void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    worldPosition = (model * vec4(position, 1.0)).xyz;
    worldNormal = normalMatrix * normal;
}
