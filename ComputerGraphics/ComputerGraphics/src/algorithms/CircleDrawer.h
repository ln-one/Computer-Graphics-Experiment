#pragma once
#include "../core/Point2D.h"
#include <windows.h>

// 圆绘制算法
class CircleDrawer {
public:
    static void DrawMidpoint(HDC hdc, Point2D center, int radius, COLORREF color = RGB(0, 0, 0));
    static void DrawBresenham(HDC hdc, Point2D center, int radius, COLORREF color = RGB(0, 0, 0));

private:
    static void SetPixel(HDC hdc, int x, int y, COLORREF color);
    static void DrawCirclePoints(HDC hdc, Point2D center, int x, int y, COLORREF color);
};
