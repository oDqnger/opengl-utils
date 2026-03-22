#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stddef.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <cglm/vec3.h>
#include "shader.h"
#include "camera.h"
#include "textures.h"
#include "lighting.h"
#include "block.h"
#include "model.h"
#include "stb_image.h"
#include "skybox.h"
#include "framebuffer.h"

float yaw   = -90.0f;
float pitch =  0.0f;
double lastX =  800.0 / 2.0;
double lastY =  600.0 / 2.0;
vec3 dir;
mat4 view;
vec3 right;
vec3 up;
vec3 cameraPos = {0,0,0};
vec3 target = {0.0f, 0.0f, 0.0f};
mat4 model;
float speed;
float acceleration = 0.2;
float disableMovement = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  


void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    movementKeys(window, dir, speed, cameraPos, right, disableMovement);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  movementMouse(window, xpos, ypos, &lastX, &lastY, &yaw, &pitch, dir);
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Window did not work pls plsplsplsplsplps mak eit work pls thx\n");
    return -1;
  }
  glfwMakeContextCurrent(window);
  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
      printf("Failed to initialize GLAD wow how coudl u\n");
      return -1;
  }    
  
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  unsigned int containerVAO = setBufferObject();
  unsigned int containerTexture = setTextureBuffer("/home/diman/Desktop/Diman/Programming work/C/template/src/assets/container2.png");
  unsigned int containerSpecular = setTextureBuffer("/home/diman/Desktop/Diman/Programming work/C/template/src/assets/container2_specular.png");

  unsigned int skyboxVAO = generateSkyboxVAO();
  unsigned int skyboxTexture = generateSkyboxCubemap();

  unsigned int fbo = generateFBO();
  unsigned int modifiedTexture = generateColorbufferFromFBO();
  unsigned int screenVAO = generateScreen();

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("Framebuffer not complete");
    return -1;
  } else {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  mat4 projection;
  glm_perspective(glm_rad(80.0f), 800.0f/600.f, 0.1f, 400, projection);

  float deltaTime;
  float currentFrame;
  float lastFrame;

  unsigned int vertexId = ConfigureVertex("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/vertex.glsl");
  unsigned int containerFrag = ConfigureFragment("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/fragContainer.glsl");
  unsigned int lightFrag = ConfigureFragment("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/fragLight.glsl");

  unsigned int reflectiveVertex = ConfigureVertex("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/reflectiveVertex.glsl");
  unsigned int reflectiveFrag = ConfigureFragment("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/reflectiveFrag.glsl");

  unsigned int containerShader = LinkShaders(vertexId, containerFrag);
  unsigned int lightShader = LinkShaders(vertexId, lightFrag);
  unsigned int screenShader = generateScreenShaders("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/screenVertex.glsl", "/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/screenFrag.glsl");
  unsigned int reflectiveShader = LinkShaders(reflectiveVertex, reflectiveFrag);
  unsigned int cubemapShader = generateSkyboxShader("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/cubemapVertex.glsl", "/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/cubemapFrag.glsl");
 
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  DirectionalLight dl[1] = {
    {{ 2, 0, 2}, {0.2, 0.2, 0.2}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
  };
  PointLight pl = {{0,0,0}, 1, 0.5, 0.32, {0.2, 0.2, 0.2}, {1,1,1}, {1,1,1}};
  SpotLight sl = {cosf(glm_rad(20)), cosf(glm_rad(40)), {dir[0], dir[1], dir[2]}, {cameraPos[0], cameraPos[1], cameraPos[2]}, 1, 0.017, 0.0007};
  
  // Model place = Model_load("/home/diman/Desktop/Diman/Programming work/C/template/src/models/place/source/Untitled.glb");
  bool onFloor = false;

  while(!glfwWindowShouldClose(window))
  {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    speed = deltaTime * 2;
    processInput(window);
    glfwSetCursorPosCallback(window, mouse_callback);

    calcCoordAxes(right, up, dir);
    calcTarget(cameraPos, dir, target);
    glm_lookat(cameraPos, target, up, view);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glEnable(GL_DEPTH_TEST);
    glClearColor(1,1,1, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

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
    glUseProgram(reflectiveShader);
    glUniformMatrix4fv(glGetUniformLocation(reflectiveShader, "view"), 1, GL_FALSE, view[0]);
    glUniformMatrix4fv(glGetUniformLocation(reflectiveShader ,"projection"), 1, GL_FALSE, projection[0]);
    glm_mat4_identity(model);
    glm_translate(model, (vec3){1,0,1});
    glm_scale(model, (vec3){1, 10, 1});
    glUniformMatrix4fv(glGetUniformLocation(reflectiveShader, "model"), 1, GL_FALSE, model[0]);
    glUniform3f(glGetUniformLocation(reflectiveShader, "cameraPos"), cameraPos[0], cameraPos[1], cameraPos[2]);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    glBindVertexArray(containerVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(screenShader);
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(screenVAO);
    glBindTexture(GL_TEXTURE_2D, modifiedTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
 
  glDeleteVertexArrays(1, &containerVAO);
  glDeleteProgram(containerShader);
  glDeleteProgram(screenShader);
  glDeleteFramebuffers(1, &fbo);
  
  glfwTerminate();
  return 0;
}
