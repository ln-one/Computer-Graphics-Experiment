# 代码导航文档

本文档帮助快速定位计算机图形学实验系统中各功能的代码实现位置。

## 项目结构概述

```
ComputerGraphics/
├── src/
│   ├── core/           # 核心数据结构
│   │   ├── Point2D.h       - 二维点结构
│   │   ├── Point3D.h       - 三维点结构
│   │   ├── Shape.h         - 二维图形结构
│   │   ├── Shape3D.h       - 三维图形结构
│   │   └── DrawMode.h      - 绘图模式枚举
│   │
│   ├── math/           # 数学工具
│   │   └── Matrix4.h       - 4x4矩阵运算（透视投影、视图变换等）
│   │
│   ├── algorithms/     # 图形算法
│   │   ├── LineDrawer.*        - 直线绘制算法（DDA、Bresenham）
│   │   ├── CircleDrawer.*      - 圆形绘制算法（中点圆、Bresenham圆）
│   │   ├── FillAlgorithms.*    - 填充算法（边界填充、扫描线填充）
│   │   ├── ClippingAlgorithms.*- 裁剪算法（4种算法）
│   │   ├── TransformAlgorithms.*- 几何变换（平移、缩放、旋转）
│   │   ├── MeshGenerator.*     - 3D网格生成
│   │   ├── ShaderManager.*     - 着色器管理
│   │   └── TextureLoader.*     - 纹理加载
│   │
│   ├── engine/         # 图形引擎
│   │   ├── GraphicsEngine.*        - 2D图形引擎
│   │   ├── GraphicsEngine3D.h      - 3D引擎头文件
│   │   ├── GraphicsEngine3D_Core.cpp   - 3D引擎初始化和OpenGL上下文
│   │   ├── GraphicsEngine3D_Render.cpp - 3D渲染相关
│   │   ├── GraphicsEngine3D_Input.cpp  - 3D鼠标交互
│   │   ├── ShapeRenderer.*         - 图形渲染器
│   │   ├── ShapeSelector.*         - 图形选择器
│   │   └── OpenGLFunctions.h       - OpenGL函数声明
│   │
│   ├── ui/             # 用户界面
│   │   ├── Dialogs3D.h             - 3D对话框头文件
│   │   ├── TransformDialog3D.cpp   - 变换参数对话框
│   │   ├── LightingDialog.cpp      - 光照设置对话框
│   │   ├── MaterialDialog.cpp      - 材质编辑对话框
│   │   ├── TextureDialog.cpp       - 纹理设置对话框
│   │   └── MenuIDs.h               - 菜单ID定义
│   │
│   └── main.cpp        # 程序入口
│
└── Docs/               # 文档目录
    ├── diagrams/           - PlantUML图表
    └── CODE_NAVIGATION.md  - 本文档
```

## 功能模块索引表

| 功能分类 | 功能名称 | 文件位置 | 关键函数/类 |
|---------|---------|---------|------------|
| 2D直线 | DDA算法 | `algorithms/LineDrawer.cpp` | `LineDrawer::DrawDDA()` |
| 2D直线 | Bresenham算法 | `algorithms/LineDrawer.cpp` | `LineDrawer::DrawBresenham()` |
| 2D圆形 | 中点圆算法 | `algorithms/CircleDrawer.cpp` | `CircleDrawer::DrawMidpoint()` |
| 2D圆形 | Bresenham圆算法 | `algorithms/CircleDrawer.cpp` | `CircleDrawer::DrawBresenham()` |
| 2D填充 | 边界填充 | `algorithms/FillAlgorithms.cpp` | `FillAlgorithms::BoundaryFill()` |
| 2D填充 | 扫描线填充 | `algorithms/FillAlgorithms.cpp` | `FillAlgorithms::ScanlineFill()` |
| 2D裁剪 | Cohen-Sutherland | `algorithms/ClippingAlgorithms.cpp` | `ClippingAlgorithms::ClipLineCohenSutherland()` |
| 2D裁剪 | 中点分割 | `algorithms/ClippingAlgorithms.cpp` | `ClippingAlgorithms::ClipLineMidpoint()` |
| 2D裁剪 | Sutherland-Hodgman | `algorithms/ClippingAlgorithms.cpp` | `ClippingAlgorithms::ClipPolygonSutherlandHodgman()` |
| 2D裁剪 | Weiler-Atherton | `algorithms/ClippingAlgorithms.cpp` | `ClippingAlgorithms::ClipPolygonWeilerAtherton()` |
| 2D变换 | 平移/缩放/旋转 | `algorithms/TransformAlgorithms.cpp` | `TransformAlgorithms::Apply*()` |
| 3D网格 | 立方体/球体/柱体/平面 | `algorithms/MeshGenerator.cpp` | `MeshGenerator::Generate*()` |
| 3D渲染 | 场景渲染 | `engine/GraphicsEngine3D_Render.cpp` | `GraphicsEngine3D::Render()` |
| 3D交互 | 鼠标事件 | `engine/GraphicsEngine3D_Input.cpp` | `GraphicsEngine3D::On*()` |
| 3D光照 | 光照设置 | `ui/LightingDialog.cpp` | `LightingDialog::Show()` |
| 3D材质 | 材质编辑 | `ui/MaterialDialog.cpp` | `MaterialDialog::Show()` |
| 3D纹理 | 纹理加载 | `algorithms/TextureLoader.cpp` | `TextureLoader::LoadTexture()` |


---

## 2D 绘图算法导航

### 直线绘制算法

#### DDA（数字微分分析器）算法
- **文件**: `ComputerGraphics/src/algorithms/LineDrawer.cpp`
- **函数**: `LineDrawer::DrawDDA(HDC hdc, Point2D p1, Point2D p2, COLORREF color)`
- **算法原理**: 
  - 基于直线的微分方程 dy/dx = (y2-y1)/(x2-x1)
  - 选择步长较大的方向作为主方向，每次递增1
  - 另一方向按斜率递增
  - 使用浮点运算，需要四舍五入取整

#### Bresenham直线算法
- **文件**: `ComputerGraphics/src/algorithms/LineDrawer.cpp`
- **函数**: `LineDrawer::DrawBresenham(HDC hdc, Point2D p1, Point2D p2, COLORREF color)`
- **算法原理**:
  - 只使用整数运算，效率更高
  - 通过判别式决定下一个像素位置
  - 判别式 d = 2*dy - dx（初始值）
  - 若 d < 0，选择右方像素，d += 2*dy
  - 若 d >= 0，选择右上方像素，d += 2*(dy-dx)

### 圆形绘制算法

#### 中点圆算法
- **文件**: `ComputerGraphics/src/algorithms/CircleDrawer.cpp`
- **函数**: `CircleDrawer::DrawMidpoint(HDC hdc, Point2D center, int radius, COLORREF color)`
- **算法原理**:
  - 利用圆的八分对称性，只需计算1/8圆弧
  - 基于圆的隐式方程 F(x,y) = x² + y² - r²
  - 判断中点位置决定下一个像素
  - 初始点 (0, r)，判别式 d = 1 - r

#### Bresenham圆算法
- **文件**: `ComputerGraphics/src/algorithms/CircleDrawer.cpp`
- **函数**: `CircleDrawer::DrawBresenham(HDC hdc, Point2D center, int radius, COLORREF color)`
- **算法原理**:
  - 中点圆算法的改进版本
  - 只使用整数运算
  - 利用八分对称性绘制完整圆

### 填充算法

#### 边界填充算法（种子填充）
- **文件**: `ComputerGraphics/src/algorithms/FillAlgorithms.cpp`
- **函数**: `FillAlgorithms::BoundaryFill(HDC hdc, HWND hwnd, int x, int y, COLORREF fillColor, COLORREF boundaryColor)`
- **算法原理**:
  - 从种子点开始，向四个方向（上下左右）扩散
  - 遇到边界颜色或已填充颜色时停止
  - 使用栈实现非递归版本，避免栈溢出

#### 扫描线填充算法
- **文件**: `ComputerGraphics/src/algorithms/FillAlgorithms.cpp`
- **函数**: `FillAlgorithms::ScanlineFill(HDC hdc, const std::vector<Point2D>& polygon, COLORREF fillColor)`
- **算法原理**:
  - 构建边表（ET）和活性边表（AET）
  - 从最低扫描线开始，逐行处理
  - 计算扫描线与多边形边的交点
  - 按交点配对填充区间

---

## 裁剪算法导航

### Cohen-Sutherland 直线裁剪算法
- **文件**: `ComputerGraphics/src/algorithms/ClippingAlgorithms.cpp`
- **函数**: `ClippingAlgorithms::ClipLineCohenSutherland(Point2D& p1, Point2D& p2, int xmin, int ymin, int xmax, int ymax)`
- **算法原理**:
  1. 为每个端点计算4位区域编码（上下左右）
  2. 若两端点编码都为0，直线完全在窗口内，接受
  3. 若两端点编码按位与不为0，直线完全在窗口外，拒绝
  4. 否则，计算直线与窗口边界的交点，用交点替换窗口外的端点
  5. 重复步骤2-4直到得出结论
- **区域编码**:
  ```
  1001 | 1000 | 1010
  -----|------|-----
  0001 | 0000 | 0010  (0000为窗口内部)
  -----|------|-----
  0101 | 0100 | 0110
  ```

### 中点分割直线裁剪算法
- **文件**: `ComputerGraphics/src/algorithms/ClippingAlgorithms.cpp`
- **函数**: `ClippingAlgorithms::ClipLineMidpoint(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax, std::vector<std::pair<Point2D, Point2D>>& result)`
- **算法原理**:
  1. 若线段完全可见或完全不可见，直接处理
  2. 否则，计算线段中点
  3. 递归处理两个子线段
  4. 当线段足够短时停止递归
- **特点**: 使用二分法逼近交点，适合硬件实现

### Sutherland-Hodgman 多边形裁剪算法
- **文件**: `ComputerGraphics/src/algorithms/ClippingAlgorithms.cpp`
- **函数**: `ClippingAlgorithms::ClipPolygonSutherlandHodgman(const std::vector<Point2D>& polygon, int xmin, int ymin, int xmax, int ymax)`
- **算法原理**:
  1. 依次用裁剪窗口的四条边裁剪多边形
  2. 对每条边，遍历多边形的每条边
  3. 根据端点相对于裁剪边的位置，决定输出顶点：
     - 两点都在内侧：输出终点
     - 起点在内，终点在外：输出交点
     - 起点在外，终点在内：输出交点和终点
     - 两点都在外侧：不输出
- **适用范围**: 凸多边形裁剪窗口

### Weiler-Atherton 多边形裁剪算法
- **文件**: `ComputerGraphics/src/algorithms/ClippingAlgorithms.cpp`
- **函数**: `ClippingAlgorithms::ClipPolygonWeilerAtherton(const std::vector<Point2D>& polygon, int xmin, int ymin, int xmax, int ymax)`
- **算法原理**:
  1. 构建主多边形和裁剪多边形的顶点链表
  2. 计算所有交点，插入到两个链表中
  3. 标记交点为"进入点"或"退出点"
  4. 从进入点开始追踪：
     - 沿主多边形前进直到遇到退出点
     - 切换到裁剪多边形，沿边界前进直到遇到进入点
     - 重复直到回到起点
- **特点**: 支持凹多边形，可能产生多个裁剪结果


---

## 3D 图形系统导航

### 网格生成

3D图形的网格数据由 `MeshGenerator` 类生成，位于 `ComputerGraphics/src/algorithms/MeshGenerator.cpp`。

| 图形类型 | 生成函数 | 参数说明 |
|---------|---------|---------|
| 立方体 | `MeshGenerator::GenerateCube(Shape3D& shape, float size)` | size: 边长 |
| 球体 | `MeshGenerator::GenerateSphere(Shape3D& shape, float radius, int segments, int rings)` | radius: 半径, segments: 经线数, rings: 纬线数 |
| 圆柱体 | `MeshGenerator::GenerateCylinder(Shape3D& shape, float radius, float height, int segments)` | radius: 底面半径, height: 高度, segments: 圆周分段数 |
| 平面 | `MeshGenerator::GeneratePlane(Shape3D& shape, float width, float height)` | width: 宽度, height: 高度 |

**顶点数据格式**: 每个顶点包含8个float值
- 位置坐标 (x, y, z) - 3个float
- 法线向量 (nx, ny, nz) - 3个float
- 纹理坐标 (u, v) - 2个float

### 渲染流程

渲染相关代码位于 `ComputerGraphics/src/engine/GraphicsEngine3D_Render.cpp`。

**主要渲染函数**:
| 函数 | 说明 |
|-----|------|
| `GraphicsEngine3D::Render()` | 主渲染函数，执行完整的3D渲染流程 |
| `GraphicsEngine3D::RenderWithFixedPipeline()` | 使用OpenGL固定管线渲染（备用方案） |
| `GraphicsEngine3D::RenderCubeImmediate()` | 立即模式渲染立方体 |
| `GraphicsEngine3D::RenderSphereImmediate()` | 立即模式渲染球体 |
| `GraphicsEngine3D::RenderCylinderImmediate()` | 立即模式渲染圆柱体 |
| `GraphicsEngine3D::RenderPlaneImmediate()` | 立即模式渲染平面 |

**渲染流程**:
1. 清除颜色缓冲和深度缓冲
2. 设置投影矩阵（透视投影）
3. 设置视图矩阵（摄像机位置）
4. 遍历所有3D图形对象
5. 对每个图形：设置模型矩阵 → 设置材质参数 → 绘制网格
6. 交换缓冲区

### 交互操作

交互处理代码位于 `ComputerGraphics/src/engine/GraphicsEngine3D_Input.cpp`。

**鼠标事件处理函数**:
| 函数 | 说明 |
|-----|------|
| `OnLButtonDown(int x, int y)` | 鼠标左键按下 - 创建图形或开始选择 |
| `OnLButtonUp(int x, int y)` | 鼠标左键释放 - 结束拖拽 |
| `OnRButtonDown(int x, int y)` | 鼠标右键按下 - 开始视角旋转 |
| `OnRButtonUp(int x, int y)` | 鼠标右键释放 - 结束视角旋转 |
| `OnLButtonDoubleClick(int x, int y)` | 鼠标双击 - 打开属性对话框 |
| `OnMouseMove(int x, int y)` | 鼠标移动 - 拖拽物体或旋转视角 |
| `OnMouseWheel(int delta)` | 鼠标滚轮 - 缩放视图 |

**内部处理函数**:
| 函数 | 说明 |
|-----|------|
| `HandleShapeCreation(int x, int y)` | 处理3D图形创建 |
| `HandleSelection(int x, int y)` | 处理3D图形选择 |
| `HandleViewControl(int deltaX, int deltaY)` | 处理视角控制（旋转摄像机） |
| `HandleObjectDragging(int deltaX, int deltaY)` | 处理物体拖拽移动 |

### OpenGL 初始化

初始化代码位于 `ComputerGraphics/src/engine/GraphicsEngine3D_Core.cpp`。

| 函数 | 说明 |
|-----|------|
| `Initialize(HWND hwnd)` | 初始化3D引擎 |
| `Shutdown()` | 关闭3D引擎，释放资源 |
| `CreateOpenGLContext()` | 创建OpenGL渲染上下文 |
| `LoadOpenGLFunctions()` | 加载OpenGL扩展函数 |
| `UpdateLight()` | 更新光照参数到着色器 |
| `ReleaseContext()` | 释放OpenGL上下文 |

### 着色器管理

着色器代码位于 `ComputerGraphics/src/algorithms/ShaderManager.cpp`。

| 函数 | 说明 |
|-----|------|
| `CreateShaderProgram(vertexSource, fragmentSource)` | 创建着色器程序 |
| `GetDefaultVertexShader()` | 获取默认顶点着色器源码 |
| `GetDefaultFragmentShader()` | 获取默认片段着色器源码 |

**着色器功能**:
- 顶点着色器：处理顶点变换（MVP矩阵）、传递法线和纹理坐标
- 片段着色器：实现Phong光照模型、纹理采样

### 纹理加载

纹理加载代码位于 `ComputerGraphics/src/algorithms/TextureLoader.cpp`。

| 函数 | 说明 |
|-----|------|
| `LoadTexture(const std::string& filepath)` | 从文件加载纹理，返回纹理ID |
| `DeleteTexture(unsigned int textureID)` | 删除纹理，释放GPU内存 |
| `IsSupportedFormat(const std::string& filepath)` | 检查文件格式是否支持 |

**支持的格式**: BMP, JPG, PNG

---

## 常见问题快速定位

### 如何创建新的2D图形？

1. **设置绘图模式**: 在 `main.cpp` 的菜单处理中调用 `GraphicsEngine::SetMode()`
2. **处理鼠标事件**: `GraphicsEngine::OnLButtonDown()` 根据当前模式调用相应的处理函数
3. **调用绘图算法**: 
   - 直线: `LineDrawer::DrawDDA()` 或 `LineDrawer::DrawBresenham()`
   - 圆形: `CircleDrawer::DrawMidpoint()` 或 `CircleDrawer::DrawBresenham()`
   - 矩形/多边形: `GraphicsEngine::DrawRectangle()` / `DrawPolygon()`

**代码路径**:
```
main.cpp (菜单选择)
  → GraphicsEngine::SetMode() (设置模式)
  → GraphicsEngine::OnLButtonDown() (鼠标点击)
    → HandleLineDrawing() / HandleCircleDrawing() / ... (模式处理)
      → LineDrawer::DrawDDA() / CircleDrawer::DrawMidpoint() / ... (算法调用)
```

### 如何创建新的3D图形？

1. **设置绘图模式**: 调用 `GraphicsEngine3D::SetMode(MODE_3D_CUBE)` 等
2. **点击创建**: `GraphicsEngine3D::OnLButtonDown()` → `HandleShapeCreation()`
3. **生成网格**: `MeshGenerator::GenerateCube()` 等
4. **渲染显示**: `GraphicsEngine3D::Render()`

**代码路径**:
```
main.cpp (菜单选择)
  → GraphicsEngine3D::SetMode(MODE_3D_CUBE) (设置模式)
  → GraphicsEngine3D::OnLButtonDown() (鼠标点击)
    → HandleShapeCreation() (创建处理)
      → MeshGenerator::GenerateCube() (生成网格)
  → GraphicsEngine3D::Render() (渲染)
```

### 如何修改3D光照？

1. **打开光照对话框**: 菜单 → 3D设置 → 光照设置
2. **对话框处理**: `LightingDialog::Show()` 显示对话框
3. **修改参数**: 用户在对话框中修改光源位置、颜色、强度
4. **更新着色器**: `GraphicsEngine3D::UpdateLight()` 将参数传递给着色器

**相关文件**:
- 对话框: `ComputerGraphics/src/ui/LightingDialog.cpp`
- 光源结构: `ComputerGraphics/src/engine/GraphicsEngine3D.h` 中的 `Light` 结构体
- 着色器更新: `ComputerGraphics/src/engine/GraphicsEngine3D_Core.cpp` 中的 `UpdateLight()`

**光照参数说明**:
| 参数 | 说明 | 默认值 |
|-----|------|-------|
| positionX/Y/Z | 光源位置 | (5, 5, 5) |
| color[3] | 光源颜色(RGB) | (1, 1, 1) 白光 |
| ambientIntensity | 环境光强度 | 0.2 |
| diffuseIntensity | 漫反射强度 | 0.8 |
| specularIntensity | 镜面反射强度 | 1.0 |

### 如何添加纹理？

1. **选择3D图形**: 使用选择模式点击图形
2. **打开纹理对话框**: 双击图形或菜单 → 纹理设置
3. **选择纹理文件**: `TextureDialog::OpenFileDialog()` 打开文件选择器
4. **加载纹理**: `TextureLoader::LoadTexture()` 加载图片到GPU
5. **应用纹理**: 设置 `Shape3D::textureID` 和 `hasTexture = true`

**相关文件**:
- 对话框: `ComputerGraphics/src/ui/TextureDialog.cpp`
- 纹理加载: `ComputerGraphics/src/algorithms/TextureLoader.cpp`
- 图形结构: `ComputerGraphics/src/core/Shape3D.h`

**支持的纹理格式**: BMP, JPG, PNG

### 如何修改3D材质？

1. **选择3D图形**: 使用选择模式点击图形
2. **打开材质对话框**: 菜单 → 材质设置
3. **修改材质参数**: 环境光、漫反射、镜面反射系数和光泽度
4. **确认应用**: 点击确定，参数保存到 `Shape3D` 对象

**相关文件**:
- 对话框: `ComputerGraphics/src/ui/MaterialDialog.cpp`
- 图形结构: `ComputerGraphics/src/core/Shape3D.h`

**材质参数说明**:
| 参数 | 说明 | 范围 |
|-----|------|-----|
| ambient[3] | 环境光反射系数(RGB) | 0.0 - 1.0 |
| diffuse[3] | 漫反射系数(RGB) | 0.0 - 1.0 |
| specular[3] | 镜面反射系数(RGB) | 0.0 - 1.0 |
| shininess | 光泽度（高光集中程度） | 1.0 - 128.0 |

### 如何执行裁剪操作？

1. **绘制图形**: 先绘制要裁剪的直线或多边形
2. **选择裁剪算法**: 菜单 → 裁剪 → 选择算法
3. **定义裁剪窗口**: 拖拽鼠标定义矩形裁剪区域
4. **执行裁剪**: 系统自动对选中图形执行裁剪

**裁剪算法选择**:
| 算法 | 适用对象 | 特点 |
|-----|---------|-----|
| Cohen-Sutherland | 直线 | 使用区域编码，效率高 |
| 中点分割 | 直线 | 二分法逼近，适合硬件实现 |
| Sutherland-Hodgman | 多边形 | 逐边裁剪，只支持凸裁剪窗口 |
| Weiler-Atherton | 多边形 | 支持凹多边形，可产生多个结果 |

**相关文件**:
- 裁剪算法: `ComputerGraphics/src/algorithms/ClippingAlgorithms.cpp`
- 裁剪执行: `ComputerGraphics/src/engine/GraphicsEngine.cpp` 中的 `Execute*Clipping()` 函数

---

## 绘图模式枚举参考

所有绘图模式定义在 `ComputerGraphics/src/core/DrawMode.h`：

| 模式 | 枚举值 | 说明 |
|-----|-------|------|
| MODE_NONE | 0 | 无操作模式 |
| MODE_LINE_DDA | - | DDA直线绘制 |
| MODE_LINE_BRESENHAM | - | Bresenham直线绘制 |
| MODE_CIRCLE_MIDPOINT | - | 中点圆绘制 |
| MODE_CIRCLE_BRESENHAM | - | Bresenham圆绘制 |
| MODE_RECTANGLE | - | 矩形绘制 |
| MODE_POLYLINE | - | 折线绘制 |
| MODE_POLYGON | - | 多边形绘制 |
| MODE_FILL_SCANLINE | - | 扫描线填充 |
| MODE_FILL_BOUNDARY | - | 边界填充 |
| MODE_SELECT | - | 图形选择 |
| MODE_TRANSLATE | - | 平移变换 |
| MODE_SCALE | - | 缩放变换 |
| MODE_ROTATE | - | 旋转变换 |
| MODE_CLIP_COHEN_SUTHERLAND | - | Cohen-Sutherland裁剪 |
| MODE_CLIP_MIDPOINT | - | 中点分割裁剪 |
| MODE_CLIP_SUTHERLAND_HODGMAN | - | Sutherland-Hodgman裁剪 |
| MODE_CLIP_WEILER_ATHERTON | - | Weiler-Atherton裁剪 |
| MODE_3D_SPHERE | - | 3D球体绘制 |
| MODE_3D_CYLINDER | - | 3D圆柱体绘制 |
| MODE_3D_PLANE | - | 3D平面绘制 |
| MODE_3D_CUBE | - | 3D立方体绘制 |
| MODE_3D_SELECT | - | 3D图形选择 |
| MODE_3D_VIEW_CONTROL | - | 3D视角控制 |
