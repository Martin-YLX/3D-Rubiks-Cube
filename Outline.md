# **3D 魔方项目提纲**

## **📌 项目名称**
**3D 魔方（Rubik's Cube 3D）**

## **📌 项目目标**
- **第一阶段**：实现 3D 魔方的**基本交互功能**，包括 **魔方建模、旋转单层、摄像机控制**。
- **第二阶段**：拓展 **AI 复原、解法提示、计时器、视觉优化**，增强可玩性和视觉效果。

---

## **📌 第一阶段：基础功能**
### **✅ 1. 3D 魔方建模**
- 使用 **OpenGL** 绘制 **3x3x3 魔方**。
- 将魔方拆分为 **27 个独立小立方体**，管理其位置与旋转。
- 设定每个小立方体的 **索引信息**，便于后续旋转计算。

### **✅ 2. 摄像机控制**
- **鼠标拖拽旋转视角**（第一人称/轨道视角）。
- **支持缩放**（滚轮或键盘调整视距）。

### **✅ 3. 交互控制**
- **键盘控制魔方旋转**（按键指定旋转方向）。
- **鼠标拖拽旋转单层**（检测鼠标拖动方向）。
- **平滑旋转动画**（使用插值，使旋转过渡自然）。

### **✅ 4. 视觉优化**
- **Phong 光照**（基本立体感）。
- **高亮选中层**（鼠标悬停提示可旋转区域）。

---

## **📌 第二阶段：拓展功能**
### **🎯 1. AI 复原魔方**
- **实现自动复原算法**：
    - **Kociemba 算法**（高效求解）。
    - 或 **IDA* 搜索**（更通用）。
- **按步执行 AI 复原**（用户可观察 AI 逐步还原）。

### **🎯 2. 求解提示**
- 允许用户请求 **下一步解法**（按键触发）。
- 在界面上 **可视化提示应该旋转的层和方向**。

### **🎯 3. 计时模式**
- 记录用户**解魔方所用时间**。
- 显示**最佳解法时间**，供用户挑战。

### **🎯 4. 视觉优化**
- **边缘高光**（增强立体感）。
- **反射特效**（添加环境贴图）。
- **更柔和的旋转动画**（四元数插值，避免顿挫感）。

---

## **📌 技术栈**
| **类别** | **技术** |
|----------|---------|
| **编程语言** | C++ |
| **图形库** | OpenGL + GLSL |
| **窗口管理** | GLFW |
| **数学库** | GLM（矩阵计算） |
| **交互检测** | Ray Picking（鼠标点击检测） |
| **AI 复原** | Kociemba 算法 / IDA* 搜索 |

---

## **📌 项目时间安排**
| **阶段** | **任务** | **预计时间** |
|----------|---------|------------|
| **第 1 周** | 3D 魔方建模、摄像机控制 | 3~5 天 |
| **第 2 周** | 旋转单层交互（鼠标拖拽 + 动画） | 5~7 天 |
| **第 3 周** | 视觉优化（光照 + 选中层高亮） | 3~5 天 |
| **第 4 周** | AI 复原算法 | 7~10 天 |
| **第 5 周** | 求解提示 + 计时模式 | 5~7 天 |
| **第 6 周** | 视觉优化（反射 + 平滑动画） | 5~7 天 |

---

## **📌 预期成果**
- **第一阶段**：可交互的 3D 魔方，支持**旋转单层、视角控制**。
- **第二阶段**：**AI 复原、求解提示、计时模式、视觉优化**，提升体验。

