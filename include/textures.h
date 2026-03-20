#ifndef TEXTURES_H
#define TEXTURES_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "./stb_image.h"
#include <cglm/cglm.h>

unsigned char* load_data(int *width, int *height, int *nrChannels, const char* path);
void generateTexture(unsigned int *textureId);
void bindTexture(unsigned int textureId);
void uploadTexture(int width, int height, int nrChannels, unsigned char* img_data);
void setTexParameters();
#endif
