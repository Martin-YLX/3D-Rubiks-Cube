#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

// 魔方状态
struct CubeFace {
    glm::vec3 color;
    bool visible;
};

struct SmallCube {
    CubeFace faces[6];  // 0:左, 1:右, 2:下, 3:上, 4:前, 5:后
};

std::vector<std::vector<std::vector<SmallCube>>> cube(3, std::vector<std::vector<SmallCube>>(3, std::vector<SmallCube>(3)));
bool keyPressed[6] = {false};  // 用于跟踪按键状态

// 初始化魔方颜色
void initCubeColors() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                // 初始化所有面为不可见
                for (int f = 0; f < 6; f++) {
                    cube[i][j][k].faces[f].visible = false;
                    cube[i][j][k].faces[f].color = glm::vec3(0.0f, 0.0f, 0.0f);
                }
                
                // 设置可见面的颜色
                // 左面
                if (i == 0) {
                    cube[i][j][k].faces[0].visible = true;
                    cube[i][j][k].faces[0].color = glm::vec3(1.0f, 0.0f, 0.0f);  // 红色
                }
                // 右面
                if (i == 2) {
                    cube[i][j][k].faces[1].visible = true;
                    cube[i][j][k].faces[1].color = glm::vec3(1.0f, 0.5f, 0.0f);  // 橙色
                }
                // 下面
                if (j == 0) {
                    cube[i][j][k].faces[2].visible = true;
                    cube[i][j][k].faces[2].color = glm::vec3(0.0f, 0.0f, 1.0f);  // 蓝色
                }
                // 上面
                if (j == 2) {
                    cube[i][j][k].faces[3].visible = true;
                    cube[i][j][k].faces[3].color = glm::vec3(0.0f, 1.0f, 0.0f);  // 绿色
                }
                // 前面
                if (k == 0) {
                    cube[i][j][k].faces[4].visible = true;
                    cube[i][j][k].faces[4].color = glm::vec3(1.0f, 1.0f, 1.0f);  // 白色
                }
                // 后面
                if (k == 2) {
                    cube[i][j][k].faces[5].visible = true;
                    cube[i][j][k].faces[5].color = glm::vec3(1.0f, 0.9f, 0.0f);  // 黄色
                }
            }
        }
    }
}

// 辅助函数：旋转一个小立方体的面 (已修正)
void rotateSmallCubeFaces(SmallCube& cube, char axis, bool clockwise) {
    CubeFace tempFaces[6];
    // tempFaces 存储旋转前的状态
    for(int i=0; i<6; ++i) tempFaces[i] = cube.faces[i];

    if (axis == 'y') { // U/D rotations (around Y axis)
        if (clockwise) { // Physical CW rotation around +Y
            cube.faces[0] = tempFaces[4]; // New Left <- Old Front
            cube.faces[1] = tempFaces[5]; // New Right <- Old Back
            cube.faces[4] = tempFaces[1]; // New Front <- Old Right
            cube.faces[5] = tempFaces[0]; // New Back <- Old Left
            // Up(3) and Down(2) stay the same relative to the cubelet
        } else { // Physical CCW rotation around +Y
            cube.faces[0] = tempFaces[5]; // New Left <- Old Back
            cube.faces[1] = tempFaces[4]; // New Right <- Old Front
            cube.faces[4] = tempFaces[0]; // New Front <- Old Left
            cube.faces[5] = tempFaces[1]; // New Back <- Old Right
            // Up(3) and Down(2) stay the same relative to the cubelet
        }
    } else if (axis == 'x') { // L/R rotations (around X axis)
        if (clockwise) { // Physical CW rotation around +X
            cube.faces[2] = tempFaces[5]; // New Down <- Old Back
            cube.faces[3] = tempFaces[4]; // New Up <- Old Front
            cube.faces[4] = tempFaces[2]; // New Front <- Old Down
            cube.faces[5] = tempFaces[3]; // New Back <- Old Up
            // Left(0) and Right(1) stay the same relative to the cubelet
        } else { // Physical CCW rotation around +X
            cube.faces[2] = tempFaces[4]; // New Down <- Old Front
            cube.faces[3] = tempFaces[5]; // New Up <- Old Back
            cube.faces[4] = tempFaces[3]; // New Front <- Old Up
            cube.faces[5] = tempFaces[2]; // New Back <- Old Down
            // Left(0) and Right(1) stay the same relative to the cubelet
        }
    } else if (axis == 'z') { // F/B rotations (around Z axis)
        if (clockwise) { // Physical CW rotation around +Z
            cube.faces[0] = tempFaces[2]; // New Left <- Old Down
            cube.faces[1] = tempFaces[3]; // New Right <- Old Up
            cube.faces[2] = tempFaces[1]; // New Down <- Old Right
            cube.faces[3] = tempFaces[0]; // New Up <- Old Left
            // Front(4) and Back(5) stay the same relative to the cubelet
        } else { // Physical CCW rotation around +Z
            cube.faces[0] = tempFaces[3]; // New Left <- Old Up
            cube.faces[1] = tempFaces[2]; // New Right <- Old Down
            cube.faces[2] = tempFaces[0]; // New Down <- Old Left
            cube.faces[3] = tempFaces[1]; // New Up <- Old Right
            // Front(4) and Back(5) stay the same relative to the cubelet
        }
    }
}

// 旋转魔方的面
void rotateFace(char face, bool clockwise) {
    std::vector<std::vector<SmallCube>> temp(3, std::vector<SmallCube>(3));
    
    switch (face) {
        case 'u': // 上层面 (Y轴)
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (clockwise)
                        temp[j][2-i] = cube[i][2][j];
                    else
                        temp[2-j][i] = cube[i][2][j];
                }
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    cube[i][2][j] = temp[i][j];
                    rotateSmallCubeFaces(cube[i][2][j], 'y', clockwise);
                }
            }
            break;
            
        case 'd': // 下层面 (Y轴)
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    // 注意：D旋转相对于U旋转是反方向的物理旋转，但我们希望顺时针按键对应顺时针观察效果
                    if (clockwise)
                        temp[j][2-i] = cube[i][0][j]; // 逻辑同U
                    else
                        temp[2-j][i] = cube[i][0][j]; // 逻辑同U
                }
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    cube[i][0][j] = temp[i][j];
                    // 旋转方向与观察方向一致，但轴向相反，所以传入 !clockwise
                    rotateSmallCubeFaces(cube[i][0][j], 'y', !clockwise);
                }
            }
            break;
            
        case 'l': // 左层面 (X轴)
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (clockwise)
                        temp[j][2-i] = cube[0][i][j];
                    else
                        temp[2-j][i] = cube[0][i][j];
                }
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    cube[0][i][j] = temp[i][j];
                     // 旋转方向与观察方向一致，但轴向相反，所以传入 !clockwise
                    rotateSmallCubeFaces(cube[0][i][j], 'x', !clockwise);
                }
            }
            break;
            
        case 'r': // 右层面 (X轴)
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (clockwise)
                        temp[j][2-i] = cube[2][i][j];
                    else
                        temp[2-j][i] = cube[2][i][j];
                }
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    cube[2][i][j] = temp[i][j];
                    rotateSmallCubeFaces(cube[2][i][j], 'x', clockwise);
                }
            }
            break;
            
        case 'f': // 前层面 (Z轴)
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (clockwise)
                        temp[j][2-i] = cube[i][j][0];
                    else
                        temp[2-j][i] = cube[i][j][0];
                }
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    cube[i][j][0] = temp[i][j];
                    rotateSmallCubeFaces(cube[i][j][0], 'z', clockwise);
                }
            }
            break;
            
        case 'b': // 后层面 (Z轴)
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (clockwise)
                        temp[j][2-i] = cube[i][j][2];
                    else
                        temp[2-j][i] = cube[i][j][2];
                }
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    cube[i][j][2] = temp[i][j];
                     // 旋转方向与观察方向一致，但轴向相反，所以传入 !clockwise
                    rotateSmallCubeFaces(cube[i][j][2], 'z', !clockwise);
                }
            }
            break;
    }
}

// 生成小立方体的顶点数据
void generateCubeVertices(float x, float y, float z, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    float size = 0.3f;      // 小立方体的大小
    float offset = 0.01f;   // 小立方体之间的间距
    
    // 计算小立方体的位置（相对于球心）
    float posX = (x - 1) * (size + offset);
    float posY = (y - 1) * (size + offset);
    float posZ = (z - 1) * (size + offset);
    
    // 获取小立方体的颜色
    glm::vec3 colors[6];
    for (int f = 0; f < 6; f++) {
        if (cube[x][y][z].faces[f].visible)
            colors[f] = cube[x][y][z].faces[f].color;
        else
            colors[f] = glm::vec3(0.0f, 0.0f, 0.0f);  // 黑色
    }
    
    // 生成顶点数据（包含位置和颜色）
    float cubeVertices[] = {
        // 前面
        posX - size/2, posY - size/2, posZ - size/2, colors[4].x, colors[4].y, colors[4].z,  // 左下
        posX + size/2, posY - size/2, posZ - size/2, colors[4].x, colors[4].y, colors[4].z,  // 右下
        posX + size/2, posY + size/2, posZ - size/2, colors[4].x, colors[4].y, colors[4].z,  // 右上
        posX - size/2, posY + size/2, posZ - size/2, colors[4].x, colors[4].y, colors[4].z,  // 左上
        
        // 后面
        posX - size/2, posY - size/2, posZ + size/2, colors[5].x, colors[5].y, colors[5].z,  // 左下
        posX + size/2, posY - size/2, posZ + size/2, colors[5].x, colors[5].y, colors[5].z,  // 右下
        posX + size/2, posY + size/2, posZ + size/2, colors[5].x, colors[5].y, colors[5].z,  // 右上
        posX - size/2, posY + size/2, posZ + size/2, colors[5].x, colors[5].y, colors[5].z,  // 左上
        
        // 左面
        posX - size/2, posY - size/2, posZ - size/2, colors[0].x, colors[0].y, colors[0].z,  // 左下
        posX - size/2, posY - size/2, posZ + size/2, colors[0].x, colors[0].y, colors[0].z,  // 右下
        posX - size/2, posY + size/2, posZ + size/2, colors[0].x, colors[0].y, colors[0].z,  // 右上
        posX - size/2, posY + size/2, posZ - size/2, colors[0].x, colors[0].y, colors[0].z,  // 左上
        
        // 右面
        posX + size/2, posY - size/2, posZ - size/2, colors[1].x, colors[1].y, colors[1].z,  // 左下
        posX + size/2, posY - size/2, posZ + size/2, colors[1].x, colors[1].y, colors[1].z,  // 右下
        posX + size/2, posY + size/2, posZ + size/2, colors[1].x, colors[1].y, colors[1].z,  // 右上
        posX + size/2, posY + size/2, posZ - size/2, colors[1].x, colors[1].y, colors[1].z,  // 左上
        
        // 上面
        posX - size/2, posY + size/2, posZ - size/2, colors[3].x, colors[3].y, colors[3].z,  // 左下
        posX + size/2, posY + size/2, posZ - size/2, colors[3].x, colors[3].y, colors[3].z,  // 右下
        posX + size/2, posY + size/2, posZ + size/2, colors[3].x, colors[3].y, colors[3].z,  // 右上
        posX - size/2, posY + size/2, posZ + size/2, colors[3].x, colors[3].y, colors[3].z,  // 左上
        
        // 下面
        posX - size/2, posY - size/2, posZ - size/2, colors[2].x, colors[2].y, colors[2].z,  // 左下
        posX + size/2, posY - size/2, posZ - size/2, colors[2].x, colors[2].y, colors[2].z,  // 右下
        posX + size/2, posY - size/2, posZ + size/2, colors[2].x, colors[2].y, colors[2].z,  // 右上
        posX - size/2, posY - size/2, posZ + size/2, colors[2].x, colors[2].y, colors[2].z   // 左上
    };
    
    // 生成索引数据（每个面两个三角形）
    unsigned int cubeIndices[] = {
        // 前面
        0, 1, 2, 2, 3, 0,
        // 后面
        4, 5, 6, 6, 7, 4,
        // 左面
        8, 9, 10, 10, 11, 8,
        // 右面
        12, 13, 14, 14, 15, 12,
        // 上面
        16, 17, 18, 18, 19, 16,
        // 下面
        20, 21, 22, 22, 23, 20
    };
    
    // 添加到总顶点和索引数据中
    size_t baseIndex = vertices.size() / 6;
    for (int i = 0; i < 24 * 6; i++) {
        vertices.push_back(cubeVertices[i]);
    }
    for (int i = 0; i < 36; i++) {
        indices.push_back(cubeIndices[i] + baseIndex);
    }
}

// 着色器编译函数
unsigned int compileShader(const char* vertexPath, const char* fragmentPath) {
    // 读取着色器代码
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
        std::cout << "ERROR: Failed to open shader files" << std::endl;
        return 0;
    }
    
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    vShaderFile.close();
    fShaderFile.close();
    
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    
    // 编译顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    
    // 检查顶点着色器编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR: Vertex shader compilation failed\n" << infoLog << std::endl;
    }
    
    // 编译片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    
    // 检查片段着色器编译错误
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR: Fragment shader compilation failed\n" << infoLog << std::endl;
    }
    
    // 创建着色器程序
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // 检查链接错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR: Shader program linking failed\n" << infoLog << std::endl;
    }
    
    // 删除着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

// 相机参数
float radius = 5.0f;  // 相机到球心的距离
float theta = 0.0f;   // 水平角度
float phi = glm::radians(45.0f);  // 垂直角度
glm::vec3 cameraPos;
glm::vec3 cameraFront;
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;
float sensitivity = 0.1f;  // 鼠标灵敏度

// 更新相机位置
void updateCamera() {
    // 计算球面坐标
    cameraPos.x = radius * sin(phi) * cos(theta);
    cameraPos.y = radius * cos(phi);
    cameraPos.z = radius * sin(phi) * sin(theta);
    
    // 计算相机朝向（始终看向原点）
    cameraFront = glm::normalize(-cameraPos);
    
    // 更新上向量
    glm::vec3 right = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    cameraUp = glm::normalize(glm::cross(right, cameraFront));
}

// 鼠标回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // 反转y轴
    lastX = xpos;
    lastY = ypos;
    
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    theta += glm::radians(xoffset);
    phi += glm::radians(yoffset);
    
    // 限制垂直角度，防止相机翻转
    if (phi > glm::radians(179.0f)) phi = glm::radians(179.0f);
    if (phi < glm::radians(1.0f)) phi = glm::radians(1.0f);
    
    updateCamera();
}

int main() {
    // 初始化GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "Rubik's Cube", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 检查OpenGL版本
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  // 设置深度测试函数
    glDisable(GL_CULL_FACE);  // 禁用面剔除，确保所有面都可见

    // 设置视口
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // 初始化魔方颜色
    initCubeColors();

    // 设置鼠标回调
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 编译着色器
    std::string vertexPath = std::string(SRC_DIR) + "/shader.vert";
    std::string fragmentPath = std::string(SRC_DIR) + "/shader.frag";
    unsigned int shaderProgram = compileShader(vertexPath.c_str(), fragmentPath.c_str());

    // 生成所有小立方体的顶点和索引数据
    std::vector<float> allVertices;
    std::vector<unsigned int> allIndices;
    
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            for (int z = 0; z < 3; z++) {
                generateCubeVertices(x, y, z, allVertices, allIndices);
            }
        }
    }

    // 设置顶点缓冲对象和顶点数组对象
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(float), allVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, allIndices.size() * sizeof(unsigned int), allIndices.data(), GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 初始化相机位置
    updateCamera();

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        
        // 检查按键状态并执行旋转
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !keyPressed[0]) {
            rotateFace('u', !shiftPressed);
            keyPressed[0] = true;
        } else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
            keyPressed[0] = false;
        }
        
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !keyPressed[1]) {
            rotateFace('d', !shiftPressed);
            keyPressed[1] = true;
        } else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
            keyPressed[1] = false;
        }
        
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !keyPressed[2]) {
            rotateFace('l', !shiftPressed);
            keyPressed[2] = true;
        } else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
            keyPressed[2] = false;
        }
        
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !keyPressed[3]) {
            rotateFace('r', !shiftPressed);
            keyPressed[3] = true;
        } else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) {
            keyPressed[3] = false;
        }
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !keyPressed[4]) {
            rotateFace('f', !shiftPressed);
            keyPressed[4] = true;
        } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
            keyPressed[4] = false;
        }
        
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !keyPressed[5]) {
            rotateFace('b', !shiftPressed);
            keyPressed[5] = true;
        } else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE) {
            keyPressed[5] = false;
        }

        // 重新生成顶点数据
        allVertices.clear();
        allIndices.clear();
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
                for (int z = 0; z < 3; z++) {
                    generateCubeVertices(x, y, z, allVertices, allIndices);
                }
            }
        }
        
        // 更新顶点缓冲数据
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(float), allVertices.data(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, allIndices.size() * sizeof(unsigned int), allIndices.data(), GL_STATIC_DRAW);

        // 获取窗口大小
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // 创建视图矩阵
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
        
        // 创建投影矩阵
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        
        // 使用着色器程序
        glUseProgram(shaderProgram);
        
        // 设置uniform变量
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));  // 恢复原始大小
        
        // 获取uniform位置
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
        
        // 设置uniform值
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // 渲染
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绘制魔方
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, allIndices.size(), GL_UNSIGNED_INT, 0);

        // 检查OpenGL错误
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cout << "OpenGL Error: " << err << std::endl;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
} 