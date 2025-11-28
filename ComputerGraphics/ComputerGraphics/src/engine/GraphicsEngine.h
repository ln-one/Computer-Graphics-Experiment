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

    // 辅助方法
    void DrawShape(const Shape& shape, COLORREF color);
    int SelectShapeAt(int x, int y);
    void DeselectAll();
    Point2D CalculateShapeCenter(const Shape& shape);
    void ApplyTranslation(Shape& shape, int dx, int dy);
    void ApplyScaling(Shape& shape, double scale, Point2D center);
    void ApplyRotation(Shape& shape, double angle, Point2D center);
    
    // 裁剪算法
    void ExecuteCohenSutherlandClipping();
    void ExecuteMidpointClipping();
    void ExecuteSutherlandHodgmanClipping();
    void ExecuteWeilerAthertonClipping();
    void DrawClipWindow(Point2D p1, Point2D p2, bool isDashed);
};
