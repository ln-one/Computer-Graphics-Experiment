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
        MessageBoxW(hwnd, L"Failed to create OpenGL context. Please ensure your graphics card supports OpenGL.", L"Initialization Failed", MB_OK | MB_ICONERROR);
        return false;
    }
    
    // Setup OpenGL basic state
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.4f, 0.8f, 1.0f);  // Blue background
    
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
    // Setup pixel format
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,  // Color depth
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,  // Depth buffer
        8,   // Stencil buffer
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
    
    // Create OpenGL rendering context
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
    // For basic OpenGL 1.1 functions, Windows provides them
    // Higher version functions require GLAD, using basic functions for now
    return true;
}

void GraphicsEngine3D::Render() {
    if (!isInitialized) {
        return;
    }
    
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Swap buffers
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
            // Record mouse position in view control mode
            break;
        default:
            break;
    }
}

void GraphicsEngine3D::OnLButtonUp(int x, int y) {
    isDragging = false;
}

void GraphicsEngine3D::OnLButtonDoubleClick(int x, int y) {
    // Double-click to open transform dialog (to be implemented)
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
    // Mouse wheel control (to be implemented)
}

void GraphicsEngine3D::HandleShapeCreation(int x, int y) {
    // Shape creation logic (to be implemented)
}

void GraphicsEngine3D::HandleSelection(int x, int y) {
    // Selection logic (to be implemented)
}

void GraphicsEngine3D::HandleViewControl(int deltaX, int deltaY) {
    // View control logic (to be implemented)
}

void GraphicsEngine3D::UpdateLight() {
    // Light update logic (to be implemented)
}
