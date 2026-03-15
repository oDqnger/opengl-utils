#include <cglm/vec3.h>
#include <stddef.h>
#include <stdio.h>
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "./utils/shader.h"
#include "./utils/stb_image.h"
#include <cglm/cglm.h>
#include "./utils/camera.h"
#include "./utils/textures.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "./utils/lighting.h"
#include "./utils/block.h"
#include "./utils/model.h"

float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
vec3 dir;
mat4 view;
vec3 right;
vec3 up;
vec3 cameraPos = {0.0f, 0.0f, 0.0f};
vec3 target = {0.0f, 0.0f, 0.0f};
mat4 model;
float speed;
float acceleration = 0.2;
vec3 target_block = {0.0f, 0.0f, 0.0f};

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
  
  unsigned int containerVAO = setBufferObject();

  mat4 projection;
glm_perspective(glm_rad(80.0f), 800.0f/600.f, 0.1f, 10000.f, projection);

  float deltaTime;
  float currentFrame;
  float lastFrame;
  vec3 distance;

  // shader stuff
  int vertexId = ConfigureVertex("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/vertex.glsl");
  int containerFrag = ConfigureFragment("/home/diman/Desktop/Diman/Programming work/C/template/src/shaders/fragContainer.glsl");
  unsigned int containerShader = LinkShaders(vertexId, containerFrag);
 
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);

  DirectionalLight dl[1] = {
    {{ 0.0f, 5.0f, 0.0f }, {0.2, 0.2, 0.2}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
  };
  
  Model coffee_bar = Model_load("/home/diman/Desktop/Diman/Programming work/C/template/src/models/place/source/Untitled.glb");

  while(!glfwWindowShouldClose(window))
  {
    // setting speed
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    speed = deltaTime * 50;

    // input proccessing
    processInput(window);
    glfwSetCursorPosCallback(window, mouse_callback);
   
    // camera calculation
    calcCoordAxes(right, up, dir);
    calcTarget(cameraPos, dir, target);
    glm_lookat(cameraPos, target, up, view);

    // clearing screen after every loop
    glClearColor(0.09,0.606,0.829, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUniformMatrix4fv(glGetUniformLocation(containerShader, "view"), 1, GL_FALSE, view[0]);
    glUniformMatrix4fv(glGetUniformLocation(containerShader, "projection"), 1, GL_FALSE, projection[0]);
    glUniform3f(glGetUniformLocation(containerShader, "viewPos"),cameraPos[0], cameraPos[1], cameraPos[2]);
    glUniform1i(glGetUniformLocation(containerShader, "material.texture_diffuse1"), 0);
    glUniform1i(glGetUniformLocation(containerShader, "material.texture_specular1"), 1);
    glUniform1f(glGetUniformLocation(containerShader, "material.shininess"), 32);

    setDirectionalLight(containerShader, dl, 1);

    glm_mat4_identity(model);
    // glm_rotate_atm(model, (vec3){0, 0, 0}, glm_rad(90), (vec3){1,0,0});
    // glm_rotate_at(model, (vec3){0, 0, 0}, glm_rad(180), (vec3){0,1,0});
    glm_scale(model, (vec3){0.5,0.5,0.5});
    glUniformMatrix4fv(glGetUniformLocation(containerShader, "model"), 1, GL_FALSE, model[0]);
    Model_draw(&coffee_bar, &containerShader);

    // change buffers and check events
    glfwSwapBuffers(window);
    glfwPollEvents();    
  }
 
  // clear memory
  glDeleteVertexArrays(1, &containerVAO);
  glDeleteProgram(containerShader);
  
  glfwTerminate();
  return 0;
}

