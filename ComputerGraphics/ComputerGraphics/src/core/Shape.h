#pragma once
#include "Point2D.h"
#include <windows.h>
#include <vector>

// 图形类型枚举
enum ShapeType {
    SHAPE_LINE,
    SHAPE_CIRCLE,
    SHAPE_RECTANGLE,
    SHAPE_POLYLINE,
    SHAPE_POLYGON,
    SHAPE_BSPLINE
};

// 图形结构
struct Shape {
    ShapeType type;
    std::vector<Point2D> points;
    COLORREF color;
    int radius;
    bool selected;

    Shape() : type(SHAPE_LINE), color(RGB(0, 0, 0)), radius(0), selected(false) {}
};
