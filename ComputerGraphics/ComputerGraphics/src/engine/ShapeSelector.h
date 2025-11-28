#pragma once
#include "../core/Point2D.h"
#include "../core/Shape.h"
#include <vector>

// 图形选择器 - 负责选择和高亮显示
class ShapeSelector {
public:
    static int SelectShapeAt(Point2D clickPoint, const std::vector<Shape>& shapes);
    static void DrawSelectionIndicator(HDC hdc, const Shape& shape);

private:
    static bool HitTestLine(Point2D point, Point2D p1, Point2D p2, int tolerance = 5);
    static bool HitTestCircle(Point2D point, Point2D center, int radius, int tolerance = 5);
    static bool HitTestPolygon(Point2D point, const std::vector<Point2D>& polygon);
    static double PointToLineDistance(Point2D point, Point2D lineStart, Point2D lineEnd);
};
