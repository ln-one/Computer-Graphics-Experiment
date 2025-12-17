#pragma once
#include "../core/Shape3D.h"
#include "../core/DrawMode.h"
#include <windows.h>
#include <vector>

/**
 * @file GraphicsEngine3D.h
 * @brief 三维图形引擎类定义
 * @author ln1.opensource@gmail.com
 */

/**
 * @struct Camera
 * @brief 摄像机参数结构
 * 
 * 定义了3D场景中摄像机的位置、朝向和目标点
 * 使用球坐标系统来简化摄像机控制
 */
struct Camera {
    float distance;    ///< 摄像机到目标点的距离
    float angleX;      ///< 水平旋转角度（绕Y轴）
    float angleY;      ///< 垂直旋转角度（绕X轴）
    float targetX;     ///< 目标点X坐标
    float targetY;     ///< 目标点Y坐标
    float targetZ;     ///< 目标点Z坐标
    
    /**
     * @brief 默认构造函数
     * 初始化摄像机位置和朝向
     */
    Camera() : distance(5.0f), angleX(0.0f), angleY(0.0f),
               targetX(0.0f), targetY(0.0f), targetZ(0.0f) {}
};

/**
 * @struct Light
 * @brief 光源参数结构
 * 
 * 定义了3D场景中光源的位置、颜色和强度参数
 * 支持Phong光照模型的环境光、漫反射和镜面反射
 */
struct Light {
    float positionX;           ///< 光源X坐标
    float positionY;           ///< 光源Y坐标
    float positionZ;           ///< 光源Z坐标
    float color[3];            ///< 光源颜色（RGB）
    float ambientIntensity;    ///< 环境光强度
    float diffuseIntensity;    ///< 漫反射光强度
    float specularIntensity;   ///< 镜面反射光强度
    
    /**
     * @brief 默认构造函数
     * 初始化光源位置和光照参数
     */
    Light() : positionX(5.0f), positionY(5.0f), positionZ(5.0f),
              ambientIntensity(0.2f), diffuseIntensity(0.8f), specularIntensity(1.0f) {
        color[0] = color[1] = color[2] = 1.0f;  // 白光
    }
};

/**
 * @class GraphicsEngine3D
 * @brief 三维图形引擎主类
 * 
 * 负责管理三维图形的绘制、交互和渲染，基于OpenGL实现
 * 提供与2D图形引擎类似的接口结构，便于统一管理
 */
class GraphicsEngine3D {
public:
    /**
     * @brief 构造函数
     */
    GraphicsEngine3D();
    
    /**
     * @brief 析构函数
     */
    ~GraphicsEngine3D();
    
    /**
     * @brief 初始化3D图形引擎
     * @param hwnd 窗口句柄
     * @return 初始化成功返回true，失败返回false
     * 
     * 创建OpenGL上下文，初始化OpenGL状态和着色器程序
     */
    bool Initialize(HWND hwnd);
    
    /**
     * @brief 关闭3D图形引擎
     * 
     * 清理OpenGL资源和上下文
     */
    void Shutdown();
    
    /**
     * @brief 设置当前绘图模式
     * @param mode 绘图模式
     */
    void SetMode(DrawMode mode);
    
    /**
     * @brief 获取当前绘图模式
     * @return 当前的绘图模式
     */
    DrawMode GetMode() const { return currentMode; }
    
    // === 鼠标事件处理 ===
    /**
     * @brief 处理鼠标左键按下事件
     */
    void OnLButtonDown(int x, int y);
    
    /**
     * @brief 处理鼠标左键释放事件
     */
    void OnLButtonUp(int x, int y);
    
    /**
     * @brief 处理鼠标右键按下事件
     */
    void OnRButtonDown(int x, int y);
    
    /**
     * @brief 处理鼠标右键释放事件
     */
    void OnRButtonUp(int x, int y);
    
    /**
     * @brief 处理鼠标左键双击事件
     */
    void OnLButtonDoubleClick(int x, int y);
    
    /**
     * @brief 处理鼠标移动事件
     */
    void OnMouseMove(int x, int y);
    
    /**
     * @brief 处理鼠标滚轮事件
     * @param delta 滚轮滚动量
     */
    void OnMouseWheel(int delta);
    
    // === 渲染相关 ===
    /**
     * @brief 渲染3D场景
     * 
     * 执行完整的3D渲染流程，包括变换、光照和着色
     */
    void Render();
    
    /**
     * @brief 清空3D场景
     */
    void ClearScene();
    
    // === 光照设置 ===
    /**
     * @brief 获取光源对象引用
     * @return 光源对象的引用
     */
    Light& GetLight() { return light; }
    
    /**
     * @brief 更新光照参数到着色器
     */
    void UpdateLight();
    
    /**
     * @brief 释放OpenGL上下文
     */
    void ReleaseContext();
    
    /**
     * @brief 使用固定管线渲染（备用方案）
     */
    void RenderWithFixedPipeline();
    
    /**
     * @brief 使用立即模式渲染立方体
     */
    void RenderCubeImmediate(float size);
    
    /**
     * @brief 使用立即模式渲染球体
     */
    void RenderSphereImmediate(float radius, int segments, int rings);
    
    /**
     * @brief 使用立即模式渲染圆柱体
     */
    void RenderCylinderImmediate(float radius, float height, int segments);
    
    /**
     * @brief 使用立即模式渲染平面
     */
    void RenderPlaneImmediate(float width, float height);
    
private:
    // === 核心组件 ===
    HWND hwnd;                            ///< 窗口句柄
    HDC hdc;                              ///< 设备上下文句柄
    HGLRC hglrc;                          ///< OpenGL渲染上下文
    
    // === 模式和状态管理 ===
    DrawMode currentMode;                 ///< 当前绘图模式
    std::vector<Shape3D> shapes;          ///< 3D图形对象集合
    int selectedShapeIndex;               ///< 当前选中图形的索引
    bool hasSelection;                    ///< 是否有图形被选中
    
    // === 摄像机和光照 ===
    Camera camera;                        ///< 摄像机对象
    Light light;                          ///< 光源对象
    
    // === 鼠标交互状态 ===
    int lastMouseX, lastMouseY;           ///< 上次鼠标位置
    bool isDragging;                      ///< 是否正在拖拽
    bool isRightDragging;                 ///< 是否正在右键拖拽
    
    // === OpenGL资源 ===
    unsigned int shaderProgram;           ///< 着色器程序ID
    bool isInitialized;                   ///< OpenGL初始化标志
    
    // === 私有辅助方法 ===
    /**
     * @brief 处理3D图形创建
     * @param x 鼠标x坐标
     * @param y 鼠标y坐标
     */
    void HandleShapeCreation(int x, int y);
    
    /**
     * @brief 处理3D图形选择
     * @param x 鼠标x坐标
     * @param y 鼠标y坐标
     */
    void HandleSelection(int x, int y);
    
    /**
     * @brief 处理视角控制
     * @param deltaX 鼠标X方向移动量
     * @param deltaY 鼠标Y方向移动量
     */
    void HandleViewControl(int deltaX, int deltaY);
    
    /**
     * @brief 处理物体拖拽移动
     * @param deltaX 鼠标X方向移动量
     * @param deltaY 鼠标Y方向移动量
     */
    void HandleObjectDragging(int deltaX, int deltaY);
    
    // === OpenGL初始化 ===
    /**
     * @brief 创建OpenGL上下文
     * @return 创建成功返回true
     */
    bool CreateOpenGLContext();
    
    /**
     * @brief 加载OpenGL函数
     * @return 加载成功返回true
     */
    bool LoadOpenGLFunctions();
};
