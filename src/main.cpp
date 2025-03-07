#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// 窗口尺寸
const unsigned int WIDTH = 800, HEIGHT = 600;

// 全局魔方小块尺寸与间隙
float cubeSize = 0.9f;
float gap = 0.1f;

//--------------------- 魔方状态 ---------------------
/* 颜色代码定义：
   0: 前（红色）
   1: 后（橙色）
   2: 左（蓝色）
   3: 右（绿色）
   4: 上（白色）
   5: 下（黄色）
   -1: 内部（用暗灰色显示）
*/
struct Cubie {
    int colors[6]; // 顺序：0-front, 1-back, 2-left, 3-right, 4-up, 5-down
};

std::vector<std::vector<std::vector<Cubie>>> cubeState;

void initCubeState() {
    cubeState.resize(3, std::vector<std::vector<Cubie>>(3, std::vector<Cubie>(3)));
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            for (int k = 0; k < 3; k++){
                // 默认内部
                for (int f = 0; f < 6; f++)
                    cubeState[i][j][k].colors[f] = -1;
                if (k == 2) cubeState[i][j][k].colors[0] = 0; // 前面：红色
                if (k == 0) cubeState[i][j][k].colors[1] = 1; // 后面：橙色
                if (i == 0) cubeState[i][j][k].colors[2] = 2; // 左面：蓝色
                if (i == 2) cubeState[i][j][k].colors[3] = 3; // 右面：绿色
                if (j == 2) cubeState[i][j][k].colors[4] = 4; // 上面：白色
                if (j == 0) cubeState[i][j][k].colors[5] = 5; // 底面：黄色
            }
        }
    }
}

glm::vec3 standardColors[6] = {
    glm::vec3(1.0f, 0.0f, 0.0f),   // 红
    glm::vec3(1.0f, 0.5f, 0.0f),   // 橙
    glm::vec3(0.0f, 0.0f, 1.0f),   // 蓝
    glm::vec3(0.0f, 1.0f, 0.0f),   // 绿
    glm::vec3(1.0f, 1.0f, 1.0f),   // 白
    glm::vec3(1.0f, 1.0f, 0.0f)    // 黄
};
glm::vec3 grayColor = glm::vec3(0.1f, 0.1f, 0.1f);

//--------------------- 摄像机（轨道） ---------------------
glm::vec3 target(0.0f, 0.0f, 0.0f);
float radius = 10.0f;
// 修改默认角度，使默认显示前、右、上三个面
float yaw   = 45.0f;    // 默认 45°
float pitch = 30.0f;    // 默认 30°

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;
bool cameraFollow = false;  // 按下 "C" 时启用跟随

// 计算摄像机位置（球坐标转笛卡尔）
glm::vec3 computeCameraPos() {
    glm::vec3 pos;
    pos.x = target.x + radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    pos.y = target.y + radius * sin(glm::radians(pitch));
    pos.z = target.z + radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return pos;
}

//--------------------- 回调 ---------------------

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

    yaw   += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

//--------------------- 辅助：旋转 cubie 的方向 ---------------------
// 绕 X 轴旋转 90°（顺时针：当从正 x 方向看）
Cubie rotateCubieAroundX90(const Cubie &c, bool clockwise) {
    Cubie res = c;
    if (clockwise) {
        res.colors[0] = c.colors[4]; // front = old up
        res.colors[4] = c.colors[1]; // up = old back
        res.colors[1] = c.colors[5]; // back = old down
        res.colors[5] = c.colors[0]; // down = old front
    } else {
        res.colors[0] = c.colors[5]; // front = old down
        res.colors[5] = c.colors[1]; // down = old back
        res.colors[1] = c.colors[4]; // back = old up
        res.colors[4] = c.colors[0]; // up = old front
    }
    return res;
}

// 绕 Y 轴旋转 90°（顺时针：当从正 y 方向看）
Cubie rotateCubieAroundY90(const Cubie &c, bool clockwise) {
    Cubie res = c;
    if (clockwise) {
        res.colors[0] = c.colors[2]; // front = old left
        res.colors[2] = c.colors[1]; // left = old back
        res.colors[1] = c.colors[3]; // back = old right
        res.colors[3] = c.colors[0]; // right = old front
    } else {
        res.colors[0] = c.colors[3]; // front = old right
        res.colors[3] = c.colors[1]; // right = old back
        res.colors[1] = c.colors[2]; // back = old left
        res.colors[2] = c.colors[0]; // left = old front
    }
    return res;
}

// 绕 Z 轴旋转 90°（顺时针：当从正 z 方向看）
Cubie rotateCubieAroundZ90(const Cubie &c, bool clockwise) {
    Cubie res = c;
    if (clockwise) {
        res.colors[4] = c.colors[2]; // up = old left
        res.colors[2] = c.colors[5]; // left = old down
        res.colors[5] = c.colors[3]; // down = old right
        res.colors[3] = c.colors[4]; // right = old up
    } else {
        res.colors[4] = c.colors[3]; // up = old right
        res.colors[3] = c.colors[5]; // right = old down
        res.colors[5] = c.colors[2]; // down = old left
        res.colors[2] = c.colors[4]; // left = old up
    }
    return res;
}

//--------------------- 对指定层进行 3×3 矩阵旋转 ---------------------
// 旋转右层（i==2），从正右侧看，顺时针旋转
void rotateR() {
    // 更新颜色方向
    Cubie temp[3][3];
    for (int j = 0; j < 3; j++){
        for (int k = 0; k < 3; k++){
            Cubie c = cubeState[2][j][k];
            Cubie updated = c;
            updated.colors[0] = c.colors[5]; // front = old down
            updated.colors[4] = c.colors[0]; // up = old front
            updated.colors[1] = c.colors[4]; // back = old up
            updated.colors[5] = c.colors[1]; // down = old back
            temp[j][k] = updated;
        }
    }
    // 更新二维位置：新[j][k] = old[2 - k][j]
    Cubie newLayer[3][3];
    for (int j = 0; j < 3; j++){
        for (int k = 0; k < 3; k++){
            newLayer[j][k] = temp[2 - k][j];
        }
    }
    for (int j = 0; j < 3; j++)
        for (int k = 0; k < 3; k++)
            cubeState[2][j][k] = newLayer[j][k];
}

// 旋转左层（i==0），从正左侧看，顺时针（等效于对正 x 轴反向旋转）
void rotateL() {
    Cubie temp[3][3];
    for (int j = 0; j < 3; j++){
        for (int k = 0; k < 3; k++){
            temp[j][k] = rotateCubieAroundX90(cubeState[0][j][k], false);
        }
    }
    for (int j = 0; j < 3; j++){
        for (int k = 0; k < 3; k++){
            cubeState[0][j][k] = temp[2 - k][j];
        }
    }
}

// 旋转上层（j==2），从正上方看，顺时针旋转
void rotateU() {
    Cubie temp[3][3];
    for (int i = 0; i < 3; i++){
        for (int k = 0; k < 3; k++){
            temp[i][k] = rotateCubieAroundY90(cubeState[i][2][k], true);
        }
    }
    for (int i = 0; i < 3; i++){
        for (int k = 0; k < 3; k++){
            cubeState[i][2][k] = temp[2 - k][i];
        }
    }
}

// 旋转下层（j==0），从正下方看，顺时针旋转
void rotateD() {
    Cubie temp[3][3];
    for (int i = 0; i < 3; i++){
        for (int k = 0; k < 3; k++){
            Cubie c = cubeState[i][0][k];
            Cubie updated = c;
            updated.colors[0] = c.colors[3]; // front = old right
            updated.colors[3] = c.colors[1]; // right = old back
            updated.colors[1] = c.colors[2]; // back = old left
            updated.colors[2] = c.colors[0]; // left = old front
            temp[i][k] = updated;
        }
    }
    Cubie newLayer[3][3];
    for (int i = 0; i < 3; i++){
        for (int k = 0; k < 3; k++){
            // 逆时针旋转公式：new[i][k] = old[k][2 - i]
            newLayer[i][k] = temp[k][2 - i];
        }
    }
    for (int i = 0; i < 3; i++)
        for (int k = 0; k < 3; k++)
            cubeState[i][0][k] = newLayer[i][k];
}

// 旋转前层（k==2），从正前方看，顺时针旋转
void rotateF() {
    Cubie temp[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            temp[i][j] = rotateCubieAroundZ90(cubeState[i][j][2], true);
        }
    }
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            cubeState[i][j][2] = temp[2 - j][i];
        }
    }
}

// 旋转后层（k==0），从正后方看，顺时针旋转
void rotateB() {
    Cubie temp[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            Cubie c = cubeState[i][j][0];
            Cubie updated = c;
            updated.colors[4] = c.colors[3]; // up = old right
            updated.colors[3] = c.colors[5]; // right = old down
            updated.colors[5] = c.colors[2]; // down = old left
            updated.colors[2] = c.colors[4]; // left = old up
            temp[i][j] = updated;
        }
    }
    Cubie newLayer[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            // 逆时针旋转公式：new[i][j] = old[j][2 - i]
            newLayer[i][j] = temp[j][2 - i];
        }
    }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            cubeState[i][j][0] = newLayer[i][j];
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // C 键控制摄像机跟随
    if (key == GLFW_KEY_C) {
        if (action == GLFW_PRESS) {
            cameraFollow = true;
        } else if (action == GLFW_RELEASE) {
            cameraFollow = false;
            firstMouse = true;
        }
    }
    // 旋转指令，按下对应键时更新对应层（直接调用更新函数）
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_R) {
            // 右层旋转
            // 注意：右层为 i==2
            // 调用后将更新 cubeState[2] 层（包括位置与方向）
            // 此处使用顺时针旋转（当从正右侧看时）
            // 具体实现见下面的 rotateR()
            // 调用：rotateR();
            // 这里直接调用函数：
            //（为了简单起见，这里只实现 R move，其他类似）
            // 若需要全部实现，请参照下述模式自行扩展。
            // 下面给出完整 R/L/U/D/F/B 的实现（见后文）
            // 在此示例中，我们直接调用 rotateR();
            // （请参考后面完整代码）
            // 以下调用示例：
            rotateR();
        }
        else if (key == GLFW_KEY_L) { rotateL(); }
        else if (key == GLFW_KEY_U) { rotateU(); }
        else if (key == GLFW_KEY_D) { rotateD(); }
        else if (key == GLFW_KEY_F) { rotateF(); }
        else if (key == GLFW_KEY_B) { rotateB(); }
    }
}

//--------------------- 着色器及辅助函数 ---------------------
std::string readShaderSource(const std::string &filepath) {
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "无法打开着色器文件: " << filepath << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = readShaderSource(vertexPath);
    std::string fragmentCode = readShaderSource(fragmentPath);
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR: 顶点着色器编译失败\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR: 片段着色器编译失败\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR: 着色器程序链接失败\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

//--------------------- 绘制几何数据 ---------------------
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

//--------------------- 局部变换 ---------------------
// 定义每个小方块 6 个面在局部空间的变换（顺序：前、后、左、右、顶、底）
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

//--------------------- 主函数 ---------------------
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "3DCube - 魔方状态更新", nullptr, nullptr);
    if (!window) {
        std::cerr << "窗口创建失败" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD 初始化失败" << std::endl;
        return -1;
    }
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    unsigned int shaderProgram = createShaderProgram("shader.vert", "shader.frag");

    // 设置四边形 VAO/VBO/EBO，用于绘制单个面
    unsigned int quadVAO, quadVBO, quadEBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    initFaceTransforms();
    initCubeState();

    int uniModel = glGetUniformLocation(shaderProgram, "model");
    int uniView  = glGetUniformLocation(shaderProgram, "view");
    int uniProj  = glGetUniformLocation(shaderProgram, "projection");
    int uniFaceColor = glGetUniformLocation(shaderProgram, "faceColor");

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH/HEIGHT, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glViewport(0, 0, WIDTH, HEIGHT);

        glm::vec3 camPos = computeCameraPos();
        glm::mat4 view = glm::lookAt(camPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));

        // 绘制魔方：遍历 3×3×3 cubie
        for (int i = 0; i < 3; i++){
            for (int j = 0; j < 3; j++){
                for (int k = 0; k < 3; k++){
                    float offset = cubeSize + gap;
                    float x = (i - 1) * offset;
                    float y = (j - 1) * offset;
                    float z = (k - 1) * offset;
                    glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                    cubeModel = glm::scale(cubeModel, glm::vec3(cubeSize));

                    for (int face = 0; face < 6; face++){
                        int colCode = cubeState[i][j][k].colors[face];
                        glm::vec3 color = (colCode == -1) ? grayColor : standardColors[colCode];
                        glm::mat4 finalModel = cubeModel * faceTransforms[face];
                        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(finalModel));
                        glUniform3fv(uniFaceColor, 1, glm::value_ptr(color));

                        glBindVertexArray(quadVAO);
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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