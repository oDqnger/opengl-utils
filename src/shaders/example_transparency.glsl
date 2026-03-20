#version 460 core
out vec4 aColor;
in vec2 TexCoords;

uniform sampler2D texture1;

void main() {
  aColor = texture(texture1, TexCoords);
}
