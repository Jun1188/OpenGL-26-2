#pragma once

namespace graphics::detail {
inline constexpr char vertexShader[] = R"GLSL(#version 330 core
layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vColor;
layout(location=2) in vec2 vTexCoord;
layout(location=3) in vec3 vNormal;
uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat3 uNormalMatrix;
out vec3 outColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 WorldPosition;
void main() {
    gl_Position = uMVP * vec4(vPos, 1.0);
    outColor = vColor;
    TexCoord = vTexCoord;
    Normal = uNormalMatrix * vNormal;
    WorldPosition = (uModel * vec4(vPos, 1.0)).xyz;
}
)GLSL";
inline constexpr char fragmentShader[] = R"GLSL(#version 330 core
in vec3 outColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 WorldPosition;
uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform bool uLighting;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform float uDirectionalIntensity;
uniform vec3 uViewPosition;
uniform vec3 uAmbientColor;
uniform float uMaterialAmbient;
uniform float uMaterialDiffuse;
uniform float uMaterialSpecular;
uniform float uShininess;
struct PointLight { vec3 position; vec3 color; float intensity; float attenuation; };
uniform int uPointLightCount;
uniform PointLight uPointLights[8];
uniform bool uOverrideColor;
uniform vec3 uOutlineColor;
uniform vec3 uTint;
out vec4 FragColor;
float highlight(vec3 n, vec3 lightDirection, vec3 viewDirection) {
    if (dot(n, lightDirection) <= 0.0) return 0.0;
    return uMaterialSpecular * pow(max(dot(viewDirection, reflect(-lightDirection, n)), 0.0), uShininess);
}
void main() {
    if (uOverrideColor) { FragColor = vec4(uOutlineColor, 1.0); return; }
    vec4 color = vec4(outColor * uTint, 1.0);
    if (uHasTexture) color *= texture(uTexture, TexCoord);
    if (uLighting && length(Normal) > 0.0) {
        vec3 n = normalize(Normal);
        vec3 viewDelta = uViewPosition - WorldPosition;
        vec3 viewDirection = viewDelta / sqrt(max(dot(viewDelta, viewDelta), 1e-12));
        vec3 sunDirection = normalize(uLightDirection);
        vec3 illumination = uMaterialAmbient * uAmbientColor
            + (uMaterialAmbient + 0.75 * uMaterialDiffuse * max(dot(n, sunDirection), 0.0)
               + highlight(n, sunDirection, viewDirection)) * uLightColor * uDirectionalIntensity;
        for (int i = 0; i < uPointLightCount; ++i) {
            vec3 delta = uPointLights[i].position - WorldPosition;
            float distanceSquared = dot(delta, delta);
            vec3 direction = delta / sqrt(max(distanceSquared, 1e-12));
            illumination += uPointLights[i].color * uPointLights[i].intensity
                * (uMaterialDiffuse * max(dot(n, direction), 0.0) + highlight(n, direction, viewDirection))
                / (1.0 + uPointLights[i].attenuation * distanceSquared);
        }
        color.rgb *= illumination;
    }
    FragColor = color;
}
)GLSL";
}
