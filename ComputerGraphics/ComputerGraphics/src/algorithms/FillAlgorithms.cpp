#include "FillAlgorithms.h"
#include <stack>
#include <algorithm>

void FillAlgorithms::BoundaryFill(HDC hdc, HWND hwnd, int x, int y, COLORREF fillColor, COLORREF boundaryColor) {
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    
    COLORREF startColor = GetPixel(hdc, x, y);
    if (startColor == boundaryColor || startColor == fillColor) return;
    
    std::stack<Point2D> seedStack;
    seedStack.push(Point2D(x, y));
    
    HPEN hPen = CreatePen(PS_SOLID, 1, fillColor);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    
    int maxIterations = 100000, iterations = 0;
    
    while (!seedStack.empty() && iterations < maxIterations) {
        Point2D seed = seedStack.top();
        seedStack.pop();
        iterations++;
        
        if (seed.x < 0 || seed.x >= clientRect.right || 
            seed.y < 0 || seed.y >= clientRect.bottom) continue;
        
        COLORREF c = GetPixel(hdc, seed.x, seed.y);
        if (c == boundaryColor || c == fillColor) continue;
        
        int left = seed.x, right = seed.x;
        
        while (left > 0 && GetPixel(hdc, left - 1, seed.y) != boundaryColor && 
               GetPixel(hdc, left - 1, seed.y) != fillColor) left--;
        
        while (right < clientRect.right - 1 && GetPixel(hdc, right + 1, seed.y) != boundaryColor && 
               GetPixel(hdc, right + 1, seed.y) != fillColor) right++;
        
        MoveToEx(hdc, left, seed.y, NULL);
        LineTo(hdc, right + 1, seed.y);
        
        for (int i = left; i <= right; i++) {
            if (seed.y > 0) {
                c = GetPixel(hdc, i, seed.y - 1);
                if (c != boundaryColor && c != fillColor) seedStack.push(Point2D(i, seed.y - 1));
            }
            if (seed.y < clientRect.bottom - 1) {
                c = GetPixel(hdc, i, seed.y + 1);
                if (c != boundaryColor && c != fillColor) seedStack.push(Point2D(i, seed.y + 1));
            }
        }
    }
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void FillAlgorithms::ScanlineFill(HDC hdc, const std::vector<Point2D>& polygon, COLORREF fillColor) {
    if (polygon.size() < 3) return;
    
    int ymin = polygon[0].y, ymax = polygon[0].y;
    for (const auto& p : polygon) {
        if (p.y < ymin) ymin = p.y;
        if (p.y > ymax) ymax = p.y;
    }
    
    for (int y = ymin; y <= ymax; y++) {
        std::vector<int> intersections;
        
        for (size_t i = 0; i < polygon.size(); i++) {
            Point2D p1 = polygon[i];
            Point2D p2 = polygon[(i + 1) % polygon.size()];
            
            if ((p1.y <= y && p2.y > y) || (p2.y <= y && p1.y > y)) {
                float x = p1.x + (float)(y - p1.y) / (p2.y - p1.y) * (p2.x - p1.x);
                intersections.push_back((int)(x + 0.5));
            }
        }
        
        std::sort(intersections.begin(), intersections.end());
        
        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            for (int x = intersections[i]; x <= intersections[i + 1]; x++) {
                SetPixel(hdc, x, y, fillColor);
            }
        }
    }
}
