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
#include "bullet.h"
#include "stb_ds.h"

float yaw   = -90.0f;
float pitch =  0.0f;
double lastX =  800.0 / 2.0;
double lastY =  600.0 / 2.0;
vec3 dir;
mat4 view;
vec3 right;
vec3 up;
vec3 cameraPos = {0,2,0};
vec3 target = {0.0f, 0.0f, 0.0f};
mat4 model;
float speed;
float acceleration = 0.2;
bool onFloor = false;
bool addBullets =false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    movementKeys(window, dir, speed, cameraPos, right, onFloor, 0.8);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  movementMouse(window, xpos, ypos, &lastX, &lastY, &yaw, &pitch, dir);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    addBullets = true;
  }
}

void create_object(unsigned int lightShader, unsigned int containerVAO, mat4 model, mat4 view, mat4 projection) {
  glUseProgram(lightShader);
  glUniformMatrix4fv(glGetUniformLocation(lightShader, "view"), 1, GL_FALSE, view[0]);
  glUniformMatrix4fv(glGetUniformLocation(lightShader ,"projection"), 1, GL_FALSE, projection[0]);
  glUniformMatrix4fv(glGetUniformLocation(lightShader, "model"), 1, GL_FALSE, model[0]);
  glUniform4f(glGetUniformLocation(lightShader, "lightColor"), 1,1,1,1);
  glBindVertexArray(containerVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
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
  unsigned int containerTexture = setTextureBuffer("../src/assets/container2.png");
  unsigned int containerSpecular = setTextureBuffer("../src/assets/container2_specular.png");

  unsigned int skyboxVAO = generateSkyboxVAO();
  unsigned int skyboxTexture = generateSkyboxCubemap();

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("Framebuffer not complete");
    return -1;
  } else {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  Bullet* bulletList = NULL;

  mat4 projection;
  glm_perspective(glm_rad(80.0f), 800.0f/600.f, 0.1f, 400, projection);

  float deltaTime;
  float currentFrame;
  float lastFrame;

  unsigned int cursorVAO = generateScreen();

  unsigned int vertexId = ConfigureVertex("../src/shaders/vertex.glsl");
  unsigned int containerFrag = ConfigureFragment("../src/shaders/fragContainer.glsl");
  unsigned int cursorVertexId = ConfigureVertex("../src/shaders/cursorVertex.glsl");
  unsigned int cursorFragId = ConfigureFragment("../src/shaders/cursorFrag.glsl");
  unsigned int lightFrag = ConfigureFragment("../src/shaders/fragLight.glsl");

  unsigned int reflectiveVertex = ConfigureVertex("../src/shaders/reflectiveVertex.glsl");
  unsigned int reflectiveFrag = ConfigureFragment("../src/shaders/reflectiveFrag.glsl");

  unsigned int containerShader = LinkShaders(vertexId, containerFrag);
  unsigned int lightShader = LinkShaders(vertexId, lightFrag);
  unsigned int reflectiveShader = LinkShaders(reflectiveVertex, reflectiveFrag);
  unsigned int cursorShader = LinkShaders(cursorVertexId, cursorFragId);
  unsigned int cubemapShader = generateSkyboxShader("../src/shaders/cubemapVertex.glsl", "../src/shaders/cubemapFrag.glsl");
 
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  DirectionalLight dl[1] = {
    {{ 2, 5, 2}, {0.2, 0.2, 0.2}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
  };
  PointLight pl = {{0,0,0}, 1, 0.5, 0.32, {0.2, 0.2, 0.2}, {1,1,1}, {1,1,1}};
  SpotLight sl = {cosf(glm_rad(20)), cosf(glm_rad(40)), {dir[0], dir[1], dir[2]}, {cameraPos[0], cameraPos[1], cameraPos[2]}, 1, 0.017, 0.0007};
  
  Model place = Model_load("/home/diman/Desktop/Diman/Programming work/C/template/src/models/gun/source/Rockets_Gun_UV_08.obj");
  float fallingSpeed = -0.02;
  float gravity = -1/500;

  struct VecWrapper {
    vec3 p;
  };
  struct VecWrapper* objectPositions = NULL;

  struct VecWrapper pos = {{6,3,9}};
  arrput(objectPositions, pos);
  pos = (struct VecWrapper){{2,3,9}};
  arrput(objectPositions, pos);
  pos = (struct VecWrapper){{9,3,8}};
  arrput(objectPositions, pos);
  pos = (struct VecWrapper){{0,3,12}};
  arrput(objectPositions, pos);

  while(!glfwWindowShouldClose(window))
  {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    speed = deltaTime * 2;
    processInput(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    if (addBullets) {
      arrput(bulletList, *create_bullet(dir, cameraPos));
      addBullets = false;
    }

    calcCoordAxes(right, up, dir);
    calcTarget(cameraPos, dir, target);
    glm_lookat(cameraPos, target, up, view);
    
    onFloor = checkCollision(cameraPos, (vec3){1,0,1}, 1, 36/2);
    if (!onFloor) {
      cameraPos[1] += fallingSpeed;
      fallingSpeed += gravity;
    } else {
      fallingSpeed = -0.02;
    }

    glClearColor(1,1,1, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    renderSkybox(cubemapShader, skyboxTexture, skyboxVAO, view, projection);

    glUseProgram(cursorShader);
    glm_mat4_identity(model);
    glm_scale(model, (vec3){0.01, 0.01, 0.01});
    glUniformMatrix4fv(glGetUniformLocation(reflectiveShader, "model"), 1, GL_FALSE, model[0]);
    glBindVertexArray(cursorVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUseProgram(reflectiveShader);
    glUniformMatrix4fv(glGetUniformLocation(reflectiveShader, "view"), 1, GL_FALSE, view[0]);
    glUniformMatrix4fv(glGetUniformLocation(reflectiveShader ,"projection"), 1, GL_FALSE, projection[0]);
    glm_mat4_identity(model);
    glm_translate(model, (vec3){1,0,1});
    glm_scale(model, (vec3){36, 1, 36});
    glUniformMatrix4fv(glGetUniformLocation(reflectiveShader, "model"), 1, GL_FALSE, model[0]);
    glUniform3f(glGetUniformLocation(reflectiveShader, "cameraPos"), cameraPos[0], cameraPos[1], cameraPos[2]);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    glBindVertexArray(containerVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    for (int i = 0; i<arrlen(objectPositions); i++) {
      glm_mat4_identity(model);
      glm_translate(model, objectPositions[i].p);
      create_object(lightShader, containerVAO, model, view, projection);
    }

    for (int i = 0; i<arrlen(bulletList); i++) {
      glUseProgram(lightShader);
      glUniformMatrix4fv(glGetUniformLocation(lightShader, "view"), 1, GL_FALSE, view[0]);
      glUniformMatrix4fv(glGetUniformLocation(lightShader ,"projection"), 1, GL_FALSE, projection[0]);
      glm_mat4_identity(model);
      glm_vec3_add(bulletList[i].dir, bulletList[i].currentPos, bulletList[i].currentPos);
      glm_translate(model, bulletList[i].currentPos);
      glm_scale(model, (vec3){0.1, 0.1, 0.1});
      glUniformMatrix4fv(glGetUniformLocation(lightShader, "model"), 1, GL_FALSE, model[0]);
      glUniform4f(glGetUniformLocation(lightShader, "lightColor"), 255,199/255,0,1);
      glBindVertexArray(containerVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);
     
      if (glm_vec3_distance(cameraPos, bulletList[i].currentPos) >= 10) {
        arrdel(bulletList, i);
        continue;
      }     

      for (int k = 0; k<arrlen(objectPositions); k++) {
        if (checkCollision(bulletList[i].currentPos, objectPositions[k].p, 0.5, 0.5)) {
          arrdel(bulletList, i);
          arrdel(objectPositions, k);
          break;
        }
      }

    }
    
    useShader(containerShader);

    mat4 gunView;
    glm_mat4_identity(gunView);
    mat4 gunModel;
    glm_mat4_identity(gunModel);
    glm_translate(gunModel, (vec3){0.3f, -0.25f, -0.5f});
    glm_scale(gunModel, (vec3){0.001f, 0.001f, 0.001f});

    glUniformMatrix4fv(glGetUniformLocation(containerShader, "view"), 1, GL_FALSE, gunView[0]);
    glUniformMatrix4fv(glGetUniformLocation(containerShader, "projection"), 1, GL_FALSE, projection[0]);
    glUniformMatrix4fv(glGetUniformLocation(containerShader, "model"), 1, GL_FALSE, gunModel[0]);

    glUniform1i(glGetUniformLocation(containerShader, "material.texture_diffuse1"), 0);
    glUniform1i(glGetUniformLocation(containerShader, "material.texture_specular1"), 1);
    glUniform1f(glGetUniformLocation(containerShader, "material.shininess"), 32);

    setDirectionalLight(containerShader, dl, 1);
    Model_draw(&place, &containerShader);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
 
  glDeleteVertexArrays(1, &containerVAO);
  glDeleteVertexArrays(1, &skyboxVAO);
  glDeleteProgram(containerShader);
  glDeleteProgram(reflectiveShader);
  
  glfwTerminate();
  return 0;
}
