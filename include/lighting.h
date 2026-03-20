#ifndef LIGHTING_H
#define LIGHTING_H

#include "glad/glad.h"
#include <cglm/cglm.h>

typedef struct {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
} DirectionalLight;

typedef struct {
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
} PointLight;

typedef struct {
  float innerCutoff;
  float outerCutoff;
  vec3 cameraDir;
  vec3 cameraPos;

  float constant;
  float linear;
  float quadratic;
} SpotLight;

void setDirectionalLight(unsigned int shader, DirectionalLight* dl, unsigned int nDl);
void setPointLight(unsigned int shader, PointLight* pointLight, unsigned int nPl);
void setSpotLight(unsigned int shader, SpotLight* spotLight, vec3 cameraPos, vec3 dir);

#endif
