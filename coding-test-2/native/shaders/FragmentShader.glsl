#version 330 core
in vec3 worldPosition;
in vec3 worldNormal;
out vec4 fragmentColor;
uniform vec3 objectColor, lightPosition, lightColor, viewPosition;
uniform float Ka, Kd, Ks, shininess, attenuation, gain;
uniform bool lighting, unlit;
uniform int studyView;
void main() {
    if (!lighting || unlit) { fragmentColor = vec4(objectColor, 1.0); return; }
    vec3 N = normalize(worldNormal);
    vec3 delta = lightPosition - worldPosition;
    float distance2 = dot(delta, delta);
    vec3 L = delta / sqrt(max(distance2, 1e-12));
    vec3 V = normalize(viewPosition - worldPosition);
    vec3 ambient = Ka * vec3(0.2);
    vec3 diffuse = Kd * max(dot(N, L), 0.0) * lightColor;
    float facing = dot(N, L) > 0.0 ? 1.0 : 0.0;
    vec3 specular = facing * Ks * pow(max(dot(V, reflect(-L, N)), 0.0), shininess) * lightColor;
    vec3 result = objectColor * (ambient + (diffuse + specular) / (1.0 + attenuation * distance2));
    if (studyView == 1) result = N * 0.5 + 0.5;
    if (studyView == 2) result = objectColor * diffuse / (1.0 + attenuation * distance2);
    if (studyView == 3) result = objectColor * specular / (1.0 + attenuation * distance2);
    fragmentColor = vec4(result * gain, 1.0);
}
