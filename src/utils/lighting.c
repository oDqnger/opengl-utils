#include "../../include/glad/glad.h"
#include "lighting.h"

void setPointLight(unsigned int shader, PointLight* pl, unsigned int nPl) {
  char dest[64];
  for (int i = 0; i < nPl; i++) {
    sprintf(dest, "pointLights[%d].position", i);
    glUniform3f(glGetUniformLocation(shader, dest), pl[i].position[0], pl[i].position[1], pl[i].position[2]);
    sprintf(dest, "pointLights[%d].constant", i);
    glUniform1f(glGetUniformLocation(shader, dest), pl[i].constant);
    sprintf(dest, "pointLights[%d].linear", i);
    glUniform1f(glGetUniformLocation(shader, dest), pl[i].linear);
    sprintf(dest, "pointLights[%d].quadratic", i);
    glUniform1f(glGetUniformLocation(shader, dest), pl[i].quadratic);
    sprintf(dest, "pointLights[%d].ambient", i);
    glUniform3f(glGetUniformLocation(shader, dest), pl[i].ambient[0], pl[i].ambient[1], pl[i].ambient[2]);
    sprintf(dest, "pointLights[%d].diffuse", i);
    glUniform3f(glGetUniformLocation(shader, dest), pl[i].diffuse[0], pl[i].diffuse[1], pl[i].diffuse[2]);
    sprintf(dest, "pointLights[%d].specular", i);
    glUniform3f(glGetUniformLocation(shader, dest), pl[i].specular[0], pl[i].specular[1], pl[i].specular[2]);
  } 
}

void setDirectionalLight(unsigned int shader, DirectionalLight* dl, unsigned int nDl) {
  char dest[64];
  for (int i = 0; i < nDl; i++) {
    sprintf(dest, "dirLight[%d].targetPos", i);
    glUniform3f(glGetUniformLocation(shader, dest), 5,0,5);
    sprintf(dest, "dirLight[%d].lightPos", i);
    glUniform3f(glGetUniformLocation(shader, dest), dl[i].position[0], dl[i].position[1], dl[i].position[2]);
    sprintf(dest, "dirLight[%d].ambient", i);
    glUniform3f(glGetUniformLocation(shader, dest), dl[i].ambient[0], dl[i].ambient[1], dl[i].ambient[2]);
    sprintf(dest, "dirLight[%d].diffuse", i);
    glUniform3f(glGetUniformLocation(shader, dest), dl[i].diffuse[0], dl[i].diffuse[1], dl[i].diffuse[2]);
    sprintf(dest, "dirLight[%d].specular", i);
    glUniform3f(glGetUniformLocation(shader, dest), dl[i].specular[0], dl[i].specular[1], dl[i].specular[2]);
    }
}
