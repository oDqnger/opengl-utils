#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <stdbool.h>
#include <stdio.h>
void movementKeys(GLFWwindow *window, vec3 dir, float speed, vec3 cameraPos, vec3 right, bool onFloor) {
    vec3 forward = { dir[0], 0.0f, dir[2] };
    if (glm_vec3_norm(forward) > 0.0001f) glm_vec3_normalize(forward);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (!onFloor) {
          vec3 vel;
          glm_vec3_scale(forward, speed, vel);
          glm_vec3_add(cameraPos, vel, cameraPos);
        } else {
          vec3 vel;
          glm_vec3_scale(forward, -speed, vel);
          glm_vec3_add(cameraPos, vel, cameraPos);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (!onFloor) {
          vec3 vel;
          glm_vec3_scale(forward, speed, vel);
          glm_vec3_sub(cameraPos, vel, cameraPos);
        } else {
          vec3 vel;
          glm_vec3_scale(forward, -speed, vel);
          glm_vec3_sub(cameraPos, vel, cameraPos);
        }
    }
    
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 velocity;
        glm_vec3_scale(right, speed, velocity);
        glm_vec3_sub(cameraPos, velocity, cameraPos);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 velocity;
        glm_vec3_scale(right, speed, velocity);
        glm_vec3_add(cameraPos, velocity, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
      cameraPos[1] += 2 * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
      cameraPos[1] -= 2 * speed;
    }
}

void movementMouse(GLFWwindow *window, double xpos, double ypos, double *lastX, double *lastY, float *yaw, float *pitch, vec3 dir) {
  float xoffset = xpos - *lastX;
  float yoffset = *lastY - ypos; 
  *lastX = xpos;
  *lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  *yaw   += xoffset;
  *pitch += yoffset;

  if(*pitch > 89.0f)
      *pitch = 89.0f;
  if(*pitch < -89.0f)
      *pitch = -89.0f;

  vec3 direction;
  direction[0] = cosf(glm_rad(*yaw)) * cosf(glm_rad(*pitch));
  direction[1] = sinf(glm_rad(*pitch));
  direction[2] = sinf(glm_rad(*yaw)) * cosf(glm_rad(*pitch));
  glm_vec3_normalize_to(direction, dir);
}
void calcCoordAxes(vec3 right, vec3 up, vec3 dir) {
  glm_vec3_crossn(dir, (vec3) {0.0f, 1.0f, 0.0f}, right);
  glm_vec3_crossn(right, dir, up);
}
void calcTarget(vec3 cameraPos, vec3 dir, vec3 target) {
  glm_vec3_add(cameraPos, dir, target);
}

bool checkCollision(vec3 cameraPos, vec3 objPos, float radius) {
  vec3 box[2] = {{objPos[0]-radius, objPos[1]-radius, objPos[2]-radius}, {objPos[0]+radius, objPos[1]+radius, objPos[2]+radius}};
  if (glm_aabb_point(box, cameraPos) == true) {
    return true;
  }
  return false;
}

void setupInitialCam(vec3 dir, vec3 cameraPos, mat4 projection);
