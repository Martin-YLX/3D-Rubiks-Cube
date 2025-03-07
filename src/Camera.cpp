#include "Camera.h"
#include "Cube.h"  // 包含旋转函数调用
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

glm::vec3 grayColor(0.1f, 0.1f, 0.1f);
glm::vec3 standardColors[6] = {
    glm::vec3(1.0f, 0.0f, 0.0f),   // 前 (红)
    glm::vec3(1.0f, 0.5f, 0.0f),   // 后 (橙)
    glm::vec3(0.0f, 0.0f, 1.0f),   // 左 (蓝)
    glm::vec3(0.0f, 1.0f, 0.0f),   // 右 (绿)
    glm::vec3(1.0f, 1.0f, 1.0f),   // 上 (白)
    glm::vec3(1.0f, 1.0f, 0.0f)    // 下 (黄)
};
glm::vec3 target(0.0f, 0.0f, 0.0f);
float radius = 10.0f;
float yaw = 45.0f;
float pitch = 30.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
bool firstMouse = true;
bool cameraFollow = false;

glm::vec3 computeCameraPos() {
    glm::vec3 pos;
    pos.x = target.x + radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    pos.y = target.y + radius * sin(glm::radians(pitch));
    pos.z = target.z + radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return pos;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!cameraFollow)
        return;
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // C 键控制摄像机跟随
    if (key == GLFW_KEY_C) {
        if (action == GLFW_PRESS) {
            cameraFollow = true;
        } else if (action == GLFW_RELEASE) {
            cameraFollow = false;
            firstMouse = true;
        }
    }
    // 旋转指令
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_R) {
            rotateR();
        } else if (key == GLFW_KEY_L) {
            rotateL();
        } else if (key == GLFW_KEY_U) {
            rotateU();
        } else if (key == GLFW_KEY_D) {
            rotateD();
        } else if (key == GLFW_KEY_F) {
            rotateF();
        } else if (key == GLFW_KEY_B) {
            rotateB();
        }
    }
}