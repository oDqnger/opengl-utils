#include "glad/glad.h"
#include "skybox.h"
#include "textures.h"
#include "shader.h"
#include <cglm/cglm.h>

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

void renderSkybox(unsigned int cubemapShader, unsigned int skyboxTexture, unsigned int skyboxVAO, mat4 view, mat4 projection) {
    glDepthMask(GL_FALSE);
    glUseProgram(cubemapShader);
    mat4 new_view;
    mat3 rot;

    glm_mat4_copy(view, new_view);
    glm_mat4_pick3(new_view, rot);
    glm_mat4_identity(new_view); 
    glm_mat4_ins3(rot, new_view);
    glUniformMatrix4fv(glGetUniformLocation(cubemapShader, "view"), 1, GL_FALSE, new_view[0]);
    glUniformMatrix4fv(glGetUniformLocation(cubemapShader, "projection"), 1, GL_FALSE, projection[0]);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}