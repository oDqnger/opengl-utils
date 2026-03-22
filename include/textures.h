#ifndef TEXTURES_H
#define TEXTURES_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "./stb_image.h"
#include <cglm/cglm.h>
#include <stdbool.h>

unsigned char* load_data(int *width, int *height, int *nrChannels, const char* path, bool is_flip);
void generateTexture(unsigned int *textureId);
void bindTexture(unsigned int textureId);
void uploadTexture(int width, int height, int nrChannels, unsigned char* img_data, GLenum texture_type);
void setTexParameters();
#endif
