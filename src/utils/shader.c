#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shader.h"
char* readFile(const char* filePath) {
  FILE* fptr;
  fptr = fopen(filePath, "r");
  if (fptr == NULL) {
    printf("ERROR IN FILE PATH (SHADER ISSUE) \n");
  }
  char str[10000];
  char* source = malloc(sizeof(char) * 10000);
  source[0] = '\0';

  fgets(str, 10000, fptr);
  strcpy(source, str);
  while (fgets(str, 10000, fptr)) {
    strcat(source, str);
  }
  fclose(fptr);
  return source;
}

void checkError(unsigned int shader, unsigned int shaderOrProgram) {
  int success;
  char infoLog[512];

  if (shaderOrProgram == 0) {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
  }
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    printf("%s\n", infoLog);
  }
}

unsigned int ConfigureVertex(const char* vertexFilePath) {
  unsigned int vertexId = glCreateShader(GL_VERTEX_SHADER);
  const char* source = readFile(vertexFilePath);
  glShaderSource(vertexId, 1, &source, NULL);
  glCompileShader(vertexId);
  checkError(vertexId, 0);
  return vertexId;
}

unsigned int ConfigureFragment(const char* fragmentFilePath) {
  unsigned int fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
  const char* source = readFile(fragmentFilePath);
  glShaderSource(fragmentId, 1, &source, NULL);
  glCompileShader(fragmentId);
  checkError(fragmentId, 0);
  return fragmentId;
}

unsigned int LinkShaders(unsigned int fragmentId, unsigned int vertexId) {
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexId);
  glAttachShader(shaderProgram, fragmentId);
  glLinkProgram(shaderProgram);

  checkError(shaderProgram, 1);

  glDeleteShader(vertexId);
  glDeleteShader(fragmentId);

  return shaderProgram;
}

void useShader(unsigned int shaderProgram) {
  glUseProgram(shaderProgram);
}
