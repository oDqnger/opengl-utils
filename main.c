#include <cglm/vec3.h>
#include <stdio.h>
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "./utils/shader.h"
#include "./utils/stb_image.h"
#include <cglm/cglm.h>
#include "./utils/camera.h"
#include "./utils/textures.h"
#include <stdbool.h>
#include <string.h>
#include "./utils/model.h"

float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
vec3 dir;
mat4 view;
vec3 right;
vec3 up;
vec3 cameraPos = {0.0f, 0.0f, 15.0f};
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
    movementKeys(window, dir, speed, cameraPos, right, acceleration);
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

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
  };

  unsigned int VAO, VAO2, VBO;
  glGenVertexArrays(1, &VAO);
  glGenVertexArrays(1, &VAO2);
  glGenBuffers(1, &VBO);
  
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(VAO2);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  int width, height, nrChannels;
  unsigned char *img_data = load_data(&width, &height, &nrChannels, "/home/diman/Desktop/Diman/Programming work/C/test-opengl/src/assets/container2.png");
  unsigned int textureId, specularId;
  generateTexture(&textureId);
  bindTexture(textureId);
  uploadTexture(width, height, nrChannels, img_data);
  setTexParameters();
  img_data = load_data(&width, &height, &nrChannels, "/home/diman/Desktop/Diman/Programming work/C/test-opengl/src/assets/container2_specular.png");
  generateTexture(&specularId);
  bindTexture(specularId);
  uploadTexture(width, height, nrChannels, img_data);
  setTexParameters();

  mat4 projection;
  glm_perspective(glm_rad(80.0f), 800.0f/600.f, 0.1f, 100.f, projection);

  float deltaTime;
  float currentFrame;
  float lastFrame;
  vec3 distance;

  // shader stuff
  int vertexId = ConfigureVertex("/home/diman/Desktop/Diman/Programming work/C/test-opengl/src/shaders/vertex.glsl");
  int containerFrag = ConfigureFragment("/home/diman/Desktop/Diman/Programming work/C/test-opengl/src/shaders/fragContainer.glsl");
  int lightFrag = ConfigureFragment("/home/diman/Desktop/Diman/Programming work/C/test-opengl/src/shaders/fragLight.glsl");
  int lightShader = LinkShaders(vertexId, lightFrag);
  int containerShader = LinkShaders(vertexId, containerFrag);
 
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);


  typedef struct {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
  } DirectionalLight;

  typedef struct {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
  } PointLight;

  DirectionalLight dl[2] = {
    {{ 5.0f, 5.0f, 5.0f }, {0.25f, 0.25f, 0.25f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
    {{5.0f,-5.0f,5.0f }, {0.08f, 0.08f, 0.08f}, {0.3f, 0.3f, 0.3f}, {0.4f, 0.4f, 0.4f}}
  };

  char dest[100] = "";
  float degrees = 0;
  float degrees1 = 0;

Model backpack = Model_load("/home/diman/Desktop/Diman/Programming work/C/test-opengl/src/models/backpack.obj");

  while(!glfwWindowShouldClose(window))
  {
    // setting speed
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    speed = deltaTime * 1.9;

    // input proccessing
    processInput(window);
    glfwSetCursorPosCallback(window, mouse_callback);
   
    // camera calculation
    calcCoordAxes(right, up, dir);
    calcTarget(cameraPos, dir, target);
    glm_lookat(cameraPos, target, up, view);

    // clearing screen after every loop
    glClearColor(0,0,sinf(glm_rad(degrees)), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // LIGHT CUBE
    useShader(lightShader);

    glUniformMatrix4fv(glGetUniformLocation(lightShader, "view"), 1, GL_FALSE, view[0]);
    glUniformMatrix4fv(glGetUniformLocation(lightShader, "projection"), 1, GL_FALSE, projection[0]);
   
    glm_mat4_identity(model);
    glm_translate(model, dl[0].position);
    glm_scale_uni(model, 0.8);
    dl[0].position[1] = sinf(glm_rad(degrees))* 10;
    dl[0].position[2] = cosf(glm_rad(degrees)) * 10 + 5;
    degrees += 0.5;

    glUniformMatrix4fv(glGetUniformLocation(lightShader, "model"), 1, GL_FALSE, model[0]);

    glUniform4f(glGetUniformLocation(lightShader, "lightColor"), dl[0].diffuse[0], dl[0].diffuse[1], dl[0].diffuse[2], 1.0);
    glBindVertexArray(VAO2);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glm_mat4_identity(model);
    glm_translate(model, dl[1].position);
    glm_scale_uni(model, 0.8);
    dl[1].position[1] = sinf(glm_rad(degrees1))* 10;
    dl[1].position[2] = -cosf(glm_rad(degrees1)) * 10 + 5;
    degrees1 -= 0.5;

    glUniformMatrix4fv(glGetUniformLocation(lightShader, "model"), 1, GL_FALSE, model[0]);

    glUniform4f(glGetUniformLocation(lightShader, "lightColor"), dl[1].diffuse[0], dl[1].diffuse[1], dl[1].diffuse[2], 1.0);
    glBindVertexArray(VAO2);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    // CONTAINER
    useShader(containerShader);

    glUniformMatrix4fv(glGetUniformLocation(containerShader, "view"), 1, GL_FALSE, view[0]);
    glUniformMatrix4fv(glGetUniformLocation(containerShader, "projection"), 1, GL_FALSE, projection[0]);
    glUniform3f(glGetUniformLocation(containerShader, "viewPos"),cameraPos[0], cameraPos[1], cameraPos[2]);
    glUniform1i(glGetUniformLocation(containerShader, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(containerShader, "material.specular"), 1);
    glUniform1f(glGetUniformLocation(containerShader, "material.shininess"), 32);
    for (int i = 0; i < 2; i++) {
      sprintf(dest, "dirLight[%d].targetPos", i);
      glUniform3f(glGetUniformLocation(containerShader, dest), 5,0,5);
      sprintf(dest, "dirLight[%d].lightPos", i);
      glUniform3f(glGetUniformLocation(containerShader, dest), dl[i].position[0], dl[i].position[1], dl[i].position[2]);
      sprintf(dest, "dirLight[%d].ambient", i);
      glUniform3f(glGetUniformLocation(containerShader, dest), dl[i].ambient[0], dl[i].ambient[1], dl[i].ambient[2]);
      sprintf(dest, "dirLight[%d].diffuse", i);
      glUniform3f(glGetUniformLocation(containerShader, dest), dl[i].diffuse[0], dl[i].diffuse[1], dl[i].diffuse[2]);
      sprintf(dest, "dirLight[%d].specular", i);
      glUniform3f(glGetUniformLocation(containerShader, dest), dl[i].specular[0], dl[i].specular[1], dl[i].specular[2]);
    }

    // for (int i = 0; i < 1; i++) {
    //   sprintf(dest, "pointLights[%d].position", i);
    //   glUniform3f(glGetUniformLocation(containerShader, dest), pl[i].position[0], pl[i].position[1], pl[i].position[2]);
    //
    //   sprintf(dest, "pointLights[%d].constant", i);
    //   glUniform1f(glGetUniformLocation(containerShader, dest), pl[i].constant);
    //   sprintf(dest, "pointLights[%d].linear", i);
    //   glUniform1f(glGetUniformLocation(containerShader, dest), pl[i].linear);
    //
    //   sprintf(dest, "pointLights[%d].quadratic", i);
    //   glUniform1f(glGetUniformLocation(containerShader, dest), pl[i].quadratic);
    //
    //   sprintf(dest, "pointLights[%d].ambient", i);
    //   glUniform3f(glGetUniformLocation(containerShader, dest), pl[i].ambient[0], pl[i].ambient[1], pl[i].ambient[2]);
    //
    //   sprintf(dest, "pointLights[%d].diffuse", i);
    //   glUniform3f(glGetUniformLocation(containerShader, dest), pl[i].diffuse[0], pl[i].diffuse[1], pl[i].diffuse[2]);
    //
    //   sprintf(dest, "pointLights[%d].specular", i);
    //   glUniform3f(glGetUniformLocation(containerShader, dest), pl[i].specular[0], pl[i].specular[1], pl[i].specular[2]);
    // }

    for (int x = 0; x<10; x++) {
      glm_mat4_identity(model);
      glm_translate(model, (vec3){x,0,0});
      glUniformMatrix4fv(glGetUniformLocation(containerShader, "model"), 1, GL_FALSE, model[0]);
      for (int y = 0; y<10; y++) {
        glm_mat4_identity(model);
        glm_translate(model, (vec3){x,0,y});
        glUniformMatrix4fv(glGetUniformLocation(containerShader, "model"), 1, GL_FALSE, model[0]);
        
        glActiveTexture(GL_TEXTURE0);
        bindTexture(textureId);
        glActiveTexture(GL_TEXTURE1);
        bindTexture(specularId);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
      glActiveTexture(GL_TEXTURE0);
      bindTexture(textureId);
      glActiveTexture(GL_TEXTURE1);
      bindTexture(specularId);
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    Model_draw(&backpack, &containerShader);

    // change buffers and check events
    glfwSwapBuffers(window);
    glfwPollEvents();    
  }
 
  // clear memory
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(containerShader);
  glDeleteProgram(lightShader);
  glDeleteVertexArrays(1, &VAO2);
  
  glfwTerminate();
  return 0;
}
