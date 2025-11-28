#pragma once
#include "../core/Point2D.h"
#include <windows.h>

// 直线绘制算法
class LineDrawer {
public:
    static void DrawDDA(HDC hdc, Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));
    static void DrawBresenham(HDC hdc, Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));

private:
    static void SetPixel(HDC hdc, int x, int y, COLORREF color);
};
