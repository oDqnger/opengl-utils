#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "stb_image.h"
#include <stdbool.h>
unsigned char* load_data(int *width, int *height, int *nrChannels, const char* path, bool is_flip) {
  stbi_set_flip_vertically_on_load(is_flip);
  unsigned char *img_data = stbi_load(path, width, height, nrChannels, 0);
  
  if (!img_data) {
      printf("Failed to load texture!\n");
      return NULL;
  }

  return img_data;
}

void generateTexture(unsigned int* textureId) {
  glGenTextures(1, textureId);
}
void bindTexture(unsigned int textureId) {
  glBindTexture(GL_TEXTURE_2D, textureId);
}
void uploadTexture(int width, int height, int nrChannels, unsigned char* img_data, GLenum texture_type) {
  GLenum format;

  if (nrChannels == 1)
      format = GL_RED;
  else if (nrChannels == 3)
      format = GL_RGB;
  else if (nrChannels == 4)
      format = GL_RGBA;
  glTexImage2D(texture_type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, img_data);
  glGenerateMipmap(GL_TEXTURE_2D);
}
void setTexParameters() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
