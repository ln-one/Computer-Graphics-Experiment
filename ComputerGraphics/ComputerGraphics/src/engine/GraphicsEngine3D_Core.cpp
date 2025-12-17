/**
 * @file GraphicsEngine3D_Core.cpp
 * @brief 3D图形引擎核心模块 - 初始化与OpenGL上下文管理
 * 
 * 本文件包含3D图形引擎的核心初始化功能：
 * - 构造函数和析构函数
 * - OpenGL上下文创建和管理
 * - OpenGL扩展函数加载
 * - 光照参数更新
 * 
 * OpenGL初始化流程说明：
 * 1. 获取窗口设备上下文(HDC)
 * 2. 设置像素格式(PixelFormat)，指定颜色深度、深度缓冲等
 * 3. 创建OpenGL渲染上下文(HGLRC)
 * 4. 激活渲染上下文
 * 5. 加载OpenGL扩展函数（现代OpenGL功能）
 * 6. 初始化着色器程序
 * 
 * @author ln1.opensource@gmail.com
 */

#include "GraphicsEngine3D.h"
#include "OpenGLFunctions.h"
#include "../algorithms/ShaderManager.h"
#include <gl/GL.h>
#include <cmath>

// ============================================================================
// OpenGL函数指针类型定义
// 这些类型用于加载OpenGL扩展函数，因为Windows默认只支持OpenGL 1.1
// ============================================================================

// 额外的OpenGL函数指针类型（未在OpenGLFunctions.h中定义的）
typedef void (APIENTRY *PFNGLDRAWELEMENTSPROC_EXT)(GLenum mode, GLsizei count, GLenum type, const void *indices);
typedef void (APIENTRY *PFNGLUSEPROGRAMPROC_EXT)(GLuint program);
typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC_EXT)(GLuint program, const GLchar *name);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC_EXT)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY *PFNGLUNIFORM3FPROC_EXT)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRY *PFNGLUNIFORM1FPROC_EXT)(GLint location, GLfloat v0);
typedef void (APIENTRY *PFNGLUNIFORM1IPROC_EXT)(GLint location, GLint v0);
typedef void (APIENTRY *PFNGLACTIVETEXTUREPROC_EXT)(GLenum texture);
typedef void (APIENTRY *PFNGLBINDTEXTUREPROC_EXT)(GLenum target, GLuint texture);

// ============================================================================
// OpenGL函数指针全局变量定义
// 这些指针在LoadOpenGLFunctions()中被初始化
// ============================================================================

// 顶点数组对象(VAO)相关函数 - 用于管理顶点属性配置
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;

// 缓冲区对象(VBO/EBO)相关函数 - 用于存储顶点数据和索引
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;

// 顶点属性相关函数 - 用于指定顶点数据格式
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;

// 额外的函数指针（未在OpenGLFunctions.h中声明）
PFNGLDRAWELEMENTSPROC_EXT glDrawElementsExt = nullptr;
PFNGLUSEPROGRAMPROC_EXT glUseProgramExt = nullptr;
PFNGLGETUNIFORMLOCATIONPROC_EXT glGetUniformLocationExt = nullptr;
PFNGLUNIFORMMATRIX4FVPROC_EXT glUniformMatrix4fvExt = nullptr;
PFNGLUNIFORM3FPROC_EXT glUniform3fExt = nullptr;
PFNGLUNIFORM1FPROC_EXT glUniform1fExt = nullptr;
PFNGLUNIFORM1IPROC_EXT glUniform1iExt = nullptr;
PFNGLACTIVETEXTUREPROC_EXT glActiveTextureExt = nullptr;
PFNGLBINDTEXTUREPROC_EXT glBindTextureExt = nullptr;

// ============================================================================
// 构造函数和析构函数
// ============================================================================

/**
 * @brief 构造函数 - 初始化3D图形引擎的所有成员变量
 * 
 * 初始化内容：
 * - Windows句柄（hwnd, hdc, hglrc）设为NULL
 * - 绘图模式设为MODE_NONE
 * - 选择状态重置
 * - 摄像机参数设置默认值（距离5.0，角度0，目标点原点）
 * - 光源参数设置默认值（位置(5,5,5)，白色光，标准强度）
 */
GraphicsEngine3D::GraphicsEngine3D() 
    : hwnd(NULL), hdc(NULL), hglrc(NULL), 
      currentMode(MODE_NONE), selectedShapeIndex(-1), hasSelection(false),
      lastMouseX(0), lastMouseY(0), isDragging(false), isRightDragging(false),
      shaderProgram(0), isInitialized(false) {
    
    // 初始化摄像机参数
    // 使用球坐标系统：distance为半径，angleX/Y为角度
    camera.distance = 5.0f;      // 摄像机距离目标点5个单位
    camera.angleX = 0.0f;        // 水平角度（绕Y轴旋转）
    camera.angleY = 0.0f;        // 垂直角度（绕X轴旋转）
    camera.targetX = 0.0f;       // 目标点在原点
    camera.targetY = 0.0f;
    camera.targetZ = 0.0f;
    
    // 初始化光源参数
    // 光源位置在(5,5,5)，从右上前方照射场景
    light.positionX = 5.0f;
    light.positionY = 5.0f;
    light.positionZ = 5.0f;
    
    // 白色光源
    light.color[0] = light.color[1] = light.color[2] = 1.0f;
    
    // Phong光照模型的三个分量强度
    light.ambientIntensity = 0.2f;   // 环境光：模拟间接光照
    light.diffuseIntensity = 0.8f;   // 漫反射：主要光照贡献
    light.specularIntensity = 1.0f;  // 镜面反射：高光效果
}

/**
 * @brief 析构函数 - 清理所有资源
 * 
 * 调用Shutdown()释放OpenGL上下文和相关资源
 */
GraphicsEngine3D::~GraphicsEngine3D() {
    Shutdown();
}

// ============================================================================
// 初始化和关闭
// ============================================================================

/**
 * @brief 初始化3D图形引擎
 * @param hwnd 窗口句柄，用于创建OpenGL上下文
 * @return 初始化成功返回true，失败返回false
 * 
 * 初始化流程：
 * 1. 检查是否已初始化，避免重复初始化
 * 2. 保存窗口句柄，获取设备上下文(HDC)
 * 3. 创建OpenGL渲染上下文
 * 4. 加载OpenGL扩展函数
 * 5. 设置OpenGL基本状态（深度测试、背景色）
 * 6. 创建着色器程序
 */
bool GraphicsEngine3D::Initialize(HWND hwnd) {
    // 防止重复初始化
    if (isInitialized) {
        return true;
    }
    
    // 保存窗口句柄并获取设备上下文
    this->hwnd = hwnd;
    this->hdc = GetDC(hwnd);
    
    // 步骤1：创建OpenGL上下文
    if (!CreateOpenGLContext()) {
        MessageBoxW(hwnd, L"Failed to create OpenGL context. Please ensure your graphics card supports OpenGL.", 
                    L"Initialization Failed", MB_OK | MB_ICONERROR);
        return false;
    }
    
    // 步骤2：加载OpenGL扩展函数
    if (!LoadOpenGLFunctions()) {
        MessageBoxW(hwnd, L"Failed to load OpenGL functions.", 
                    L"Initialization Failed", MB_OK | MB_ICONERROR);
        return false;
    }
    
    // 步骤3：设置OpenGL基本状态
    glEnable(GL_DEPTH_TEST);              // 启用深度测试，确保正确的遮挡关系
    glClearColor(0.2f, 0.4f, 0.8f, 1.0f); // 设置背景色为蓝色
    
    // 步骤4：初始化着色器程序
    const char* vertexShader = ShaderManager::GetDefaultVertexShader();
    const char* fragmentShader = ShaderManager::GetDefaultFragmentShader();
    shaderProgram = ShaderManager::CreateShaderProgram(vertexShader, fragmentShader);
    
    if (shaderProgram == 0) {
        MessageBoxW(hwnd, L"Failed to create shader program.", 
                    L"Initialization Failed", MB_OK | MB_ICONERROR);
        return false;
    }
    
    isInitialized = true;
    return true;
}

/**
 * @brief 关闭3D图形引擎，释放所有资源
 * 
 * 清理流程：
 * 1. 取消当前OpenGL上下文
 * 2. 删除OpenGL渲染上下文
 * 3. 释放设备上下文
 * 4. 重置初始化标志
 */
void GraphicsEngine3D::Shutdown() {
    if (hglrc) {
        // 取消当前上下文绑定
        wglMakeCurrent(NULL, NULL);
        // 删除OpenGL渲染上下文
        wglDeleteContext(hglrc);
        hglrc = NULL;
    }
    
    if (hdc && hwnd) {
        // 释放设备上下文
        ReleaseDC(hwnd, hdc);
        hdc = NULL;
    }
    
    isInitialized = false;
}

// ============================================================================
// OpenGL上下文创建
// ============================================================================

/**
 * @brief 创建OpenGL渲染上下文
 * @return 创建成功返回true，失败返回false
 * 
 * 创建流程：
 * 1. 设置像素格式描述符(PIXELFORMATDESCRIPTOR)
 *    - 指定双缓冲、RGBA颜色模式
 *    - 32位颜色深度、24位深度缓冲、8位模板缓冲
 * 2. 选择并设置像素格式
 * 3. 创建OpenGL渲染上下文
 * 4. 激活渲染上下文
 */
bool GraphicsEngine3D::CreateOpenGLContext() {
    // 像素格式描述符 - 定义OpenGL渲染的像素属性
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),  // 结构体大小
        1,                               // 版本号
        PFD_DRAW_TO_WINDOW |            // 支持窗口绘制
        PFD_SUPPORT_OPENGL |            // 支持OpenGL
        PFD_DOUBLEBUFFER,               // 双缓冲（避免闪烁）
        PFD_TYPE_RGBA,                  // RGBA颜色模式
        32,                             // 32位颜色深度
        0, 0, 0, 0, 0, 0,              // 颜色位忽略
        0,                              // 无Alpha缓冲
        0,                              // 忽略Shift位
        0,                              // 无累积缓冲
        0, 0, 0, 0,                    // 累积位忽略
        24,                             // 24位深度缓冲（Z-buffer）
        8,                              // 8位模板缓冲
        0,                              // 无辅助缓冲
        PFD_MAIN_PLANE,                // 主绘图层
        0,                              // 保留
        0, 0, 0                        // 层掩码忽略
    };
    
    // 选择最匹配的像素格式
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    if (pixelFormat == 0) {
        return false;
    }
    
    // 设置像素格式
    if (!SetPixelFormat(hdc, pixelFormat, &pfd)) {
        return false;
    }
    
    // 创建OpenGL渲染上下文
    hglrc = wglCreateContext(hdc);
    if (!hglrc) {
        return false;
    }
    
    // 激活渲染上下文（使其成为当前上下文）
    if (!wglMakeCurrent(hdc, hglrc)) {
        wglDeleteContext(hglrc);
        hglrc = NULL;
        return false;
    }
    
    return true;
}

// ============================================================================
// OpenGL函数加载
// ============================================================================

/**
 * @brief 加载OpenGL扩展函数
 * @return 所有必需函数加载成功返回true，否则返回false
 * 
 * Windows默认只支持OpenGL 1.1，现代OpenGL功能需要通过
 * wglGetProcAddress动态加载函数指针。
 * 
 * 加载的函数类别：
 * 1. VAO函数 - 顶点数组对象管理
 * 2. VBO/EBO函数 - 缓冲区对象管理
 * 3. 顶点属性函数 - 顶点数据格式定义
 * 4. 着色器函数 - 着色器程序使用
 * 5. Uniform函数 - 着色器参数传递
 * 6. 纹理函数 - 纹理绑定和激活
 */
bool GraphicsEngine3D::LoadOpenGLFunctions() {
    // 加载VAO相关函数
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
    
    // 加载缓冲区相关函数
    glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
    
    // 加载顶点属性相关函数
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    
    // 加载绘制和着色器相关函数
    glDrawElementsExt = (PFNGLDRAWELEMENTSPROC_EXT)wglGetProcAddress("glDrawElements");
    glUseProgramExt = (PFNGLUSEPROGRAMPROC_EXT)wglGetProcAddress("glUseProgram");
    glGetUniformLocationExt = (PFNGLGETUNIFORMLOCATIONPROC_EXT)wglGetProcAddress("glGetUniformLocation");
    
    // 加载Uniform设置函数
    glUniformMatrix4fvExt = (PFNGLUNIFORMMATRIX4FVPROC_EXT)wglGetProcAddress("glUniformMatrix4fv");
    glUniform3fExt = (PFNGLUNIFORM3FPROC_EXT)wglGetProcAddress("glUniform3f");
    glUniform1fExt = (PFNGLUNIFORM1FPROC_EXT)wglGetProcAddress("glUniform1f");
    glUniform1iExt = (PFNGLUNIFORM1IPROC_EXT)wglGetProcAddress("glUniform1i");
    
    // 加载纹理相关函数
    glActiveTextureExt = (PFNGLACTIVETEXTUREPROC_EXT)wglGetProcAddress("glActiveTexture");
    glBindTextureExt = (PFNGLBINDTEXTUREPROC_EXT)wglGetProcAddress("glBindTexture");
    
    // 验证所有必需函数是否加载成功
    return (glGenVertexArrays && glBindVertexArray && glGenBuffers && 
            glBindBuffer && glBufferData && glVertexAttribPointer && 
            glEnableVertexAttribArray && glDrawElementsExt && glUseProgramExt &&
            glGetUniformLocationExt && glUniformMatrix4fvExt && glUniform3fExt &&
            glUniform1fExt && glUniform1iExt && glActiveTextureExt && glBindTextureExt &&
            glDeleteVertexArrays && glDeleteBuffers);
}

// ============================================================================
// 光照更新
// ============================================================================

/**
 * @brief 更新光照参数到着色器程序
 * 
 * 当用户通过光照对话框修改光照参数后调用此函数，
 * 将新的光照参数传递给着色器程序。
 * 
 * 更新的参数包括：
 * - lightPos: 光源位置(x, y, z)
 * - lightColor: 光源颜色(r, g, b)
 * - lightAmbientIntensity: 环境光强度
 * - lightDiffuseIntensity: 漫反射强度
 * - lightSpecularIntensity: 镜面反射强度
 */
void GraphicsEngine3D::UpdateLight() {
    // 调试输出缓冲区
    char debugMsg[512];
    
    // 调试输出：检查shapes数量
    sprintf_s(debugMsg, "UpdateLight开始: shapes数量=%zu, isInitialized=%d", 
              shapes.size(), isInitialized ? 1 : 0);
    OutputDebugStringA(debugMsg);
    
    // 检查引擎是否已初始化
    if (!isInitialized || shaderProgram == 0) {
        OutputDebugStringA("UpdateLight: 引擎未初始化，直接返回");
        return;
    }
    
    // 确保OpenGL上下文是当前的
    wglMakeCurrent(hdc, hglrc);
    
    // 激活着色器程序
    glUseProgramExt(shaderProgram);
    
    // 获取着色器中uniform变量的位置
    int lightPosLoc = glGetUniformLocationExt(shaderProgram, "lightPos");
    int lightColorLoc = glGetUniformLocationExt(shaderProgram, "lightColor");
    int lightAmbientIntensityLoc = glGetUniformLocationExt(shaderProgram, "lightAmbientIntensity");
    int lightDiffuseIntensityLoc = glGetUniformLocationExt(shaderProgram, "lightDiffuseIntensity");
    int lightSpecularIntensityLoc = glGetUniformLocationExt(shaderProgram, "lightSpecularIntensity");
    
    // 设置光源位置
    if (lightPosLoc >= 0) {
        glUniform3fExt(lightPosLoc, light.positionX, light.positionY, light.positionZ);
    }
    
    // 设置光源颜色
    if (lightColorLoc >= 0) {
        glUniform3fExt(lightColorLoc, light.color[0], light.color[1], light.color[2]);
    }
    
    // 设置环境光强度
    if (lightAmbientIntensityLoc >= 0) {
        glUniform1fExt(lightAmbientIntensityLoc, light.ambientIntensity);
    }
    
    // 设置漫反射强度
    if (lightDiffuseIntensityLoc >= 0) {
        glUniform1fExt(lightDiffuseIntensityLoc, light.diffuseIntensity);
    }
    
    // 设置镜面反射强度
    if (lightSpecularIntensityLoc >= 0) {
        glUniform1fExt(lightSpecularIntensityLoc, light.specularIntensity);
    }
    
    // 重要：禁用着色器程序，恢复固定管线状态
    // 如果不禁用，后续的固定管线渲染会出问题
    glUseProgramExt(0);
    
    // 调试输出：更新后的shapes数量
    sprintf_s(debugMsg, "UpdateLight结束: shapes数量=%zu", shapes.size());
    OutputDebugStringA(debugMsg);
    
    // 调试输出
    sprintf_s(debugMsg, 
        "光照参数已更新: 位置(%.2f, %.2f, %.2f) 强度(环境:%.2f, 漫反射:%.2f, 镜面:%.2f) 颜色(%.2f, %.2f, %.2f)",
        light.positionX, light.positionY, light.positionZ,
        light.ambientIntensity, light.diffuseIntensity, light.specularIntensity,
        light.color[0], light.color[1], light.color[2]);
    OutputDebugStringA(debugMsg);
}

// ============================================================================
// 上下文管理
// ============================================================================

/**
 * @brief 释放OpenGL上下文
 * 
 * 在切换到2D渲染模式时调用，释放当前OpenGL上下文，
 * 允许其他渲染系统使用设备上下文。
 */
void GraphicsEngine3D::ReleaseContext() {
    wglMakeCurrent(NULL, NULL);
}

// ============================================================================
// 场景管理
// ============================================================================

/**
 * @brief 清空3D场景中的所有图形
 * 
 * 清除shapes容器中的所有3D图形对象，
 * 并重置选择状态。
 */
void GraphicsEngine3D::ClearScene() {
    shapes.clear();
    selectedShapeIndex = -1;
    hasSelection = false;
}

/**
 * @brief 设置当前绘图模式
 * @param mode 要设置的绘图模式
 */
void GraphicsEngine3D::SetMode(DrawMode mode) {
    currentMode = mode;
}
