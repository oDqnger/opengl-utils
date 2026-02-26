#ifndef CAMERA_H
#define CAMERA_H

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdbool.h>

void movementKeys(GLFWwindow *window, vec3 dir, float speed, vec3 cameraPos, vec3 right, float acceleration);
void movementMouse(GLFWwindow *window, double xpos, double ypos, double lastX, double lastY, float yaw, float pitch, vec3 dir);
void calcCoordAxes(vec3 right, vec3 up, vec3 dir);
void calcTarget(vec3 cameraPos, vec3 dir, vec3 target);
bool checkCollision(float sumRadi, float distance);
void setupInitialCam(vec3 dir, vec3 cameraPos, mat4 projection);

#endif
