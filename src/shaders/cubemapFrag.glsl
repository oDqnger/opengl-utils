#version 460 core
out vec4 aColor;
in vec3 TexCoords;

uniform samplerCube texture1;

void main() {
    aColor = texture(texture1, TexCoords);
}