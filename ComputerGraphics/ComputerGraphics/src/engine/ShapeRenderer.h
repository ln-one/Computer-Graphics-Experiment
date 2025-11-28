#pragma once
#include "../core/Shape.h"
#include <windows.h>

// 图形渲染器 - 负责绘制图形
class ShapeRenderer {
public:
    static void DrawShape(HDC hdc, const Shape& shape, COLORREF color);
};
