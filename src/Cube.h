#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <glm/glm.hpp>

struct Cubie {
    // 颜色顺序：0-front, 1-back, 2-left, 3-right, 4-up, 5-down
    int colors[6];
};

extern std::vector<std::vector<std::vector<Cubie>>> cubeState;

void initCubeState();

// 旋转函数（更新魔方状态，包括颜色方向和二维位置交换）
void rotateR();
void rotateL();
void rotateU();
void rotateD();
void rotateF();
void rotateB();

#endif // CUBE_H