#pragma once
#include "framework.h"
#include <vector>
#include <cmath>

// 绘图模式枚举
enum DrawMode {
    MODE_NONE = 0,
    MODE_EXPR1,             // 实验一：基本图形绘制
    MODE_LINE_DDA,          // DDA直线算法
    MODE_LINE_BRESENHAM,    // Bresenham直线算法
    MODE_CIRCLE_MIDPOINT,   // 中点圆算法
    MODE_CIRCLE_BRESENHAM,  // Bresenham圆算法
    MODE_RECTANGLE,         // 矩形
    MODE_POLYLINE,          // 多段线
    MODE_BSPLINE,          // B样条曲线
    MODE_FILL_SCANLINE,    // 扫描线填充
    MODE_FILL_BOUNDARY     // 边界填充
};

// 点结构
struct Point2D {
    int x, y;
    Point2D(int x = 0, int y = 0) : x(x), y(y) {}
};

// 图形绘制引擎类
class GraphicsEngine {
private:
    HDC hdc;
    HWND hwnd;
    DrawMode currentMode;
    std::vector<Point2D> tempPoints;
    bool isDrawing;
    
public:
    GraphicsEngine();
    ~GraphicsEngine();
    
    void Initialize(HWND hwnd, HDC hdc);
    void SetMode(DrawMode mode);
    DrawMode GetMode() const { return currentMode; }
    
    // 鼠标事件处理
    void OnLButtonDown(int x, int y);
    void OnMouseMove(int x, int y);
    void OnRButtonDown(int x, int y);  // 右键结束多段线等
    
    // 实验一：基本图形绘制
    void DrawExpr1Graphics();
    
    // 实验二：基本图形生成算法
    void DrawLineDDA(Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));
    void DrawLineBresenham(Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));
    void DrawCircleMidpoint(Point2D center, int radius, COLORREF color = RGB(0, 0, 0));
    void DrawCircleBresenham(Point2D center, int radius, COLORREF color = RGB(0, 0, 0));
    void DrawRectangle(Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));
    void DrawPolyline(const std::vector<Point2D>& points, COLORREF color = RGB(0, 0, 0));
    void DrawBSpline(const std::vector<Point2D>& controlPoints, COLORREF color = RGB(0, 0, 0));
    
    // 填充算法
    void ScanLineFill(const std::vector<Point2D>& polygon, COLORREF fillColor);
    void BoundaryFill(int x, int y, COLORREF fillColor, COLORREF boundaryColor);
    
    // 辅助函数
    void SetPixel(int x, int y, COLORREF color);
    COLORREF GetPixel(int x, int y);
    void ClearCanvas();
    
private:
    // B样条计算
    Point2D CalculateBSplinePoint(float t, const std::vector<Point2D>& controlPoints);
    float BasisFunction(int i, int k, float t, const std::vector<float>& knots);
    
    // 扫描线填充辅助函数
    struct EdgeTableEntry {
        int ymax;
        float x;
        float dx;
        EdgeTableEntry* next;
    };
    void BuildEdgeTable(const std::vector<Point2D>& polygon, std::vector<EdgeTableEntry*>& edgeTable);
};

// 菜单ID定义
#define ID_FILE_NEW         40001
#define ID_FILE_EXIT        40002

#define ID_EXPR_EXPR1       40101

#define ID_DRAW_LINE_DDA    40201
#define ID_DRAW_LINE_BRES   40202
#define ID_DRAW_CIRCLE_MID  40203
#define ID_DRAW_CIRCLE_BRES 40204
#define ID_DRAW_RECTANGLE   40205
#define ID_DRAW_POLYLINE    40206
#define ID_DRAW_BSPLINE     40207

#define ID_FILL_SCANLINE    40301
#define ID_FILL_BOUNDARY    40302

#define ID_HELP_ABOUT       40401