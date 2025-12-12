#pragma once
#include "../core/Shape3D.h"
#include "../core/DrawMode.h"
#include <windows.h>
#include <vector>

// 摄像机参数
struct Camera {
    float distance;    // 距离目标的距离
    float angleX;      // 水平旋转角
    float angleY;      // 垂直旋转角
    float targetX;     // 观察目标点X
    float targetY;     // 观察目标点Y
    float targetZ;     // 观察目标点Z
    
    Camera() : distance(5.0f), angleX(0.0f), angleY(0.0f),
               targetX(0.0f), targetY(0.0f), targetZ(0.0f) {}
};

// 光源参数
struct Light {
    float positionX;
    float positionY;
    float positionZ;
    float color[3];
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
    
    Light() : positionX(5.0f), positionY(5.0f), positionZ(5.0f),
              ambientIntensity(0.2f), diffuseIntensity(0.8f), specularIntensity(1.0f) {
        color[0] = color[1] = color[2] = 1.0f;
    }
};

// 3D图形引擎（结构参照GraphicsEngine）
class GraphicsEngine3D {
public:
    GraphicsEngine3D();
    ~GraphicsEngine3D();
    
    // 初始化（参照GraphicsEngine::Initialize）
    bool Initialize(HWND hwnd);
    void Shutdown();
    
    // 模式管理（参照GraphicsEngine::SetMode）
    void SetMode(DrawMode mode);
    DrawMode GetMode() const { return currentMode; }
    
    // 鼠标事件（参照GraphicsEngine的鼠标处理）
    void OnLButtonDown(int x, int y);
    void OnLButtonUp(int x, int y);
    void OnLButtonDoubleClick(int x, int y);
    void OnMouseMove(int x, int y);
    void OnMouseWheel(int delta);
    
    // 渲染（参照GraphicsEngine::RenderAll）
    void Render();
    void ClearScene();
    
    // 光照设置
    Light& GetLight() { return light; }
    void UpdateLight();
    
private:
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;  // OpenGL上下文
    
    DrawMode currentMode;
    std::vector<Shape3D> shapes;
    int selectedShapeIndex;
    bool hasSelection;
    
    Camera camera;
    Light light;
    
    // 鼠标状态
    int lastMouseX, lastMouseY;
    bool isDragging;
    
    // 着色器程序ID
    unsigned int shaderProgram;
    
    // OpenGL初始化标志
    bool isInitialized;
    
    // 私有辅助方法（参照GraphicsEngine的Handle方法）
    void HandleShapeCreation(int x, int y);
    void HandleSelection(int x, int y);
    void HandleViewControl(int x, int y);
    
    // OpenGL上下文创建
    bool CreateOpenGLContext();
    bool LoadOpenGLFunctions();
};
