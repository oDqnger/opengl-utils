#include "glad/glad.h"
#include "skybox.h"
#include "textures.h"
#include "shader.h"

static float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};

unsigned int generateSkyboxVAO() {
  unsigned int cubemapVAO, cubemapVBO;
  glGenVertexArrays(1, &cubemapVAO);
  glGenBuffers(1, &cubemapVBO);
  glBindVertexArray(cubemapVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
  glEnableVertexAttribArray(0);

  return cubemapVAO;
}

unsigned int generateSkyboxCubemap() {
  unsigned int cubeMap;
  glGenTextures(1, &cubeMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

  const char* paths[6] = {
    "/home/diman/Desktop/Diman/Programming work/C/template/src/assets/skybox/right.jpg",
    "/home/diman/Desktop/Diman/Programming work/C/template/src/assets/skybox/left.jpg",
    "/home/diman/Desktop/Diman/Programming work/C/template/src/assets/skybox/top.jpg",
    "/home/diman/Desktop/Diman/Programming work/C/template/src/assets/skybox/bottom.jpg",
    "/home/diman/Desktop/Diman/Programming work/C/template/src/assets/skybox/front.jpg",
    "/home/diman/Desktop/Diman/Programming work/C/template/src/assets/skybox/back.jpg",
  };

  for (int i = 0; i<6; i++) {
    int width, height, nrChannels;
    char* data = load_data(&width, &height, &nrChannels, paths[i], false);
    uploadTexture(width, height, nrChannels, data, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return cubeMap;
}

unsigned int generateSkyboxShader(const char* vertex, const char* frag) {
  unsigned int cubemapVertex = ConfigureVertex(vertex);
  unsigned int cubemapFrag = ConfigureFragment(frag);

  unsigned int cubemapShader = LinkShaders(cubemapVertex, cubemapFrag);

  return cubemapShader;
}