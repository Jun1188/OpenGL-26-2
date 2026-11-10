#version 330 core
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
// @debug hook shader-view
uniform int uStudyView; // 0 composed, 1 normals, 2 diffuse, 3 specular. Default uniform value is zero.
out vec4 FragColor;
// @debug hook specular
float highlight(vec3 n, vec3 lightDirection, vec3 viewDirection) {
    if (dot(n, lightDirection) <= 0.0) return 0.0;
    return uMaterialSpecular * pow(max(dot(viewDirection, reflect(-lightDirection, n)), 0.0), uShininess);
}
void main() {
    if (uOverrideColor) { FragColor = vec4(uOutlineColor, 1.0); return; }
    vec4 color = vec4(outColor * uTint, 1.0);
    if (uHasTexture) color *= texture(uTexture, TexCoord);
    // @debug hook lighting
    if (uLighting && length(Normal) > 0.0) {
        vec3 n = normalize(Normal);
        vec3 viewDelta = uViewPosition - WorldPosition;
        vec3 viewDirection = viewDelta / sqrt(max(dot(viewDelta, viewDelta), 1e-12));
        vec3 sunDirection = normalize(uLightDirection);
        // @debug hook ambient
        vec3 ambient = uMaterialAmbient * uAmbientColor
            + uMaterialAmbient * uLightColor * uDirectionalIntensity;
        // @debug hook diffuse
        vec3 diffuse = 0.75 * uMaterialDiffuse * max(dot(n, sunDirection), 0.0)
            * uLightColor * uDirectionalIntensity;
        vec3 specular = highlight(n, sunDirection, viewDirection) * uLightColor * uDirectionalIntensity;
        for (int i = 0; i < uPointLightCount; ++i) {
            vec3 delta = uPointLights[i].position - WorldPosition;
            float distanceSquared = dot(delta, delta);
            vec3 direction = delta / sqrt(max(distanceSquared, 1e-12));
            vec3 energy = uPointLights[i].color * uPointLights[i].intensity
                / (1.0 + uPointLights[i].attenuation * distanceSquared);
            diffuse += energy * uMaterialDiffuse * max(dot(n, direction), 0.0);
            specular += energy * highlight(n, direction, viewDirection);
        }
        if (uStudyView == 1) color.rgb = n * 0.5 + 0.5;
        else if (uStudyView == 2) color.rgb *= diffuse;
        else if (uStudyView == 3) color.rgb *= specular;
        else color.rgb *= ambient + diffuse + specular;
    }
    FragColor = color;
}
