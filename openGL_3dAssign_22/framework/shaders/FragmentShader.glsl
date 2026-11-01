#version 330 core
in vec3 outColor;
in vec2 TexCoord;
in vec3 Normal;
uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform bool uLighting;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform bool uOverrideColor;
uniform vec3 uOutlineColor;
uniform vec3 uTint;
out vec4 FragColor;
void main() {
    if (uOverrideColor) { FragColor = vec4(uOutlineColor, 1.0); return; }
    vec4 color = vec4(outColor * uTint, 1.0);
    if (uHasTexture) color *= texture(uTexture, TexCoord);
    if (uLighting && length(Normal) > 0.0)
        color.rgb *= (0.25 + 0.75 * max(dot(normalize(Normal), normalize(uLightDirection)), 0.0)) * uLightColor;
    FragColor = color;
}
