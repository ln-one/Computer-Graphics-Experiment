#pragma once
#include "../core/Point2D.h"
#include <windows.h>
#include <vector>

// 填充算法
class FillAlgorithms {
public:
    static void BoundaryFill(HDC hdc, HWND hwnd, int x, int y, COLORREF fillColor, COLORREF boundaryColor);
    static void ScanlineFill(HDC hdc, const std::vector<Point2D>& polygon, COLORREF fillColor);

private:
    struct EdgeTableEntry {
        int ymax;
        float x;
        float dx;
    };
};
