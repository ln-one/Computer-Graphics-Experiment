#pragma once
#include "../core/Point2D.h"
#include "../core/Shape.h"
#include "../core/DrawMode.h"
#include <windows.h>
#include <vector>

// 图形引擎主类
class GraphicsEngine {
public:
    GraphicsEngine();
    ~GraphicsEngine();

    void Initialize(HWND hwnd, HDC hdc);
    void SetMode(DrawMode mode);
    DrawMode GetMode() const { return currentMode; }

    // 鼠标事件
    void OnLButtonDown(int x, int y);
    void OnMouseMove(int x, int y);
    void OnRButtonDown(int x, int y);

    // 渲染
    void ClearCanvas();
    void RenderAll();
    void DrawExpr1Graphics();

private:
    HDC hdc;
    HWND hwnd;
    DrawMode currentMode;
    std::vector<Point2D> tempPoints;
    bool isDrawing;

    // 图形管理
    std::vector<Shape> shapes;
    int selectedShapeIndex;
    bool hasSelection;

    // 变换状态
    Point2D transformStartPoint;
    Point2D transformAnchorPoint;
    bool isTransforming;
    double initialDistance;
    double initialAngle;

    // 裁剪状态
    Point2D clipWindowStart;
    Point2D clipWindowEnd;
    bool isDefiningClipWindow;
    bool hasClipWindow;

    // 私有辅助方法 - 处理不同绘图模式
    void HandleLineDrawing(Point2D clickPoint);
    void HandleCircleDrawing(Point2D clickPoint);
    void HandleRectangleDrawing(Point2D clickPoint);
    void HandlePolyDrawing(Point2D clickPoint);
    void HandleScanlineFillDrawing(Point2D clickPoint);
    void HandleSelection(Point2D clickPoint);
    void HandleTranslation(Point2D clickPoint);
    void HandleScaling(Point2D clickPoint);
    void HandleRotation(Point2D clickPoint);
    void HandleClippingWindow(Point2D clickPoint);
    
    // 裁剪算法
    void ExecuteCohenSutherlandClipping();
    void ExecuteMidpointClipping();
    void ExecuteSutherlandHodgmanClipping();
    void DrawClipWindow(Point2D p1, Point2D p2);
};
