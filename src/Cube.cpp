#include "Cube.h"
#include <vector>

// 全局魔方状态：cubeState[3][3][3]
std::vector<std::vector<std::vector<Cubie>>> cubeState;

// 初始化魔方，外层赋予标准颜色，内部 (-1) 灰色
void initCubeState() {
    cubeState.resize(3, std::vector<std::vector<Cubie>>(3, std::vector<Cubie>(3)));
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            for (int k = 0; k < 3; k++){
                // 默认内部
                for (int f = 0; f < 6; f++)
                    cubeState[i][j][k].colors[f] = -1;
                if (k == 2) cubeState[i][j][k].colors[0] = 0; // 前 (红)
                if (k == 0) cubeState[i][j][k].colors[1] = 1; // 后 (橙)
                if (i == 0) cubeState[i][j][k].colors[2] = 2; // 左 (蓝)
                if (i == 2) cubeState[i][j][k].colors[3] = 3; // 右 (绿)
                if (j == 2) cubeState[i][j][k].colors[4] = 4; // 上 (白)
                if (j == 0) cubeState[i][j][k].colors[5] = 5; // 下 (黄)
            }
        }
    }
}

/*
   颜色下标定义：
   0-front, 1-back, 2-left, 3-right, 4-up, 5-down
   -1 表示内部 (灰色)

   每个函数分两步：
   1) 更新该层 cubie 的颜色排列 (面方向)
   2) 在该层的 3×3 网格中做二维旋转
*/

/** 右层旋转 (R move) **/
void rotateR() {
    // i==2 固定 (右层)；局部坐标 (j, k)
    // 颜色更新规则 (从右侧看顺时针)：
    //   new_front = old_down
    //   new_up    = old_front
    //   new_back  = old_up
    //   new_down  = old_back
    Cubie temp[3][3];
    for (int j = 0; j < 3; j++){
        for (int k = 0; k < 3; k++){
            Cubie c = cubeState[2][j][k];
            Cubie updated = c;
            updated.colors[0] = c.colors[5]; // front = old down
            updated.colors[4] = c.colors[0]; // up    = old front
            updated.colors[1] = c.colors[4]; // back  = old up
            updated.colors[5] = c.colors[1]; // down  = old back
            temp[j][k] = updated;
        }
    }
    // 位置更新：new[j][k] = old[2 - k][j] (标准 90° 顺时针)
    Cubie newLayer[3][3];
    for (int j = 0; j < 3; j++){
        for (int k = 0; k < 3; k++){
            newLayer[j][k] = temp[2 - k][j];
        }
    }
    // 写回
    for (int j = 0; j < 3; j++){
        for (int k = 0; k < 3; k++){
            cubeState[2][j][k] = newLayer[j][k];
        }
    }
}

/** 后层旋转 (B move) **/
void rotateB() {
    // k==0 固定 (后层)；局部坐标 (i, j)
    // 颜色更新规则 (从后面看顺时针)：
    //   new_up    = old_right
    //   new_right = old_down
    //   new_down  = old_left
    //   new_left  = old_up
    Cubie temp[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            Cubie c = cubeState[i][j][0];
            Cubie updated = c;
            updated.colors[4] = c.colors[3]; // up    = old right
            updated.colors[3] = c.colors[5]; // right = old down
            updated.colors[5] = c.colors[2]; // down  = old left
            updated.colors[2] = c.colors[4]; // left  = old up
            temp[i][j] = updated;
        }
    }
    // 位置更新：new[i][j] = old[j][2 - i] (在全局中是逆时针，但从后面看是顺时针)
    Cubie newLayer[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            newLayer[i][j] = temp[j][2 - i];
        }
    }
    // 写回
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            cubeState[i][j][0] = newLayer[i][j];
        }
    }
}

/** 下层旋转 (D move) **/
void rotateD() {
    // j==0 固定 (下层)；局部坐标 (i, k)
    // 颜色更新规则 (从下方看顺时针)：
    //   new_front = old_right
    //   new_right = old_back
    //   new_back  = old_left
    //   new_left  = old_front
    Cubie temp[3][3];
    for (int i = 0; i < 3; i++){
        for (int k = 0; k < 3; k++){
            Cubie c = cubeState[i][0][k];
            Cubie updated = c;
            updated.colors[0] = c.colors[3]; // front = old right
            updated.colors[3] = c.colors[1]; // right = old back
            updated.colors[1] = c.colors[2]; // back  = old left
            updated.colors[2] = c.colors[0]; // left  = old front
            temp[i][k] = updated;
        }
    }
    // 位置更新：new[i][k] = old[k][2 - i]
    Cubie newLayer[3][3];
    for (int i = 0; i < 3; i++){
        for (int k = 0; k < 3; k++){
            newLayer[i][k] = temp[k][2 - i];
        }
    }
    // 写回
    for (int i = 0; i < 3; i++){
        for (int k = 0; k < 3; k++){
            cubeState[i][0][k] = newLayer[i][k];
        }
    }
}

/** 前层旋转 (F move) **/
void rotateF() {
    // 固定前层： k == 2；局部坐标 (i, j)
    // 颜色更新：从正前方看，顺时针旋转：
    //   new.front = old.front (不变)
    //   new.back  = old.back  (不变)
    //   new.right = old.up    (4)
    //   new.down  = old.right (3)
    //   new.left  = old.down  (5)
    //   new.up    = old.left  (2)
    Cubie temp[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            Cubie c = cubeState[i][j][2];
            Cubie updated = c;
            updated.colors[3] = c.colors[4]; // right = old up
            updated.colors[5] = c.colors[3]; // down  = old right
            updated.colors[2] = c.colors[5]; // left  = old down
            updated.colors[4] = c.colors[2]; // up    = old left
            // front (0) 和 back (1) 保持不变
            temp[i][j] = updated;
        }
    }
    // 位置更新：新位置 new[i][j] = old[2 - j][i]
    Cubie newLayer[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            newLayer[i][j] = temp[2 - j][i];
        }
    }
    // 写回前层 (k==2)
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            cubeState[i][j][2] = newLayer[i][j];
        }
    }
}

/** 左层旋转 (L move) **/
void rotateL() {
    // 固定左层： i == 0；局部坐标 (j, k)
    // 颜色更新规则 (从左侧看顺时针)：
    //   new.front = old.up
    //   new.down  = old.front
    //   new.back  = old.down
    //   new.up    = old.back
    Cubie temp[3][3];
    for (int j = 0; j < 3; j++){
        for (int k = 0; k < 3; k++){
            Cubie c = cubeState[0][j][k];
            Cubie updated = c;
            updated.colors[0] = c.colors[4]; // front = old up
            updated.colors[5] = c.colors[0]; // down  = old front
            updated.colors[1] = c.colors[5]; // back  = old down
            updated.colors[4] = c.colors[1]; // up    = old back
            temp[j][k] = updated;
        }
    }
    // 更新二维位置：对于左层，采用新公式：
    // new[j][k] = old[k][2 - j]
    Cubie newLayer[3][3];
    for (int j = 0; j < 3; j++){
        for (int k = 0; k < 3; k++){
            newLayer[j][k] = temp[k][2 - j];
        }
    }
    // 写回左层 (i==0)
    for (int j = 0; j < 3; j++){
        for (int k = 0; k < 3; k++){
            cubeState[0][j][k] = newLayer[j][k];
        }
    }
}

/** 上层旋转 (U move) **/
void rotateU() {
    // j==2 固定 (上层)；局部坐标 (i, k)
    // 颜色更新规则 (从上面看顺时针)：
    //   new_right = old_front
    //   new_back  = old_right
    //   new_left  = old_back
    //   new_front = old_left
    Cubie temp[3][3];
    for (int i = 0; i < 3; i++){
        for (int k = 0; k < 3; k++){
            Cubie c = cubeState[i][2][k];
            Cubie updated = c;
            updated.colors[3] = c.colors[0]; // right = old front
            updated.colors[1] = c.colors[3]; // back  = old right
            updated.colors[2] = c.colors[1]; // left  = old back
            updated.colors[0] = c.colors[2]; // front = old left
            temp[i][k] = updated;
        }
    }
    // 位置更新：new[i][k] = old[2 - k][i]
    Cubie newLayer[3][3];
    for (int i = 0; i < 3; i++){
        for (int k = 0; k < 3; k++){
            newLayer[i][k] = temp[2 - k][i];
        }
    }
    // 写回
    for (int i = 0; i < 3; i++){
        for (int k = 0; k < 3; k++){
            cubeState[i][2][k] = newLayer[i][k];
        }
    }
}