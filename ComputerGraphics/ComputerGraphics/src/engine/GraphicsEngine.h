/**
 * @dir engine
 * @brief 图形引擎目录
 * 
 * 本目录包含图形渲染引擎的核心代码，负责图形的渲染和用户交互。
 * 
 * 目录内容：
 * 
 * 【2D图形引擎】
 * - GraphicsEngine.*    - 2D图形引擎，处理2D绑定和渲染
 * - ShapeRenderer.*     - 图形渲染器，负责具体图形的绘制
 * - ShapeSelector.*     - 图形选择器，处理图形的选中和高亮
 * 
 * 【3D图形引擎】
 * - GraphicsEngine3D.h          - 3D引擎头文件，类声明
 * - GraphicsEngine3D_Core.cpp   - 3D引擎核心：初始化、OpenGL上下文管理
 * - GraphicsEngine3D_Render.cpp - 3D引擎渲染：场景渲染、光照计算
 * - GraphicsEngine3D_Input.cpp  - 3D引擎输入：鼠标交互、视角控制
 * - OpenGLFunctions.h           - OpenGL函数指针声明
 * 
 * 架构说明：
 * - GraphicsEngine 负责2D图形，使用Windows GDI进行渲染
 * - GraphicsEngine3D 负责3D图形，使用OpenGL 3.3 Core Profile进行渲染
 * - 两个引擎共享相同的用户界面，通过DrawMode切换工作模式
 */

#pragma once
#include "../core/Point2D.h"
#include "../core/Shape.h"
#include "../core/DrawMode.h"
#include <windows.h>
#include <vector>

/**
 * @file GraphicsEngine.h
 * @brief 二维图形引擎类定义
 * @author 计算机图形学项目组
 */

/**
 * @class GraphicsEngine
 * @brief 二维图形引擎主类
 * 
 * 负责管理二维图形的绘制、交互和渲染，是整个2D图形系统的核心
 * 提供统一的接口来处理各种绘图算法、用户交互和图形变换
 */
class GraphicsEngine {
public:
    /**
     * @brief 构造函数
     */
    GraphicsEngine();
    
    /**
     * @brief 析构函数
     */
    ~GraphicsEngine();

    /**
     * @brief 初始化图形引擎
     * @param hwnd 窗口句柄
     * @param hdc 设备上下文句柄
     */
    void Initialize(HWND hwnd, HDC hdc);
    
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
     * @param x 鼠标x坐标
     * @param y 鼠标y坐标
     */
    void OnLButtonDown(int x, int y);
    
    /**
     * @brief 处理鼠标移动事件
     * @param x 鼠标x坐标
     * @param y 鼠标y坐标
     */
    void OnMouseMove(int x, int y);
    
    /**
     * @brief 处理鼠标右键按下事件
     * @param x 鼠标x坐标
     * @param y 鼠标y坐标
     */
    void OnRButtonDown(int x, int y);

    // === 渲染相关 ===
    /**
     * @brief 清空画布
     */
    void ClearCanvas();
    
    /**
     * @brief 渲染所有图形
     */
    void RenderAll();
    
    /**
     * @brief 绘制实验图形
     */
    void DrawExpr1Graphics();
    
    // === 基础绘图方法（算法类的包装接口）===
    /**
     * @brief 使用DDA算法绘制直线
     */
    void DrawLineDDA(Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));
    
    /**
     * @brief 使用Bresenham算法绘制直线
     */
    void DrawLineBresenham(Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));
    
    /**
     * @brief 使用中点算法绘制圆形
     */
    void DrawCircleMidpoint(Point2D center, int radius, COLORREF color = RGB(0, 0, 0));
    
    /**
     * @brief 使用Bresenham算法绘制圆形
     */
    void DrawCircleBresenham(Point2D center, int radius, COLORREF color = RGB(0, 0, 0));
    
    /**
     * @brief 绘制矩形
     */
    void DrawRectangle(Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));
    
    /**
     * @brief 绘制折线
     */
    void DrawPolyline(const std::vector<Point2D>& points, COLORREF color = RGB(0, 0, 0));
    
    /**
     * @brief 绘制多边形
     */
    void DrawPolygon(const std::vector<Point2D>& points, COLORREF color = RGB(0, 0, 0));

private:
    // === 核心组件 ===
    HDC hdc;                              ///< Windows设备上下文句柄
    HWND hwnd;                            ///< 窗口句柄
    DrawMode currentMode;                 ///< 当前绘图模式
    std::vector<Point2D> tempPoints;      ///< 临时点集合（用于多点绘图）
    bool isDrawing;                       ///< 是否正在绘图状态

    // === 图形管理 ===
    std::vector<Shape> shapes;            ///< 所有图形对象的集合
    int selectedShapeIndex;               ///< 当前选中图形的索引
    bool hasSelection;                    ///< 是否有图形被选中

    // === 几何变换状态 ===
    Point2D transformStartPoint;          ///< 变换操作的起始点
    Point2D transformAnchorPoint;         ///< 变换操作的锚点（中心点）
    bool isTransforming;                  ///< 是否正在进行变换操作
    double initialDistance;               ///< 缩放操作的初始距离
    double initialAngle;                  ///< 旋转操作的初始角度

    // === 裁剪操作状态 ===
    Point2D clipWindowStart;              ///< 裁剪窗口的起始点
    Point2D clipWindowEnd;                ///< 裁剪窗口的结束点
    bool isDefiningClipWindow;            ///< 是否正在定义裁剪窗口
    bool hasClipWindow;                   ///< 是否已定义裁剪窗口

    // === 私有辅助方法 - 绘图模式处理 ===
    /**
     * @brief 处理直线绘制模式的鼠标点击
     */
    void HandleLineDrawing(Point2D clickPoint);
    
    /**
     * @brief 处理圆形绘制模式的鼠标点击
     */
    void HandleCircleDrawing(Point2D clickPoint);
    
    /**
     * @brief 处理矩形绘制模式的鼠标点击
     */
    void HandleRectangleDrawing(Point2D clickPoint);
    
    /**
     * @brief 处理多边形/折线绘制模式的鼠标点击
     */
    void HandlePolyDrawing(Point2D clickPoint);
    
    /**
     * @brief 处理扫描线填充模式的鼠标点击
     */
    void HandleScanlineFillDrawing(Point2D clickPoint);
    
    /**
     * @brief 处理图形选择模式的鼠标点击
     */
    void HandleSelection(Point2D clickPoint);
    
    /**
     * @brief 处理平移变换模式的鼠标点击
     */
    void HandleTranslation(Point2D clickPoint);
    
    /**
     * @brief 处理缩放变换模式的鼠标点击
     */
    void HandleScaling(Point2D clickPoint);
    
    /**
     * @brief 处理旋转变换模式的鼠标点击
     */
    void HandleRotation(Point2D clickPoint);
    
    /**
     * @brief 处理裁剪窗口定义模式的鼠标点击
     */
    void HandleClippingWindow(Point2D clickPoint);
    
    // === 裁剪算法执行 ===
    /**
     * @brief 执行Cohen-Sutherland裁剪算法
     */
    void ExecuteCohenSutherlandClipping();
    
    /**
     * @brief 执行中点分割裁剪算法
     */
    void ExecuteMidpointClipping();
    
    /**
     * @brief 执行Sutherland-Hodgman裁剪算法
     */
    void ExecuteSutherlandHodgmanClipping();
    
    /**
     * @brief 执行Weiler-Atherton裁剪算法
     */
    void ExecuteWeilerAthertonClipping();
    
    /**
     * @brief 绘制裁剪窗口
     */
    void DrawClipWindow(Point2D p1, Point2D p2);
};
