#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include "glad/glad.h"

unsigned int generateFBO();
unsigned int generateColorbufferFromFBO();
unsigned int generateScreen();
unsigned int generateScreenShaders(const char* vert, const char* frag);

#endif