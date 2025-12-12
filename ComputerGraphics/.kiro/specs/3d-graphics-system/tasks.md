# Implementation Plan

## 阶段1：最小可运行的3D系统（可测试：能切换到3D模式并显示空白OpenGL窗口）

- [x] 1. 集成外部库并创建基础结构
  - 下载GLAD、GLM、stb_image到项目external目录
  - 更新项目配置，添加include路径
  - 创建 `Point3D.h` 和 `Shape3D.h` 基础数据结构
  - 扩展 `DrawMode.h` 添加3D模式枚举
  - 更新 `MenuIDs.h` 添加3D菜单ID
  - _Requirements: 0.1, 0.2_

- [x] 2. 创建最小3D引擎并集成到main.cpp
  - 创建 `GraphicsEngine3D.h` 和 `GraphicsEngine3D.cpp` 骨架
  - 实现 `Initialize()` 创建OpenGL上下文
  - 实现 `Render()` 清除屏幕为蓝色（测试OpenGL工作）
  - 实现 `Shutdown()` 清理资源
  - 在 `main.cpp` 添加全局变量 `g_engine3D` 和 `is3DMode`
  - 在 `WM_CREATE` 添加"Mode"菜单和初始化3D引擎
  - 在 `WM_PAINT` 根据模式调用对应引擎
  - 在 `WM_COMMAND` 处理模式切换
  - **测试点：启动程序，切换到3D模式，看到蓝色背景**
  - _Requirements: 0.2, 0.3, 0.4, 0.5_

## 阶段2：绘制第一个3D图形（可测试：能看到一个立方体）

- [x] 3. 实现着色器管理
  - 创建 `ShaderManager.h` 和 `ShaderManager.cpp`
  - 实现着色器编译、链接、错误检查
  - 提供嵌入的基础顶点和片段着色器代码
  - _Requirements: 5.8, 6.7_

- [x] 4. 实现立方体网格生成
  - 创建 `MeshGenerator.h` 和 `MeshGenerator.cpp`
  - 实现 `GenerateCube()` 生成立方体顶点和索引
  - 实现 `CreateBuffers()` 创建VAO/VBO/EBO
  - _Requirements: 1.4_

- [x] 5. 实现基础渲染和立方体绘制
  - 在 `GraphicsEngine3D` 中初始化着色器程序
  - 实现 `OnLButtonDown()` 在点击时创建立方体
  - 实现完整的 `Render()` 方法：设置MVP矩阵、渲染立方体
  - 初始化默认摄像机参数
  - 在 `main.cpp` 添加"3D Shapes"菜单和"Cube"选项
  - 在 `WM_COMMAND` 处理立方体绘制命令
  - 在 `WM_LBUTTONDOWN` 路由到3D引擎
  - **测试点：切换到3D模式，点击Cube菜单，点击屏幕，看到立方体**
  - _Requirements: 1.4, 1.5_

## 阶段3：添加其他基本图形（可测试：能看到球体、柱体、平面）

- [x] 6. 实现球体网格生成
  - 在 `MeshGenerator.cpp` 实现 `GenerateSphere()`
  - 在 `main.cpp` 添加"Sphere"菜单项
  - 在 `GraphicsEngine3D` 处理球体创建
  - **测试点：点击Sphere菜单，点击屏幕，看到球体**
  - _Requirements: 1.1_

- [ ] 7. 实现柱体网格生成
  - 在 `MeshGenerator.cpp` 实现 `GenerateCylinder()`
  - 在 `main.cpp` 添加"Cylinder"菜单项
  - 在 `GraphicsEngine3D` 处理柱体创建
  - **测试点：点击Cylinder菜单，点击屏幕，看到柱体**
  - _Requirements: 1.2_

- [ ] 8. 实现平面网格生成
  - 在 `MeshGenerator.cpp` 实现 `GeneratePlane()`
  - 在 `main.cpp` 添加"Plane"菜单项
  - 在 `GraphicsEngine3D` 处理平面创建
  - **测试点：点击Plane菜单，点击屏幕，看到平面**
  - _Requirements: 1.3_

## 阶段4：实现选择和移动（可测试：能选中和移动图形）

- [x] 9. 实现鼠标选择功能
  - 在 `GraphicsEngine3D` 实现简单的选择逻辑（点击最近的图形）
  - 实现选中状态的视觉反馈（改变颜色）
  - 在 `main.cpp` 添加"Select"菜单项和处理
  - **测试点：点击Select菜单，点击图形，看到颜色变化**
  - _Requirements: 2.1, 2.4_

- [x] 10. 实现鼠标拖动移动
  - 在 `GraphicsEngine3D` 实现 `OnMouseMove()` 和 `OnLButtonUp()`
  - 实现拖动时的XY平面移动
  - 在 `main.cpp` 添加 `WM_LBUTTONUP` 和 `WM_MOUSEMOVE` 路由
  - **测试点：选中图形后拖动鼠标，图形跟随移动**
  - _Requirements: 2.2, 2.5_

- [ ] 11. 实现鼠标滚轮Z轴移动
  - 在 `GraphicsEngine3D` 实现 `OnMouseWheel()`
  - 在 `main.cpp` 添加 `WM_MOUSEWHEEL` 消息处理
  - **测试点：选中图形后滚动滚轮，图形前后移动**
  - _Requirements: 2.3_

## 阶段5：实现变换对话框（可测试：能精确设置位置、旋转、缩放）

- [ ] 12. 创建变换对话框资源
  - 创建或修改 `.rc` 资源文件
  - 添加变换对话框模板，包含9个输入框（位置XYZ、旋转XYZ、缩放XYZ）
  - 定义对话框和控件ID
  - _Requirements: 3.1_

- [ ] 13. 实现变换对话框逻辑
  - 创建 `Dialogs3D.h` 和 `Dialogs3D.cpp`
  - 实现 `TransformDialog3D` 类和 `DialogProc()`
  - 实现输入验证和参数应用
  - 在 `GraphicsEngine3D` 实现 `OnLButtonDoubleClick()`
  - 在 `main.cpp` 添加 `WM_LBUTTONDBLCLK` 消息处理
  - **测试点：双击图形，弹出对话框，修改参数，图形变换**
  - _Requirements: 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8_

## 阶段6：实现视角控制（可测试：能旋转和缩放视角）

- [ ] 14. 实现视角控制模式
  - 在 `GraphicsEngine3D` 实现摄像机旋转逻辑
  - 在拖动时更新摄像机角度和位置
  - 在 `main.cpp` 添加"View Control"菜单项
  - **测试点：点击View Control，拖动鼠标，场景旋转**
  - _Requirements: 4.1, 4.2, 4.4_

- [ ] 15. 实现视角缩放
  - 在视角控制模式下处理滚轮事件
  - 调整摄像机距离
  - **测试点：在View Control模式下滚动滚轮，场景缩放**
  - _Requirements: 4.3, 4.4_

## 阶段7：实现光照系统（可测试：能看到光照效果和调整光照）

- [ ] 16. 添加光照着色器和默认光照
  - 更新着色器代码，添加Phong光照模型
  - 在 `GraphicsEngine3D` 初始化默认光源
  - 在渲染时传递光照参数到着色器
  - **测试点：重新编译运行，图形有明暗效果**
  - _Requirements: 5.8_

- [ ] 17. 创建光照设置对话框
  - 在 `.rc` 添加光照对话框模板
  - 在 `Dialogs3D.cpp` 实现 `LightingDialog` 类
  - 在 `main.cpp` 添加"Lighting"菜单项和处理
  - **测试点：点击Lighting菜单，修改光照参数，看到效果变化**
  - _Requirements: 5.1, 5.2, 5.3, 5.4, 5.5, 5.6, 5.7_

## 阶段8：实现材质系统（可测试：能修改材质参数）

- [ ] 18. 创建材质编辑对话框
  - 在 `.rc` 添加材质对话框模板
  - 在 `Dialogs3D.cpp` 实现 `MaterialDialog` 类
  - 在双击对话框中添加"Material"按钮
  - **测试点：双击图形，点击Material按钮，修改材质，看到效果**
  - _Requirements: 6.1, 6.2, 6.3, 6.4, 6.5, 6.6, 6.7, 6.8_

## 阶段9：实现纹理系统（可测试：能加载和显示纹理）

- [ ] 19. 实现纹理加载
  - 创建 `TextureLoader.h` 和 `TextureLoader.cpp`
  - 实现 `LoadTexture()` 使用stb_image
  - 实现 `DeleteTexture()`
  - _Requirements: 7.2, 7.5, 7.7_

- [ ] 20. 创建纹理设置对话框
  - 在 `.rc` 添加纹理对话框模板
  - 在 `Dialogs3D.cpp` 实现 `TextureDialog` 类
  - 实现文件选择对话框
  - 更新着色器支持纹理
  - 在双击对话框中添加"Texture"按钮
  - **测试点：双击图形，点击Texture按钮，选择图片，看到纹理**
  - _Requirements: 7.1, 7.2, 7.3, 7.4, 7.5, 7.6, 7.8_

