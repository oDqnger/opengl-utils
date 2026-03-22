#ifndef SKYBOX_H
#define SKYBOX_H

unsigned int generateSkyboxVAO();
unsigned int generateSkyboxCubemap();
unsigned int generateSkyboxShader(const char* vertex, const char* frag);

#endif