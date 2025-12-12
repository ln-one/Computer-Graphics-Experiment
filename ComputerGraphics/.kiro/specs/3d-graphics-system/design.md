# Design Document - 3D Graphics System

## Overview

在现有2D系统基础上添加3D功能，**完全保留2D代码**。3D模块参照2D架构设计，结构对称，易于理解和演示。

### 设计原则

1. **保留2D**：不修改任何现有2D代码
2. **架构对称**：3D模块结构与2D模块对应
3. **分层清晰**：数据层、算法层、引擎层、UI层
4. **易于演示**：每个功能独立，便于定位代码

### 技术选择

- **3D渲染**：OpenGL 3.3+
- **数学库**：GLM (header-only)
- **纹理加载**：stb_image (单文件)

## Architecture

### 整体架构（参照2D架构）

```
┌─────────────────────────────────────────────────────┐
│                    main.cpp                         │
│  - 窗口管理                                          │
│  - 菜单处理                                          │
│  - 模式切换 (2D/3D)                                  │
└──────────────┬──────────────────┬───────────────────┘
               │                  │
    ┌──────────▼────────┐  ┌─────▼──────────────┐
    │ GraphicsEngine    │  │ GraphicsEngine3D   │
    │ (现有2D引擎)      │  │ (新增3D引擎)       │
    │                   │  │                    │
    │ - SetMode()       │  │ - SetMode()        │
    │ - OnLButtonDown() │  │ - OnLButtonDown()  │
    │ - RenderAll()     │  │ - Render()         │
    └──────────┬────────┘  └─────┬──────────────┘
               │                  │
    ┌──────────▼────────┐  ┌─────▼──────────────┐
    │ ShapeRenderer     │  │ ShapeRenderer3D    │
    │ (2D渲染器)        │  │ (3D渲染器)         │
    └──────────┬────────┘  └─────┬──────────────┘
               │                  │
    ┌──────────▼────────┐  ┌─────▼──────────────┐
    │ 算法类            │  │ 算法类             │
    │ - LineDrawer      │  │ - MeshGenerator    │
    │ - CircleDrawer    │  │ - ShaderManager    │
    │ - FillAlgorithms  │  │ - TextureLoader    │
    └───────────────────┘  └────────────────────┘
```

### 文件结构（对称设计）

```
ComputerGraphics/src/
├── core/                    # 核心数据结构
│   ├── Point2D.h           (现有)
│   ├── Shape.h             (现有)
│   ├── DrawMode.h          (现有，需扩展)
│   ├── Point3D.h           (新增)
│   └── Shape3D.h           (新增)
│
├── algorithms/              # 算法层
│   ├── LineDrawer.cpp      (现有2D)
│   ├── CircleDrawer.cpp    (现有2D)
│   ├── MeshGenerator.cpp   (新增3D)
│   └── ShaderManager.cpp   (新增3D)
│
├── engine/                  # 引擎层
│   ├── GraphicsEngine.cpp  (现有2D)
│   ├── ShapeRenderer.cpp   (现有2D)
│   ├── GraphicsEngine3D.cpp (新增3D)
│   └── ShapeRenderer3D.cpp  (新增3D)
│
├── ui/                      # UI层
│   ├── MenuIDs.h           (现有，需扩展)
│   └── Dialogs3D.cpp       (新增3D对话框)
│
└── main.cpp                (现有，需扩展)
```

## Components and Interfaces

### 1. 核心数据结构

#### Point3D.h - 三维点

```cpp
#pragma once

struct Point3D {
    float x, y, z;
    
    Point3D() : x(0), y(0), z(0) {}
    Point3D(float x, float y, float z) : x(x), y(y), z(z) {}
};
```

#### Shape3D.h - 三维图形（参照Shape.h设计）

```cpp
#pragma once
#include "Point3D.h"
#include <vector>

// 3D图形类型
enum Shape3DType {
    SHAPE3D_SPHERE,
    SHAPE3D_CYLINDER,
    SHAPE3D_PLANE,
    SHAPE3D_CUBE
};

// 3D图形结构（参照2D的Shape结构）
struct Shape3D {
    Shape3DType type;
    
    // 变换参数
    Point3D position;
    Point3D rotation;  // 绕X,Y,Z轴旋转角度
    Point3D scale;
    
    // 材质参数
    float ambient[3];   // 环境光
    float diffuse[3];   // 漫反射
    float specular[3];  // 镜面反射
    float shininess;    // 光泽度
    
    // 纹理
    unsigned int textureID;
    bool hasTexture;
    
    // 网格数据
    std::vector<float> vertices;  // 顶点数据
    std::vector<unsigned int> indices;  // 索引数据
    unsigned int VAO, VBO, EBO;  // OpenGL缓冲对象
    
    bool selected;
    
    Shape3D() : type(SHAPE3D_SPHERE), 
                position(0,0,0), rotation(0,0,0), scale(1,1,1),
                shininess(32.0f), textureID(0), hasTexture(false),
                VAO(0), VBO(0), EBO(0), selected(false) {
        // 默认材质
        ambient[0] = ambient[1] = ambient[2] = 0.2f;
        diffuse[0] = diffuse[1] = diffuse[2] = 0.8f;
        specular[0] = specular[1] = specular[2] = 1.0f;
    }
};
```


### 2. 引擎层

#### GraphicsEngine3D.h - 3D图形引擎（参照GraphicsEngine设计）

```cpp
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
    Point3D target;    // 观察目标点
};

// 光源参数
struct Light {
    Point3D position;
    float color[3];
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
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
    
    // 私有辅助方法（参照GraphicsEngine的Handle方法）
    void HandleShapeCreation(int x, int y);
    void HandleSelection(int x, int y);
    void HandleViewControl(int x, int y);
};
```

#### ShapeRenderer3D.h - 3D图形渲染器（参照ShapeRenderer设计）

```cpp
#pragma once
#include "../core/Shape3D.h"

// 3D图形渲染器（参照ShapeRenderer）
class ShapeRenderer3D {
public:
    // 渲染单个图形（参照ShapeRenderer::DrawShape）
    static void RenderShape(const Shape3D& shape, unsigned int shaderProgram);
    
    // 绘制选择指示器（参照ShapeSelector::DrawSelectionIndicator）
    static void DrawSelectionIndicator(const Shape3D& shape);
};
```

### 3. 算法层

#### MeshGenerator.h - 网格生成算法（参照LineDrawer等算法类）

```cpp
#pragma once
#include "../core/Shape3D.h"
#include <vector>

// 网格生成算法类（参照LineDrawer, CircleDrawer的设计）
class MeshGenerator {
public:
    // 生成球体网格
    static void GenerateSphere(Shape3D& shape, float radius, 
                               int segments, int rings);
    
    // 生成柱体网格
    static void GenerateCylinder(Shape3D& shape, float radius, 
                                 float height, int segments);
    
    // 生成平面网格
    static void GeneratePlane(Shape3D& shape, float width, float height);
    
    // 生成立方体网格
    static void GenerateCube(Shape3D& shape, float size);
    
private:
    // 辅助方法：创建OpenGL缓冲对象
    static void CreateBuffers(Shape3D& shape);
};
```

#### ShaderManager.h - 着色器管理（新增）

```cpp
#pragma once
#include <string>

// 着色器管理类
class ShaderManager {
public:
    // 创建着色器程序
    static unsigned int CreateShaderProgram(
        const char* vertexSource, 
        const char* fragmentSource);
    
    // 获取默认着色器
    static const char* GetDefaultVertexShader();
    static const char* GetDefaultFragmentShader();
    
private:
    static unsigned int CompileShader(const char* source, unsigned int type);
    static bool CheckCompileErrors(unsigned int shader, const std::string& type);
};
```

#### TextureLoader.h - 纹理加载（新增）

```cpp
#pragma once
#include <string>

// 纹理加载类
class TextureLoader {
public:
    // 从文件加载纹理
    static unsigned int LoadTexture(const std::string& filepath);
    
    // 删除纹理
    static void DeleteTexture(unsigned int textureID);
};
```


### 4. UI层

#### MenuIDs.h - 菜单ID扩展

```cpp
// 在现有MenuIDs.h中添加3D相关ID

// === 现有2D菜单ID（不变）===
#define ID_FILE_NEW 1001
#define ID_DRAW_LINE_DDA 2001
// ... 其他2D ID ...

// === 新增3D菜单ID ===
#define ID_MODE_2D 5001
#define ID_MODE_3D 5002

#define ID_3D_SPHERE 6001
#define ID_3D_CYLINDER 6002
#define ID_3D_PLANE 6003
#define ID_3D_CUBE 6004

#define ID_3D_SELECT 6101
#define ID_3D_VIEW_CONTROL 6102
#define ID_3D_LIGHTING 6103
```

#### Dialogs3D.h - 3D对话框

```cpp
#pragma once
#include "../core/Shape3D.h"
#include <windows.h>

// 变换参数对话框
class TransformDialog3D {
public:
    static bool Show(HWND parent, Shape3D* shape);
private:
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, 
                                       WPARAM wParam, LPARAM lParam);
};

// 光照设置对话框
class LightingDialog {
public:
    static bool Show(HWND parent, Light* light);
private:
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, 
                                       WPARAM wParam, LPARAM lParam);
};

// 材质编辑对话框
class MaterialDialog {
public:
    static bool Show(HWND parent, Shape3D* shape);
private:
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, 
                                       WPARAM wParam, LPARAM lParam);
};

// 纹理设置对话框
class TextureDialog {
public:
    static bool Show(HWND parent, Shape3D* shape);
private:
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, 
                                       WPARAM wParam, LPARAM lParam);
};
```

## Data Models

### 顶点数据格式

```cpp
// 每个顶点包含：位置、法线、纹理坐标
// 存储在Shape3D.vertices中，格式：
// [x, y, z, nx, ny, nz, u, v, x, y, z, nx, ny, nz, u, v, ...]
//  \_____/  \________/  \___/
//  位置(3)   法线(3)    纹理(2)
```

### 变换矩阵计算

```cpp
// 使用GLM库计算变换矩阵
glm::mat4 model = glm::mat4(1.0f);

// 1. 平移
model = glm::translate(model, glm::vec3(shape.position.x, 
                                        shape.position.y, 
                                        shape.position.z));

// 2. 旋转（按Z-Y-X顺序）
model = glm::rotate(model, glm::radians(shape.rotation.z), 
                    glm::vec3(0, 0, 1));
model = glm::rotate(model, glm::radians(shape.rotation.y), 
                    glm::vec3(0, 1, 0));
model = glm::rotate(model, glm::radians(shape.rotation.x), 
                    glm::vec3(1, 0, 0));

// 3. 缩放
model = glm::scale(model, glm::vec3(shape.scale.x, 
                                    shape.scale.y, 
                                    shape.scale.z));
```

### 摄像机矩阵计算

```cpp
// 视图矩阵
glm::vec3 cameraPos = glm::vec3(
    camera.target.x + camera.distance * cos(camera.angleY) * cos(camera.angleX),
    camera.target.y + camera.distance * sin(camera.angleY),
    camera.target.z + camera.distance * cos(camera.angleY) * sin(camera.angleX)
);

glm::mat4 view = glm::lookAt(
    cameraPos,
    glm::vec3(camera.target.x, camera.target.y, camera.target.z),
    glm::vec3(0, 1, 0)
);

// 投影矩阵
glm::mat4 projection = glm::perspective(
    glm::radians(45.0f),  // FOV
    aspectRatio,
    0.1f,   // near plane
    100.0f  // far plane
);
```

## Error Handling

### 1. OpenGL初始化失败

```cpp
bool GraphicsEngine3D::Initialize(HWND hwnd) {
    // 创建OpenGL上下文
    if (!CreateOpenGLContext()) {
        MessageBox(hwnd, L"无法创建OpenGL上下文\n请确保显卡支持OpenGL 3.3+", 
                   L"初始化失败", MB_OK | MB_ICONERROR);
        return false;
    }
    
    // 加载OpenGL函数
    if (!LoadOpenGLFunctions()) {
        MessageBox(hwnd, L"无法加载OpenGL函数", 
                   L"初始化失败", MB_OK | MB_ICONERROR);
        return false;
    }
    
    return true;
}
```

### 2. 着色器编译失败

```cpp
bool ShaderManager::CheckCompileErrors(unsigned int shader, 
                                       const std::string& type) {
    int success;
    char infoLog[512];
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        
        std::string msg = "着色器编译失败 (" + type + "):\n" + infoLog;
        MessageBoxA(NULL, msg.c_str(), "着色器错误", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}
```

### 3. 纹理加载失败

```cpp
unsigned int TextureLoader::LoadTexture(const std::string& filepath) {
    // 使用stb_image加载
    int width, height, channels;
    unsigned char* data = stbi_load(filepath.c_str(), 
                                     &width, &height, &channels, 0);
    
    if (!data) {
        std::string msg = "无法加载纹理文件:\n" + filepath;
        MessageBoxA(NULL, msg.c_str(), "纹理加载失败", MB_OK | MB_ICONERROR);
        return 0;
    }
    
    // 创建OpenGL纹理...
    
    stbi_image_free(data);
    return textureID;
}
```

### 4. 对话框输入验证

```cpp
INT_PTR TransformDialog3D::DialogProc(HWND hwnd, UINT msg, 
                                      WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COMMAND && LOWORD(wParam) == IDOK) {
        // 获取输入值
        char buffer[256];
        GetDlgItemTextA(hwnd, IDC_EDIT_POS_X, buffer, 256);
        
        // 验证输入
        float value;
        if (sscanf(buffer, "%f", &value) != 1) {
            MessageBox(hwnd, L"请输入有效的数字", 
                       L"输入错误", MB_OK | MB_ICONWARNING);
            return TRUE;
        }
        
        // 应用值...
    }
}
```






## Implementation Details

### 1. main.cpp 修改（最小化修改）

```cpp
// 全局变量
GraphicsEngine g_engine;      // 现有2D引擎
GraphicsEngine3D g_engine3D;  // 新增3D引擎
bool is3DMode = false;        // 模式标志

// WM_CREATE - 添加3D菜单
case WM_CREATE: {
    // ... 现有2D菜单代码 ...
    
    // 新增：模式切换菜单
    HMENU hModeMenu = CreatePopupMenu();
    AppendMenuW(hModeMenu, MF_STRING, ID_MODE_2D, L"2D Mode");
    AppendMenuW(hModeMenu, MF_STRING, ID_MODE_3D, L"3D Mode");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hModeMenu, L"Mode");
    
    // 新增：3D图形菜单
    HMENU h3DMenu = CreatePopupMenu();
    AppendMenuW(h3DMenu, MF_STRING, ID_3D_SPHERE, L"Sphere");
    AppendMenuW(h3DMenu, MF_STRING, ID_3D_CYLINDER, L"Cylinder");
    AppendMenuW(h3DMenu, MF_STRING, ID_3D_PLANE, L"Plane");
    AppendMenuW(h3DMenu, MF_STRING, ID_3D_CUBE, L"Cube");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)h3DMenu, L"3D Shapes");
    
    // 新增：3D控制菜单
    HMENU h3DControlMenu = CreatePopupMenu();
    AppendMenuW(h3DControlMenu, MF_STRING, ID_3D_SELECT, L"Select");
    AppendMenuW(h3DControlMenu, MF_STRING, ID_3D_VIEW_CONTROL, L"View Control");
    AppendMenuW(h3DControlMenu, MF_STRING, ID_3D_LIGHTING, L"Lighting");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)h3DControlMenu, L"3D Control");
    
    SetMenu(hwnd, hMenuBar);
    
    // 初始化3D引擎
    g_engine3D.Initialize(hwnd);
    return 0;
}

// WM_PAINT - 根据模式渲染
case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    
    if (is3DMode) {
        g_engine3D.Render();  // 3D渲染
    } else {
        g_engine.Initialize(hwnd, hdc);
        g_engine.RenderAll();  // 2D渲染
    }
    
    EndPaint(hwnd, &ps);
    return 0;
}

// WM_LBUTTONDOWN - 根据模式处理
case WM_LBUTTONDOWN: {
    if (is3DMode) {
        g_engine3D.OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
    } else {
        HDC hdc = GetDC(hwnd);
        g_engine.Initialize(hwnd, hdc);
        g_engine.OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
        ReleaseDC(hwnd, hdc);
    }
    InvalidateRect(hwnd, NULL, FALSE);
    return 0;
}

// WM_COMMAND - 添加3D命令处理
case WM_COMMAND: {
    switch (LOWORD(wParam)) {
        // ... 现有2D命令 ...
        
        // 模式切换
        case ID_MODE_2D:
            is3DMode = false;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        case ID_MODE_3D:
            is3DMode = true;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        
        // 3D图形绘制
        case ID_3D_SPHERE:
            g_engine3D.SetMode(MODE_3D_SPHERE);
            break;
        case ID_3D_CYLINDER:
            g_engine3D.SetMode(MODE_3D_CYLINDER);
            break;
        // ... 其他3D命令 ...
    }
    return 0;
}
```

### 2. 着色器代码（嵌入在ShaderManager中）

```cpp
const char* ShaderManager::GetDefaultVertexShader() {
    return R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoord = aTexCoord;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
}

const char* ShaderManager::GetDefaultFragmentShader() {
    return R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoord;
        
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 viewPos;
        
        uniform vec3 ambient;
        uniform vec3 diffuse;
        uniform vec3 specular;
        uniform float shininess;
        
        uniform bool useTexture;
        uniform sampler2D textureSampler;
        
        void main() {
            // Phong光照模型
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            
            // 环境光
            vec3 ambientColor = ambient * lightColor;
            
            // 漫反射
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuseColor = diffuse * diff * lightColor;
            
            // 镜面反射
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
            vec3 specularColor = specular * spec * lightColor;
            
            vec3 result = ambientColor + diffuseColor + specularColor;
            
            if (useTexture) {
                result *= texture(textureSampler, TexCoord).rgb;
            }
            
            FragColor = vec4(result, 1.0);
        }
    )";
}
```

### 3. 外部库集成

#### GLM (数学库)
- 下载：https://github.com/g-truc/glm
- 使用：Header-only，直接包含头文件
- 示例：`#include <glm/glm.hpp>`

#### GLAD (OpenGL加载器)
- 生成：https://glad.dav1d.de/
- 配置：OpenGL 3.3, Core Profile
- 集成：将生成的 `glad.c` 和 `glad.h` 添加到项目

#### stb_image (图像加载)
- 下载：https://github.com/nothings/stb
- 使用：单头文件 `stb_image.h`
- 集成：在一个 `.cpp` 文件中定义 `#define STB_IMAGE_IMPLEMENTATION`

## Design Decisions

### 1. 为什么使用OpenGL而不是Direct3D？
- **学习曲线**：OpenGL更简单，资源更多
- **跨平台**：虽然本项目是Windows，但OpenGL便于未来扩展
- **集成简单**：与现有GDI系统共存更容易

### 2. 为什么不修改现有2D代码？
- **风险控制**：避免破坏已有功能
- **清晰分离**：2D和3D逻辑独立，易于理解
- **便于演示**：可以对比展示2D和3D功能

### 3. 为什么使用对话框而不是实时UI？
- **符合要求**：实验要求使用对话框
- **实现简单**：Windows对话框API成熟
- **易于演示**：参数修改直观明确

### 4. 为什么着色器代码嵌入而不是外部文件？
- **简化部署**：不需要额外的shader文件
- **便于演示**：所有代码在项目中，易于定位
- **减少错误**：避免文件路径问题

