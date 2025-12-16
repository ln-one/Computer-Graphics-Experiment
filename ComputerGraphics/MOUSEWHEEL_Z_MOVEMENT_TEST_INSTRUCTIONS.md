# 鼠标滚轮Z轴移动测试说明
# Mouse Wheel Z-Axis Movement Test Instructions

## 功能说明 / Feature Description

实现了鼠标滚轮控制选中3D图形沿Z轴移动的功能（Requirement 2.3）。

When a 3D shape is selected, scrolling the mouse wheel will move the shape along the Z-axis (forward/backward in 3D space).

## 测试步骤 / Test Steps

### 1. 启动程序并切换到3D模式
   - 启动程序
   - 点击菜单 "模式(M)" -> "3D模式(3)"
   - 应该看到蓝色背景的3D视图

### 2. 创建一个3D图形
   - 点击菜单 "3D图形(3)" -> "立方体(U)" 或其他图形
   - 在窗口中点击鼠标左键创建图形
   - 应该看到一个3D图形出现

### 3. 选择图形
   - 点击菜单 "3D控制(O)" -> "选择(S)"
   - 点击刚才创建的图形
   - 图形应该变成黄色（表示被选中）

### 4. 测试Z轴移动
   - 确保图形处于选中状态（黄色）
   - 向上滚动鼠标滚轮 -> 图形应该向前移动（变大/靠近）
   - 向下滚动鼠标滚轮 -> 图形应该向后移动（变小/远离）

### 5. 测试视角缩放（无选中图形时）
   - 点击空白区域取消选择（图形恢复原色）
   - 滚动鼠标滚轮
   - 整个场景应该缩放（摄像机距离改变）

## 预期行为 / Expected Behavior

| 状态 | 滚轮操作 | 预期结果 |
|------|----------|----------|
| 有选中图形 | 向上滚动 | 选中图形沿Z轴正方向移动（向前） |
| 有选中图形 | 向下滚动 | 选中图形沿Z轴负方向移动（向后） |
| 无选中图形 | 向上滚动 | 摄像机拉近（场景变大） |
| 无选中图形 | 向下滚动 | 摄像机拉远（场景变小） |

## 实现细节 / Implementation Details

- 文件: `ComputerGraphics/src/engine/GraphicsEngine3D.cpp`
- 方法: `OnMouseWheel(int delta)`
- Z轴移动速度: 0.01f（每120单位滚轮增量）
- 摄像机缩放速度: 0.5f（每120单位滚轮增量）
- 摄像机距离范围: 1.0f - 50.0f

## 调试信息 / Debug Information

在Visual Studio的输出窗口中可以看到调试信息：
- 当移动选中图形时: "Moving shape X along Z-axis: delta=Y, newZ=Z"
