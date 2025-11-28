#include "LineDrawer.h"
#include <cmath>

void LineDrawer::SetPixel(HDC hdc, int x, int y, COLORREF color) {
    ::SetPixel(hdc, x, y, color);
}

void LineDrawer::DrawDDA(HDC hdc, Point2D p1, Point2D p2, COLORREF color) {
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    if (steps == 0) return;

    float xInc = (float)dx / steps;
    float yInc = (float)dy / steps;
    float x = (float)p1.x;
    float y = (float)p1.y;

    for (int i = 0; i <= steps; i++) {
        SetPixel(hdc, (int)(x + 0.5), (int)(y + 0.5), color);
        x += xInc;
        y += yInc;
    }
}

void LineDrawer::DrawBresenham(HDC hdc, Point2D p1, Point2D p2, COLORREF color) {
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);
    int sx = p1.x < p2.x ? 1 : -1;
    int sy = p1.y < p2.y ? 1 : -1;
    int err = dx - dy;
    int x = p1.x, y = p1.y;

    while (true) {
        SetPixel(hdc, x, y, color);
        if (x == p2.x && y == p2.y) break;

        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 < dx) { err += dx; y += sy; }
    }
}
