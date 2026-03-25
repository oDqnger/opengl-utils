#ifndef SKYBOX_H
#define SKYBOX_H

#include <cglm/cglm.h>

unsigned int generateSkyboxVAO();
unsigned int generateSkyboxCubemap();
unsigned int generateSkyboxShader(const char* vertex, const char* frag);
void renderSkybox(unsigned int shader, unsigned int texture, unsigned int vao, mat4 view, mat4 projection);

#endif