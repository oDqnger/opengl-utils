#ifndef SHADER_H
#define SHADER_H

extern unsigned int vertexId;
extern unsigned int fragmentId;
extern unsigned int shaderProgram;

unsigned int ConfigureVertex(const char* vertexFilePath);
unsigned int ConfigureFragment(const char* fragmentFilePath);
char* readFile(const char* filePath);
unsigned int LinkShaders(unsigned int vertexShader, unsigned int fragmentId);
void checkError(unsigned int shader, unsigned int shaderOrProgram);
void useShader(unsigned int shaderProgram);

#endif
