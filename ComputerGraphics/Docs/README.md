# 计算机图形学实验系统

## 项目简介

本项目是一个完整的计算机图形学实验系统，基于 Windows GDI 和 OpenGL 实现，涵盖了 2D 和 3D 图形学的核心算法和技术。项目采用 C++ 开发，使用 Visual Studio 作为开发环境。

## 功能特性

### 2D 图形功能
- **直线绘制**：DDA 算法、Bresenham 算法
- **圆形绘制**：中点圆算法、Bresenham 圆算法
- **区域填充**：边界填充算法、扫描线填充算法
- **几何变换**：平移、旋转、缩放
- **裁剪算法**：
  - Cohen-Sutherland 直线裁剪
  - 中点分割直线裁剪
  - Sutherland-Hodgman 多边形裁剪
  - Weiler-Atherton 多边形裁剪

### 3D 图形功能
- **基本图元**：立方体、球体、圆柱体、平面
- **光照系统**：Phong 光照模型（环境光、漫反射、镜面反射）
- **材质系统**：可调节的材质属性
- **纹理映射**：支持图片纹理加载
- **交互操作**：视角旋转、缩放、物体选择和拖拽

## 项目目录结构

```
ComputerGraphics/
├── ComputerGraphics.sln          # Visual Studio 解决方案文件
├── ComputerGraphics/             # 主项目目录
│   ├── src/                      # 源代码目录
│   │   ├── core/                 # 核心数据结构
│   │   ├── math/                 # 数学工具库
│   │   ├── algorithms/           # 图形算法实现
│   │   ├── engine/               # 图形引擎
│   │   ├── ui/                   # 用户界面
│   │   └── main.cpp              # 程序入口
│   ├── ComputerGraphics.vcxproj  # 项目配置文件
│   └── *.h, *.rc, *.ico          # 资源文件
├── Docs/                         # 文档目录
│   ├── README.md                 # 项目说明（本文件）
│   ├── CODE_NAVIGATION.md        # 代码导航文档
│   ├── Requriments.md            # 需求文档
│   └── diagrams/                 # UML 图表
└── external/                     # 第三方库
    ├── glad/                     # OpenGL 加载器
    ├── glm/                      # 数学库
    └── stb/                      # 图像加载库
```

## 源代码模块说明

### core/ - 核心数据结构
存放项目的基础数据类型定义。

| 文件 | 说明 |
|------|------|
| `Point2D.h` | 二维点结构，用于 2D 图形绘制 |
| `Point3D.h` | 三维点结构，用于 3D 图形绘制 |
| `Shape.h` | 二维图形结构，包含类型、顶点、颜色等属性 |
| `Shape3D.h` | 三维图形结构，包含变换、材质、纹理等属性 |
| `DrawMode.h` | 绘图模式枚举，定义各种绘图操作类型 |

### math/ - 数学工具库
存放图形学相关的数学运算工具。

| 文件 | 说明 |
|------|------|
| `Matrix4.h` | 4x4 矩阵运算类，支持透视投影、视图变换、模型变换 |

### algorithms/ - 图形算法实现
存放各种图形学算法的具体实现。

| 文件 | 说明 |
|------|------|
| `LineDrawer.*` | 直线绘制算法（DDA、Bresenham） |
| `CircleDrawer.*` | 圆形绘制算法（中点圆、Bresenham 圆） |
| `FillAlgorithms.*` | 区域填充算法（边界填充、扫描线填充） |
| `ClippingAlgorithms.*` | 裁剪算法（Cohen-Sutherland、中点分割、Sutherland-Hodgman、Weiler-Atherton） |
| `TransformAlgorithms.*` | 几何变换算法（平移、旋转、缩放） |
| `MeshGenerator.*` | 3D 网格生成器（立方体、球体、圆柱体、平面） |
| `ShaderManager.*` | OpenGL 着色器管理（编译、链接、使用） |
| `TextureLoader.*` | 纹理加载器（支持常见图片格式） |

### engine/ - 图形引擎
存放图形渲染引擎的核心代码。

| 文件 | 说明 |
|------|------|
| `GraphicsEngine.*` | 2D 图形引擎，处理 2D 绑定和渲染 |
| `GraphicsEngine3D.h` | 3D 图形引擎头文件 |
| `GraphicsEngine3D_Core.cpp` | 3D 引擎核心：初始化、OpenGL 上下文管理 |
| `GraphicsEngine3D_Render.cpp` | 3D 引擎渲染：场景渲染、光照计算 |
| `GraphicsEngine3D_Input.cpp` | 3D 引擎输入：鼠标交互、视角控制 |
| `OpenGLFunctions.h` | OpenGL 函数指针声明 |
| `ShapeRenderer.*` | 图形渲染器，负责具体图形的绘制 |
| `ShapeSelector.*` | 图形选择器，处理图形的选中和高亮 |

### ui/ - 用户界面
存放对话框和菜单相关代码。

| 文件 | 说明 |
|------|------|
| `Dialogs3D.h` | 3D 对话框类声明 |
| `TransformDialog3D.cpp` | 变换参数对话框（平移、旋转、缩放） |
| `LightingDialog.cpp` | 光照设置对话框（光源位置、颜色、强度） |
| `MaterialDialog.cpp` | 材质编辑对话框（环境光、漫反射、镜面反射） |
| `TextureDialog.cpp` | 纹理设置对话框（加载、应用纹理） |
| `MenuIDs.h` | 菜单 ID 定义 |

## 编译和运行

### 环境要求
- Windows 10/11
- Visual Studio 2019 或更高版本
- C++17 支持

### 第三方库配置
请参考 `external/README.md` 和 `external/SETUP_INSTRUCTIONS.md` 配置以下库：
- GLAD (OpenGL 3.3 Core Profile)
- GLM (数学库)
- stb_image (图像加载)

### 编译步骤
1. 打开 `ComputerGraphics.sln`
2. 确保第三方库已正确配置
3. 选择 Debug 或 Release 配置
4. 按 F5 编译并运行

## 相关文档

- [代码导航文档](CODE_NAVIGATION.md) - 快速定位各功能的代码位置
- [需求文档](Requriments.md) - 项目功能需求说明
- [UML 图表](diagrams/README.md) - 系统架构和类图

## 技术栈

- **语言**：C++17
- **2D 渲染**：Windows GDI
- **3D 渲染**：OpenGL 3.3 Core Profile
- **数学库**：GLM
- **图像加载**：stb_image
- **开发环境**：Visual Studio 2019+
