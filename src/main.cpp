#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Cube.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

float cubeSize = 0.9f;
float gap = 0.1f;

// 顶点数据，用于绘制单个面
float quadVertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f
};
unsigned int quadIndices[] = {
    0, 1, 2,
    2, 3, 0
};

// 局部变换（面Transforms），用于将一个四边形移动到 cubie 的对应面
std::vector<glm::mat4> faceTransforms;
void initFaceTransforms() {
    faceTransforms.clear();
    glm::mat4 identity = glm::mat4(1.0f);
    glm::mat4 front = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.5f));
    glm::mat4 back = glm::rotate(identity, glm::radians(180.0f), glm::vec3(0,1,0));
    back = glm::translate(back, glm::vec3(0.0f, 0.0f, 0.5f));
    glm::mat4 left = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(0,1,0));
    left = glm::translate(left, glm::vec3(0.0f, 0.0f, 0.5f));
    glm::mat4 right = glm::rotate(identity, glm::radians(90.0f), glm::vec3(0,1,0));
    right = glm::translate(right, glm::vec3(0.0f, 0.0f, 0.5f));
    glm::mat4 top = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(1,0,0));
    top = glm::translate(top, glm::vec3(0.0f, 0.0f, 0.5f));
    glm::mat4 bottom = glm::rotate(identity, glm::radians(90.0f), glm::vec3(1,0,0));
    bottom = glm::translate(bottom, glm::vec3(0.0f, 0.0f, 0.5f));
    faceTransforms.push_back(front);
    faceTransforms.push_back(back);
    faceTransforms.push_back(left);
    faceTransforms.push_back(right);
    faceTransforms.push_back(top);
    faceTransforms.push_back(bottom);
}

int main() {
    if(!glfwInit()){
        std::cerr << "GLFW 初始化失败" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800,600,"3DCube",nullptr,nullptr);
    if(!window){
        std::cerr << "窗口创建失败" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "GLAD 初始化失败" << std::endl;
        return -1;
    }
    glViewport(0,0,800,600);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f,0.2f,0.2f,1.0f);

    unsigned int shaderProgram = createShaderProgram("shader.vert", "shader.frag");

    // 设置VAO/VBO/EBO用于绘制四边形
    unsigned int quadVAO, quadVBO, quadEBO;
    glGenVertexArrays(1,&quadVAO);
    glGenBuffers(1,&quadVBO);
    glGenBuffers(1,&quadEBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    initFaceTransforms();
    initCubeState();

    int uniModel = glGetUniformLocation(shaderProgram, "model");
    int uniView = glGetUniformLocation(shaderProgram, "view");
    int uniProj = glGetUniformLocation(shaderProgram, "projection");
    int uniFaceColor = glGetUniformLocation(shaderProgram, "faceColor");

    glm::mat4 projection = glm::perspective(glm::radians(45.0f),(float)800/600,0.1f,100.0f);

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glViewport(0,0,800,600);

        glm::vec3 camPos = computeCameraPos();
        glm::mat4 view = glm::lookAt(camPos, target, glm::vec3(0,1,0));
        glUniformMatrix4fv(uniView,1,GL_FALSE,glm::value_ptr(view));
        glUniformMatrix4fv(uniProj,1,GL_FALSE,glm::value_ptr(projection));

        // 遍历 3×3×3 cubie
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                for(int k=0;k<3;k++){
                    float offset = cubeSize + gap;
                    float x = (i-1)*offset;
                    float y = (j-1)*offset;
                    float z = (k-1)*offset;
                    glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(x,y,z));
                    cubeModel = glm::scale(cubeModel, glm::vec3(cubeSize));

                    for(int face=0;face<6;face++){
                        int colCode = cubeState[i][j][k].colors[face];
                        glm::vec3 color = (colCode == -1)? grayColor: standardColors[colCode];
                        glm::mat4 finalModel = cubeModel * faceTransforms[face];
                        glUniformMatrix4fv(uniModel,1,GL_FALSE,glm::value_ptr(finalModel));
                        glUniform3fv(uniFaceColor,1,glm::value_ptr(color));
                        glBindVertexArray(quadVAO);
                        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
                    }
                }
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}