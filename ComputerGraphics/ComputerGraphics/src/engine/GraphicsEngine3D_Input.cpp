/**
 * @file GraphicsEngine3D_Input.cpp
 * @brief 3D图形引擎输入处理模块
 * 
 * 本文件包含3D图形引擎的鼠标交互处理功能：
 * - 鼠标按键事件处理（左键、右键、双击）
 * - 鼠标移动事件处理
 * - 鼠标滚轮事件处理
 * - 图形创建、选择、拖拽逻辑
 * - 视角控制逻辑
 * 
 * 交互模式说明：
 * 1. 图形创建模式：点击创建对应类型的3D图形
 * 2. 选择模式：点击选择图形，拖拽移动图形，滚轮调整Z轴位置
 * 3. 视角控制模式：拖拽旋转视角，滚轮缩放
 * 4. Ctrl键：在任何模式下按住Ctrl可临时切换到视角控制
 * 
 * @author 计算机图形学项目组
 */

#include "GraphicsEngine3D.h"
#include "../algorithms/MeshGenerator.h"
#include "../ui/Dialogs3D.h"
#include <cmath>
#include <cfloat>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// 鼠标按键事件处理
// ============================================================================

/**
 * @brief 处理鼠标左键按下事件
 * @param x 鼠标X坐标（屏幕坐标）
 * @param y 鼠标Y坐标（屏幕坐标）
 * 
 * 根据当前绘图模式执行不同操作：
 * - 图形创建模式：在点击位置创建新图形
 * - 选择模式：选择点击位置的图形
 * - 视角控制模式：开始视角旋转
 * - 按住Ctrl键：临时进入视角控制模式
 */
void GraphicsEngine3D::OnLButtonDown(int x, int y) {
    // 记录鼠标位置，用于后续的拖拽计算
    lastMouseX = x;
    lastMouseY = y;
    isDragging = true;
    
    // 检查是否按住了Ctrl键（用于视角控制）
    bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    
    if (ctrlPressed) {
        // Ctrl + 左键 = 旋转视角
        // 不创建形状，只记录鼠标位置用于旋转
        return;
    }
    
    // 根据当前模式执行相应操作
    switch (currentMode) {
        case MODE_3D_SPHERE:
        case MODE_3D_CYLINDER:
        case MODE_3D_PLANE:
        case MODE_3D_CUBE:
            // 图形创建模式：在点击位置创建新图形
            HandleShapeCreation(x, y);
            break;
        case MODE_3D_SELECT:
            // 选择模式：尝试选择点击位置的图形
            HandleSelection(x, y);
            break;
        case MODE_3D_VIEW_CONTROL:
            // 视角控制模式：只旋转，不创建形状
            break;
        default:
            break;
    }
}

/**
 * @brief 处理鼠标左键释放事件
 * @param x 鼠标X坐标
 * @param y 鼠标Y坐标
 * 
 * 结束拖拽状态
 */
void GraphicsEngine3D::OnLButtonUp(int x, int y) {
    isDragging = false;
}

/**
 * @brief 处理鼠标右键按下事件
 * @param x 鼠标X坐标
 * @param y 鼠标Y坐标
 * 
 * 开始右键拖拽（可用于平移视角等功能）
 */
void GraphicsEngine3D::OnRButtonDown(int x, int y) {
    lastMouseX = x;
    lastMouseY = y;
    isRightDragging = true;
}

/**
 * @brief 处理鼠标右键释放事件
 * @param x 鼠标X坐标
 * @param y 鼠标Y坐标
 * 
 * 结束右键拖拽状态
 */
void GraphicsEngine3D::OnRButtonUp(int x, int y) {
    isRightDragging = false;
}

/**
 * @brief 处理鼠标左键双击事件
 * @param x 鼠标X坐标
 * @param y 鼠标Y坐标
 * 
 * 双击打开变换对话框，允许用户精确设置图形的位置、旋转和缩放参数。
 * 
 * 处理流程：
 * 1. 尝试选择双击位置的图形
 * 2. 如果选中了图形，打开变换对话框
 * 3. 用户可以在对话框中修改变换参数
 */
void GraphicsEngine3D::OnLButtonDoubleClick(int x, int y) {
    // 首先尝试选择点击位置的图形
    HandleSelection(x, y);
    
    // 如果有选中的图形，打开变换对话框
    if (hasSelection && selectedShapeIndex >= 0 && 
        selectedShapeIndex < (int)shapes.size()) {
        
        Shape3D& selectedShape = shapes[selectedShapeIndex];
        
        // 调试输出
        char debugMsg[256];
        sprintf_s(debugMsg, "打开变换对话框，图形索引: %d", selectedShapeIndex);
        OutputDebugStringA(debugMsg);
        
        // 显示变换对话框
        if (TransformDialog3D::Show(hwnd, &selectedShape)) {
            // 用户点击了确定，参数已经被应用
            OutputDebugStringA("变换对话框: 用户确认，参数已应用");
        } else {
            // 用户点击了取消
            OutputDebugStringA("变换对话框: 用户取消");
        }
    } else {
        OutputDebugStringA("双击: 该位置没有图形");
    }
}

// ============================================================================
// 鼠标移动和滚轮事件处理
// ============================================================================

/**
 * @brief 处理鼠标移动事件
 * @param x 鼠标X坐标
 * @param y 鼠标Y坐标
 * 
 * 根据当前模式和拖拽状态执行不同操作：
 * - 视角控制模式或按住Ctrl：旋转摄像机视角
 * - 选择模式且有选中图形：拖拽移动图形
 */
void GraphicsEngine3D::OnMouseMove(int x, int y) {
    // 如果没有在拖拽，不处理
    if (!isDragging) {
        return;
    }
    
    // 计算鼠标移动量
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;
    
    // 检查是否按住了Ctrl键（用于视角控制）
    bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    
    if (currentMode == MODE_3D_VIEW_CONTROL || ctrlPressed) {
        // 视角控制：旋转摄像机
        HandleViewControl(deltaX, deltaY);
    } else if (currentMode == MODE_3D_SELECT && hasSelection && selectedShapeIndex >= 0) {
        // 选择模式且有选中图形：拖拽移动图形
        HandleObjectDragging(deltaX, deltaY);
    }
    
    // 更新上次鼠标位置
    lastMouseX = x;
    lastMouseY = y;
}

/**
 * @brief 处理鼠标滚轮事件
 * @param delta 滚轮滚动量（正值向上，负值向下）
 * 
 * 根据当前模式执行不同操作：
 * - 视角控制模式或按住Ctrl：缩放视角（调整摄像机距离）
 * - 选择模式且有选中图形：移动图形的Z轴位置
 * - 其他情况：默认缩放视角
 */
void GraphicsEngine3D::OnMouseWheel(int delta) {
    // 检查是否按住了Ctrl键
    bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    
    // 视角控制模式或按住Ctrl：调整摄像机距离（缩放视角）
    if (currentMode == MODE_3D_VIEW_CONTROL || ctrlPressed) {
        float zoomSpeed = 0.5f;
        camera.distance -= (delta / 120.0f) * zoomSpeed;  // 120是标准滚轮增量
        
        // 限制摄像机距离在合理范围内
        if (camera.distance < 1.0f) camera.distance = 1.0f;
        if (camera.distance > 50.0f) camera.distance = 50.0f;
        
        // 调试输出
        char debugMsg[256];
        sprintf_s(debugMsg, "视角缩放: delta=%d, 摄像机距离=%.2f", 
                  delta, camera.distance);
        OutputDebugStringA(debugMsg);
    }
    // 选择模式且有选中图形：移动图形的Z轴位置
    else if (currentMode == MODE_3D_SELECT && hasSelection && selectedShapeIndex >= 0 && 
        selectedShapeIndex < (int)shapes.size()) {
        
        // 计算Z轴移动量
        float zMoveSpeed = 0.01f;
        float zDelta = (delta / 120.0f) * zMoveSpeed;
        
        // 应用Z轴移动到选中的图形
        Shape3D& selectedShape = shapes[selectedShapeIndex];
        selectedShape.positionZ += zDelta;
        
        // 调试输出
        char debugMsg[256];
        sprintf_s(debugMsg, "移动图形 %d 的Z轴位置: delta=%d, 新Z=%.2f", 
                  selectedShapeIndex, delta, selectedShape.positionZ);
        OutputDebugStringA(debugMsg);
    } else {
        // 默认：调整摄像机距离（缩放视角）
        float zoomSpeed = 0.5f;
        camera.distance -= (delta / 120.0f) * zoomSpeed;
        
        // 限制摄像机距离
        if (camera.distance < 1.0f) camera.distance = 1.0f;
        if (camera.distance > 50.0f) camera.distance = 50.0f;
        
        // 调试输出
        char debugMsg[256];
        sprintf_s(debugMsg, "默认缩放: delta=%d, 摄像机距离=%.2f", 
                  delta, camera.distance);
        OutputDebugStringA(debugMsg);
    }
}


// ============================================================================
// 图形创建处理
// ============================================================================

/**
 * @brief 处理3D图形创建
 * @param x 鼠标X坐标（屏幕坐标）
 * @param y 鼠标Y坐标（屏幕坐标）
 * 
 * 在鼠标点击位置创建新的3D图形。
 * 
 * 坐标转换说明：
 * 屏幕坐标 → 世界坐标的简化映射：
 * - 屏幕X映射到世界X：范围[-2, 2]
 * - 屏幕Y映射到世界Y：范围[-2, 2]（Y轴反转）
 * - 世界Z固定为0
 * 
 * 不同图形类型的默认颜色：
 * - 立方体：红色
 * - 球体：蓝色
 * - 圆柱体：绿色
 * - 平面：灰色
 */
void GraphicsEngine3D::HandleShapeCreation(int x, int y) {
    // 调试输出
    char debugMsg[256];
    sprintf_s(debugMsg, "创建图形: 位置(%d, %d), 当前模式: %d", x, y, (int)currentMode);
    OutputDebugStringA(debugMsg);
    
    Shape3D newShape;
    
    // 获取窗口尺寸
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    if (width > 0 && height > 0) {
        // 将屏幕坐标映射到世界坐标
        // 屏幕坐标原点在左上角，世界坐标原点在中心
        float worldX = ((float)x / width - 0.5f) * 4.0f;   // 范围: -2 到 2
        float worldY = -((float)y / height - 0.5f) * 4.0f; // 范围: -2 到 2（Y轴反转）
        float worldZ = 0.0f;  // 放置在Z=0平面
        
        newShape.positionX = worldX;
        newShape.positionY = worldY;
        newShape.positionZ = worldZ;
    }
    
    // 设置默认旋转和缩放
    newShape.rotationX = 0.0f;
    newShape.rotationY = 0.0f;
    newShape.rotationZ = 0.0f;
    newShape.scaleX = 1.0f;
    newShape.scaleY = 1.0f;
    newShape.scaleZ = 1.0f;
    
    // 根据当前模式生成网格并设置材质颜色
    // 不同类型的图形使用不同的默认颜色，便于区分
    switch (currentMode) {
        case MODE_3D_CUBE:
            MeshGenerator::GenerateCube(newShape, 1.0f);
            // 红色立方体
            newShape.ambient[0] = 0.2f; newShape.ambient[1] = 0.05f; newShape.ambient[2] = 0.05f;
            newShape.diffuse[0] = 0.8f; newShape.diffuse[1] = 0.2f; newShape.diffuse[2] = 0.2f;
            newShape.specular[0] = 1.0f; newShape.specular[1] = 0.5f; newShape.specular[2] = 0.5f;
            break;
        case MODE_3D_SPHERE:
            MeshGenerator::GenerateSphere(newShape, 0.5f, 16, 16);
            // 蓝色球体
            newShape.ambient[0] = 0.05f; newShape.ambient[1] = 0.05f; newShape.ambient[2] = 0.2f;
            newShape.diffuse[0] = 0.2f; newShape.diffuse[1] = 0.4f; newShape.diffuse[2] = 0.9f;
            newShape.specular[0] = 0.5f; newShape.specular[1] = 0.5f; newShape.specular[2] = 1.0f;
            break;
        case MODE_3D_CYLINDER:
            MeshGenerator::GenerateCylinder(newShape, 0.5f, 1.0f, 16);
            // 绿色圆柱体
            newShape.ambient[0] = 0.05f; newShape.ambient[1] = 0.2f; newShape.ambient[2] = 0.05f;
            newShape.diffuse[0] = 0.2f; newShape.diffuse[1] = 0.8f; newShape.diffuse[2] = 0.2f;
            newShape.specular[0] = 0.5f; newShape.specular[1] = 1.0f; newShape.specular[2] = 0.5f;
            break;
        case MODE_3D_PLANE:
            MeshGenerator::GeneratePlane(newShape, 1.0f, 1.0f);
            // 灰色平面
            newShape.ambient[0] = 0.15f; newShape.ambient[1] = 0.15f; newShape.ambient[2] = 0.15f;
            newShape.diffuse[0] = 0.6f; newShape.diffuse[1] = 0.6f; newShape.diffuse[2] = 0.6f;
            newShape.specular[0] = 0.3f; newShape.specular[1] = 0.3f; newShape.specular[2] = 0.3f;
            break;
        default:
            return;  // 未知图形类型
    }
    
    // 设置高光指数（控制高光大小）
    newShape.shininess = 32.0f;
    
    // 添加到图形集合
    shapes.push_back(newShape);
    
    // 调试输出
    char debugMsg2[256];
    sprintf_s(debugMsg2, "图形已添加! 总数: %zu, VAO: %u", shapes.size(), newShape.VAO);
    OutputDebugStringA(debugMsg2);
}

// ============================================================================
// 图形选择处理
// ============================================================================

/**
 * @brief 处理3D图形选择
 * @param x 鼠标X坐标（屏幕坐标）
 * @param y 鼠标Y坐标（屏幕坐标）
 * 
 * 选择算法说明：
 * 使用与渲染代码完全相同的投影计算，确保选择准确
 */
void GraphicsEngine3D::HandleSelection(int x, int y) {
    // 获取窗口尺寸
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    if (width <= 0 || height <= 0) return;
    
    float aspectRatio = (float)width / (float)height;
    
    // 透视投影参数（与渲染代码一致）
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float top = nearPlane * tanf(fov * (float)M_PI / 360.0f);
    float right_proj = top * aspectRatio;
    
    // 计算摄像机位置（与渲染代码完全一致）
    float cameraX = camera.targetX + camera.distance * cosf(camera.angleY * (float)M_PI / 180.0f) * cosf(camera.angleX * (float)M_PI / 180.0f);
    float cameraY = camera.targetY + camera.distance * sinf(camera.angleY * (float)M_PI / 180.0f);
    float cameraZ = camera.targetZ + camera.distance * cosf(camera.angleY * (float)M_PI / 180.0f) * sinf(camera.angleX * (float)M_PI / 180.0f);
    
    // 计算视图矩阵的基向量（与渲染代码完全一致）
    float fx = camera.targetX - cameraX;
    float fy = camera.targetY - cameraY;
    float fz = camera.targetZ - cameraZ;
    float flen = sqrtf(fx*fx + fy*fy + fz*fz);
    if (flen > 0.0001f) { fx /= flen; fy /= flen; fz /= flen; }
    
    float upX = 0.0f, upY = 1.0f, upZ = 0.0f;
    float rx = fy * upZ - fz * upY;
    float ry = fz * upX - fx * upZ;
    float rz = fx * upY - fy * upX;
    float rlen = sqrtf(rx*rx + ry*ry + rz*rz);
    if (rlen > 0.0001f) { rx /= rlen; ry /= rlen; rz /= rlen; }
    
    float ux = ry * fz - rz * fy;
    float uy = rz * fx - rx * fz;
    float uz = rx * fy - ry * fx;
    
    // 调试输出摄像机信息
    char camDebug[256];
    sprintf_s(camDebug, "摄像机: 位置(%.2f, %.2f, %.2f), 角度(%.1f, %.1f), 距离=%.1f", 
              cameraX, cameraY, cameraZ, camera.angleX, camera.angleY, camera.distance);
    OutputDebugStringA(camDebug);
    
    // 简单选择：找到距离点击位置最近的图形
    int closestShapeIndex = -1;
    float minDistance = FLT_MAX;
    
    for (size_t i = 0; i < shapes.size(); i++) {
        const Shape3D& shape = shapes[i];
        
        // 将图形位置从世界坐标转换到摄像机坐标
        float dx = shape.positionX - cameraX;
        float dy = shape.positionY - cameraY;
        float dz = shape.positionZ - cameraZ;
        
        // 在摄像机坐标系中的位置（与渲染代码的视图矩阵一致）
        float eyeX = rx * dx + ry * dy + rz * dz;
        float eyeY = ux * dx + uy * dy + uz * dz;
        float eyeZ = -fx * dx - fy * dy - fz * dz;  // 注意：这里是负的前向向量
        
        // 如果在摄像机后面，跳过
        if (eyeZ <= nearPlane) {
            char skipMsg[128];
            sprintf_s(skipMsg, "图形 %zu: 在摄像机后面 (eyeZ=%.2f)", i, eyeZ);
            OutputDebugStringA(skipMsg);
            continue;
        }
        
        // 透视投影（与glFrustum一致）
        // NDC_x = eyeX * nearPlane / (right_proj * eyeZ)
        // NDC_y = eyeY * nearPlane / (top * eyeZ)
        float ndcX = (eyeX * nearPlane) / (right_proj * eyeZ);
        float ndcY = (eyeY * nearPlane) / (top * eyeZ);
        
        // NDC转屏幕坐标
        float screenX = (ndcX + 1.0f) * 0.5f * width;
        float screenY = (1.0f - ndcY) * 0.5f * height;  // Y轴反转
        
        // 计算点击位置与图形中心的距离
        float distX = (float)x - screenX;
        float distY = (float)y - screenY;
        float distance = sqrtf(distX * distX + distY * distY);
        
        // 选择容差（像素）
        float selectionRadius = 100.0f;
        
        // 调试输出
        char debugMsg[256];
        sprintf_s(debugMsg, "图形 %zu: 世界(%.2f,%.2f,%.2f) 眼睛(%.2f,%.2f,%.2f) NDC(%.2f,%.2f) 屏幕(%.1f,%.1f) 点击(%d,%d) 距离=%.1f", 
                  i, shape.positionX, shape.positionY, shape.positionZ,
                  eyeX, eyeY, eyeZ, ndcX, ndcY, screenX, screenY, x, y, distance);
        OutputDebugStringA(debugMsg);
        
        // 如果在选择半径内且是最近的图形
        if (distance < selectionRadius && distance < minDistance) {
            minDistance = distance;
            closestShapeIndex = (int)i;
        }
    }
    
    // 清除之前的选择
    for (size_t i = 0; i < shapes.size(); i++) {
        shapes[i].selected = false;
    }
    
    // 选择最近的图形（如果找到）
    if (closestShapeIndex >= 0) {
        shapes[closestShapeIndex].selected = true;
        selectedShapeIndex = closestShapeIndex;
        hasSelection = true;
        
        // 调试输出
        char debugMsg[256];
        sprintf_s(debugMsg, ">>> 选中图形 %d，位置 (%.2f, %.2f, %.2f)", 
                  closestShapeIndex, 
                  shapes[closestShapeIndex].positionX,
                  shapes[closestShapeIndex].positionY,
                  shapes[closestShapeIndex].positionZ);
        OutputDebugStringA(debugMsg);
    } else {
        selectedShapeIndex = -1;
        hasSelection = false;
        OutputDebugStringA(">>> 未选中任何图形");
    }
}

// ============================================================================
// 视角控制处理
// ============================================================================

/**
 * @brief 处理视角控制（摄像机旋转）
 * @param deltaX 鼠标X方向移动量
 * @param deltaY 鼠标Y方向移动量
 * 
 * 使用球坐标系统控制摄像机：
 * - 水平移动(deltaX)改变angleX，使摄像机绕Y轴旋转
 * - 垂直移动(deltaY)改变angleY，使摄像机上下俯仰
 * 
 * 垂直角度限制在[-89°, 89°]，防止摄像机翻转
 */
void GraphicsEngine3D::HandleViewControl(int deltaX, int deltaY) {
    // 根据鼠标移动更新摄像机角度
    camera.angleX += deltaX * 0.5f;  // 水平旋转
    camera.angleY += deltaY * 0.5f;  // 垂直旋转
    
    // 限制垂直角度，防止摄像机翻转
    if (camera.angleY > 89.0f) camera.angleY = 89.0f;
    if (camera.angleY < -89.0f) camera.angleY = -89.0f;
}

// ============================================================================
// 物体拖拽处理
// ============================================================================

/**
 * @brief 处理物体拖拽移动
 * @param deltaX 鼠标X方向移动量
 * @param deltaY 鼠标Y方向移动量
 * 
 * 将鼠标移动转换为世界空间中的物体移动：
 * - 鼠标X移动 → 物体X移动
 * - 鼠标Y移动 → 物体Y移动（Y轴反转）
 * - Z轴位置通过滚轮控制
 */
void GraphicsEngine3D::HandleObjectDragging(int deltaX, int deltaY) {
    // 检查是否有有效的选中图形
    if (!hasSelection || selectedShapeIndex < 0 || selectedShapeIndex >= (int)shapes.size()) {
        return;
    }
    
    Shape3D& selectedShape = shapes[selectedShapeIndex];
    
    // 获取窗口尺寸用于坐标转换
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    if (width <= 0 || height <= 0) return;
    
    // 将鼠标移动转换为世界空间移动
    // 移动灵敏度系数
    float movementScale = 0.01f;
    
    // 计算世界坐标移动量
    // X移动：正deltaX向右移动
    float worldDeltaX = (float)deltaX * movementScale;
    // Y移动：正deltaY向下移动，但3D世界Y轴向上，所以取反
    float worldDeltaY = -(float)deltaY * movementScale;
    
    // 应用移动到选中图形的位置
    selectedShape.positionX += worldDeltaX;
    selectedShape.positionY += worldDeltaY;
    // Z轴位置在XY平面拖拽时保持不变
    
    // 调试输出
    char debugMsg[256];
    sprintf_s(debugMsg, "拖拽图形 %d: deltaX=%d, deltaY=%d, 新位置=(%.2f, %.2f, %.2f)", 
              selectedShapeIndex, deltaX, deltaY, 
              selectedShape.positionX, selectedShape.positionY, selectedShape.positionZ);
    OutputDebugStringA(debugMsg);
}
