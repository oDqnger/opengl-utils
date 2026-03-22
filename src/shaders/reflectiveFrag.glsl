#version 460 core
out vec4 aColor;
in vec3 Normal;
in vec3 Position;

uniform samplerCube texture1;
uniform vec3 cameraPos;

void main() {
    vec3 incidence = normalize(Position - cameraPos);
    vec3 reflective = reflect(incidence, normalize(Normal));
    vec3 refractive = refract(incidence, normalize(Normal), 1/1.5);

    aColor = texture(texture1, reflective);
    // aColor = vec4(0,0,0,1);
}