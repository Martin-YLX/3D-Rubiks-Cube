#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

extern glm::vec3 grayColor;
extern glm::vec3 standardColors[6];
extern glm::vec3 target;
extern float radius;
extern float yaw;
extern float pitch;
extern float lastX;
extern float lastY;
extern bool firstMouse;
extern bool cameraFollow;

glm::vec3 computeCameraPos();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif // CAMERA_H