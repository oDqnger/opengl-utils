#include "glad/glad.h"
#include "framebuffer.h"
#include "shader.h"
#include <stddef.h>

unsigned int generateFBO() {
  unsigned int fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);  
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

  return fbo;
}

unsigned int generateColorbufferFromFBO() {
  unsigned int textureColorbuffer;
  glGenTextures(1, &textureColorbuffer);
  glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

  return textureColorbuffer;
}

static float vertices[] = {
    -1, -1, 0, 0,
    -1, 1, 0, 1,
    1,1, 1, 1,

    -1,-1, 0, 0,
    1, -1, 1, 0,
    1,1, 1, 1,
};

unsigned int generateScreen() {
  unsigned int grassVAO, grassVBO;
  glGenVertexArrays(1, &grassVAO);
  glGenBuffers(1, &grassVBO);
  glBindVertexArray(grassVAO);
  glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(2*sizeof(float)));
  glEnableVertexAttribArray(1);
  return grassVAO;
}

unsigned int generateScreenShaders(const char* vert, const char* frag) {
  unsigned int screenVertex = ConfigureVertex("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/screenVertex.glsl");
  unsigned int screenFragment = ConfigureFragment("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/screenFrag.glsl");
  unsigned int screenShader = LinkShaders(screenVertex, screenFragment);

  return screenShader;
}