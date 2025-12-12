#include "GraphicsEngine3D.h"
#include <gl/GL.h>

GraphicsEngine3D::GraphicsEngine3D() 
    : hwnd(NULL), hdc(NULL), hglrc(NULL), 
      currentMode(MODE_NONE), selectedShapeIndex(-1), hasSelection(false),
      lastMouseX(0), lastMouseY(0), isDragging(false),
      shaderProgram(0), isInitialized(false) {
}

GraphicsEngine3D::~GraphicsEngine3D() {
    Shutdown();
}

bool GraphicsEngine3D::Initialize(HWND hwnd) {
    if (isInitialized) {
        return true;
    }
    
    this->hwnd = hwnd;
    this->hdc = GetDC(hwnd);
    
    if (!CreateOpenGLContext()) {
        MessageBoxW(hwnd, L"无法创建OpenGL上下文\n请确保显卡支持OpenGL", 
                   L"初始化失败", MB_OK | MB_ICONERROR);
        return false;
    }
    
    // 设置OpenGL基本状态
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.4f, 0.8f, 1.0f);  // 蓝色背景
    
    isInitialized = true;
    return true;
}

void GraphicsEngine3D::Shutdown() {
    if (hglrc) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hglrc);
        hglrc = NULL;
    }
    
    if (hdc && hwnd) {
        ReleaseDC(hwnd, hdc);
        hdc = NULL;
    }
    
    isInitialized = false;
}

bool GraphicsEngine3D::CreateOpenGLContext() {
    // 设置像素格式
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,  // 颜色深度
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,  // 深度缓冲
        8,   // 模板缓冲
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    if (pixelFormat == 0) {
        return false;
    }
    
    if (!SetPixelFormat(hdc, pixelFormat, &pfd)) {
        return false;
    }
    
    // 创建OpenGL渲染上下文
    hglrc = wglCreateContext(hdc);
    if (!hglrc) {
        return false;
    }
    
    if (!wglMakeCurrent(hdc, hglrc)) {
        wglDeleteContext(hglrc);
        hglrc = NULL;
        return false;
    }
    
    return true;
}

bool GraphicsEngine3D::LoadOpenGLFunctions() {
    // 对于基础OpenGL 1.1功能，Windows已经提供
    // 更高版本的功能需要GLAD，这里先使用基础功能
    return true;
}

void GraphicsEngine3D::Render() {
    if (!isInitialized) {
        return;
    }
    
    // 清除颜色和深度缓冲
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 交换缓冲区
    SwapBuffers(hdc);
}

void GraphicsEngine3D::ClearScene() {
    shapes.clear();
    selectedShapeIndex = -1;
    hasSelection = false;
}

void GraphicsEngine3D::SetMode(DrawMode mode) {
    currentMode = mode;
}

void GraphicsEngine3D::OnLButtonDown(int x, int y) {
    lastMouseX = x;
    lastMouseY = y;
    isDragging = true;
    
    switch (currentMode) {
        case MODE_3D_SPHERE:
        case MODE_3D_CYLINDER:
        case MODE_3D_PLANE:
        case MODE_3D_CUBE:
            HandleShapeCreation(x, y);
            break;
        case MODE_3D_SELECT:
            HandleSelection(x, y);
            break;
        case MODE_3D_VIEW_CONTROL:
            // 视角控制模式下记录鼠标位置
            break;
        default:
            break;
    }
}

void GraphicsEngine3D::OnLButtonUp(int x, int y) {
    isDragging = false;
}

void GraphicsEngine3D::OnLButtonDoubleClick(int x, int y) {
    // 双击打开变换对话框（后续实现）
}

void GraphicsEngine3D::OnMouseMove(int x, int y) {
    if (!isDragging) {
        return;
    }
    
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;
    
    if (currentMode == MODE_3D_VIEW_CONTROL) {
        HandleViewControl(deltaX, deltaY);
    }
    
    lastMouseX = x;
    lastMouseY = y;
}

void GraphicsEngine3D::OnMouseWheel(int delta) {
    // 鼠标滚轮控制（后续实现）
}

void GraphicsEngine3D::HandleShapeCreation(int x, int y) {
    // 图形创建逻辑（后续实现）
}

void GraphicsEngine3D::HandleSelection(int x, int y) {
    // 选择逻辑（后续实现）
}

void GraphicsEngine3D::HandleViewControl(int deltaX, int deltaY) {
    // 视角控制逻辑（后续实现）
}

void GraphicsEngine3D::UpdateLight() {
    // 光照更新逻辑（后续实现）
}
