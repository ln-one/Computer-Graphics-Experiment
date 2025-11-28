#include "CircleDrawer.h"

void CircleDrawer::SetPixel(HDC hdc, int x, int y, COLORREF color) {
    ::SetPixel(hdc, x, y, color);
}

void CircleDrawer::DrawCirclePoints(HDC hdc, Point2D center, int x, int y, COLORREF color) {
    SetPixel(hdc, center.x + x, center.y + y, color);
    SetPixel(hdc, center.x - x, center.y + y, color);
    SetPixel(hdc, center.x + x, center.y - y, color);
    SetPixel(hdc, center.x - x, center.y - y, color);
    SetPixel(hdc, center.x + y, center.y + x, color);
    SetPixel(hdc, center.x - y, center.y + x, color);
    SetPixel(hdc, center.x + y, center.y - x, color);
    SetPixel(hdc, center.x - y, center.y - x, color);
}

void CircleDrawer::DrawMidpoint(HDC hdc, Point2D center, int radius, COLORREF color) {
    int x = 0, y = radius;
    int d = 1 - radius;

    while (x <= y) {
        DrawCirclePoints(hdc, center, x, y, color);
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

void CircleDrawer::DrawBresenham(HDC hdc, Point2D center, int radius, COLORREF color) {
    int x = 0, y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        DrawCirclePoints(hdc, center, x, y, color);
        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}
