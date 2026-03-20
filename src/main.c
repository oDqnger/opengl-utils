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
#include "nuklear.h"

float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
vec3 dir;
mat4 view;
vec3 right;
vec3 up;
vec3 cameraPos = {0,0,0};
vec3 target = {0.0f, 0.0f, 0.0f};
mat4 model;
float speed;
float acceleration = 0.2;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  


void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    movementKeys(window, dir, speed, cameraPos, right);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  movementMouse(window, xpos, ypos, lastX, lastY, yaw, pitch, dir);
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
  
  // unsigned int containerVAO = setBufferObject();
  // unsigned int containerTexture = setTextureBuffer("/home/diman/Desktop/Diman/Programming work/C/template/src/assets/container2.png");
  // unsigned int containerSpecular = setTextureBuffer("/home/diman/Desktop/Diman/Programming work/C/template/src/assets/container2_specular.png");
  
  int width, height, nrChannels;
  unsigned char* grassData = load_data(&width, &height, &nrChannels, "/home/diman/Desktop/Diman/Programming work/C/template/src/assets/grass.png");
  unsigned int grassTexture;
  generateTexture(&grassTexture);
  bindTexture(grassTexture);
  uploadTexture(width, height, nrChannels, grassData);
  setTexParameters();

  float vertices[] = {
    0,0,0,  0,0,
    0.5, 0, 0, 1, 0,
    0, 0.5, 0, 0, 1,
    0.5, 0.5, 0, 1,1
  };
  unsigned int indices[] = {
    0,1,2,
    1,2,3
  };

  unsigned int grassVAO, grassVBO, grassEBO;
  glGenVertexArrays(1, &grassVAO);
  glGenBuffers(1, &grassVBO);
  glGenBuffers(1, &grassEBO);
  glBindVertexArray(grassVAO);
  glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  mat4 projection;
glm_perspective(glm_rad(80.0f), 800.0f/600.f, 0.1f, 400, projection);

  float deltaTime;
  float currentFrame;
  float lastFrame;
  vec3 distance;

  // shader stuff
  int vertexId = ConfigureVertex("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/vertex.glsl");
  int vertexId2= ConfigureVertex("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/example_vertex.glsl");
  int containerFrag = ConfigureFragment("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/fragContainer.glsl");
  int lightFrag = ConfigureFragment("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/fragLight.glsl");
  int transFrag = ConfigureFragment("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/example_transparency.glsl");
  unsigned int containerShader = LinkShaders(vertexId, containerFrag);
  unsigned int lightShader = LinkShaders(vertexId, lightFrag);
  unsigned int transShader = LinkShaders(vertexId2, transFrag);
 
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);

  DirectionalLight dl[1] = {
    {{ 2, 0, 2}, {0.2, 0.2, 0.2}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
  };
  PointLight pl = {{0,0,0}, 1, 0.5, 0.32, {0.2, 0.2, 0.2}, {1,1,1}, {1,1,1}};
  SpotLight sl = {cosf(glm_rad(20)), cosf(glm_rad(40)), {dir[0], dir[1], dir[2]}, {cameraPos[0], cameraPos[1], cameraPos[2]}, 1, 0.017, 0.0007};
  
  Model place = Model_load("/home/diman/Desktop/Diman/Programming work/C/template/src/models/place/source/Untitled.glb");

  while(!glfwWindowShouldClose(window))
  {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    speed = deltaTime * 5;

    processInput(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    calcCoordAxes(right, up, dir);
    calcTarget(cameraPos, dir, target);
    glm_lookat(cameraPos, target, up, view);

    glClearColor(0.09,0.606,0.829, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(transShader);
    glUniformMatrix4fv(glGetUniformLocation(transShader, "view"), 1, GL_FALSE, view[0]);
    glUniformMatrix4fv(glGetUniformLocation(transShader, "projection"), 1, GL_FALSE, projection[0]);
    glm_mat4_identity(model);
    glUniformMatrix4fv(glGetUniformLocation(transShader, "model"), 1, GL_FALSE, model[0]);
    glUniform1i(glGetUniformLocation(transShader, "texture"), 0);
    glBindVertexArray(grassVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    

    // glEnable(GL_STENCIL_TEST);
    // glStencilMask(0xFF);
    // glStencilOp(GL_ALWAYS, GL_ALWAYS, GL_ALWAYS);
    // glUseProgram(outlineShader); 
    // glDisable(GL_DEPTH_TEST);
    // glStencilMask(0x00);
    // // scale object
    // // use outline shader
    // // draw objects
    // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // glEnable(GL_DEPTH_TEST);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    // glUseProgram(containerShader);
    // glUniformMatrix4fv(glGetUniformLocation(containerShader, "view"), 1, GL_FALSE, view[0]);
    // glUniformMatrix4fv(glGetUniformLocation(containerShader, "projection"), 1, GL_FALSE, projection[0]);
    // glUniform3f(glGetUniformLocation(containerShader, "viewPos"),cameraPos[0], cameraPos[1], cameraPos[2]);
    // glUniform1i(glGetUniformLocation(containerShader, "material.texture_diffuse1"), 0);
    // glUniform1i(glGetUniformLocation(containerShader, "material.texture_specular1"), 1);
    // glUniform1f(glGetUniformLocation(containerShader, "material.shininess"), 32);
    //
    // setDirectionalLight(containerShader, dl, 1);
    // // setPointLight(containerShader, &pl, 1);
    // // setSpotLight(containerShader, &sl, cameraPos, dir);
    //
    // glm_mat4_identity(model);
    // glUniformMatrix4fv(glGetUniformLocation(containerShader, "model"), 1, GL_FALSE, model[0]);
    // // Model_draw(&place, &containerShader);
    // //
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, containerTexture);
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, containerSpecular);
    // glBindVertexArray(containerVAO);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
 
  // glDeleteVertexArrays(1, &containerVAO);
  glDeleteProgram(containerShader);
  
  glfwTerminate();
  return 0;
}

