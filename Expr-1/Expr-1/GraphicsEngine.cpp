#include "GraphicsEngine.h"
#include <algorithm>

GraphicsEngine::GraphicsEngine() : hdc(nullptr), hwnd(nullptr), 
    currentMode(MODE_NONE), isDrawing(false) {
}

GraphicsEngine::~GraphicsEngine() {
}

void GraphicsEngine::Initialize(HWND hwnd, HDC hdc) {
    this->hwnd = hwnd;
    this->hdc = hdc;
}

void GraphicsEngine::SetMode(DrawMode mode) {
    currentMode = mode;
    isDrawing = false;
    tempPoints.clear();
}

void GraphicsEngine::OnLButtonDown(int x, int y) {
    Point2D clickPoint(x, y);
    
    switch (currentMode) {
    case MODE_LINE_DDA:
    case MODE_LINE_BRESENHAM:
        if (!isDrawing) {
            tempPoints.clear();
            tempPoints.push_back(clickPoint);
            isDrawing = true;
        } else {
            tempPoints.push_back(clickPoint);
            if (currentMode == MODE_LINE_DDA) {
                DrawLineDDA(tempPoints[0], tempPoints[1]);
            } else {
                DrawLineBresenham(tempPoints[0], tempPoints[1]);
            }
            isDrawing = false;
        }
        break;
        
    case MODE_CIRCLE_MIDPOINT:
    case MODE_CIRCLE_BRESENHAM:
        if (!isDrawing) {
            tempPoints.clear();
            tempPoints.push_back(clickPoint);
            isDrawing = true;
        } else {
            tempPoints.push_back(clickPoint);
            int radius = (int)sqrt(pow(tempPoints[1].x - tempPoints[0].x, 2) + 
                                 pow(tempPoints[1].y - tempPoints[0].y, 2));
            if (currentMode == MODE_CIRCLE_MIDPOINT) {
                DrawCircleMidpoint(tempPoints[0], radius);
            } else {
                DrawCircleBresenham(tempPoints[0], radius);
            }
            isDrawing = false;
        }
        break;
        
    case MODE_RECTANGLE:
        if (!isDrawing) {
            tempPoints.clear();
            tempPoints.push_back(clickPoint);
            isDrawing = true;
        } else {
            tempPoints.push_back(clickPoint);
            DrawRectangle(tempPoints[0], tempPoints[1]);
            isDrawing = false;
        }
        break;
        
    case MODE_POLYLINE:
        tempPoints.push_back(clickPoint);
        if (!isDrawing) {
            isDrawing = true;
        }
        break;
        
    case MODE_BSPLINE:
        tempPoints.push_back(clickPoint);
        if (!isDrawing) {
            isDrawing = true;
        }
        if (tempPoints.size() >= 4) {
            DrawBSpline(tempPoints);
            tempPoints.clear();
            isDrawing = false;
        }
        break;
        
    case MODE_FILL_BOUNDARY:
        BoundaryFill(x, y, RGB(255, 0, 0), RGB(0, 0, 0));
        break;
    }
}

void GraphicsEngine::OnRButtonDown(int x, int y) {
    if (currentMode == MODE_POLYLINE && tempPoints.size() >= 2) {
        DrawPolyline(tempPoints);
        tempPoints.clear();
        isDrawing = false;
    }
}

void GraphicsEngine::OnMouseMove(int x, int y) {
    // 可以在这里添加实时预览功能
}

void GraphicsEngine::DrawExpr1Graphics() {
    // 实验一的原始绘图代码
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    
    int offsetX = 100, offsetY = 100;
    int scale = 5;
    
    // 外圆角矩形 (66×46, R7)
    RoundRect(hdc,
        offsetX, offsetY,
        offsetX + 66 * scale, offsetY + 46 * scale,
        7 * scale, 7 * scale);
    
    // 内圆角矩形孔 (43×30, R3)
    RoundRect(hdc,
        offsetX + (66 - 43) / 2 * scale,
        offsetY + (46 - 30) / 2 * scale,
        offsetX + (66 + 43) / 2 * scale,
        offsetY + (46 + 30) / 2 * scale,
        3 * scale, 3 * scale);
    
    // 四个圆孔 (Φ7)
    int holeR = 7 / 2.0 * scale;
    int holeCenterOffsetX = (66 - 52) / 2 * scale;
    int holeCenterOffsetY = (46 - 32) / 2 * scale;

    int centers[4][2] = {
        {offsetX + holeCenterOffsetX, offsetY + holeCenterOffsetY},
        {offsetX + 66 * scale - holeCenterOffsetX, offsetY + holeCenterOffsetY},
        {offsetX + holeCenterOffsetX, offsetY + 46 * scale - holeCenterOffsetY},
        {offsetX + 66 * scale - holeCenterOffsetX, offsetY + 46 * scale - holeCenterOffsetY}
    };

    for (int i = 0; i < 4; i++) {
        Ellipse(hdc,
            centers[i][0] - holeR, centers[i][1] - holeR,
            centers[i][0] + holeR, centers[i][1] + holeR);
    }
    
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void GraphicsEngine::DrawLineDDA(Point2D p1, Point2D p2, COLORREF color) {
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    
    float xIncrement = (float)dx / steps;
    float yIncrement = (float)dy / steps;
    
    float x = (float)p1.x;
    float y = (float)p1.y;
    
    for (int i = 0; i <= steps; i++) {
        SetPixel((int)(x + 0.5), (int)(y + 0.5), color);
        x += xIncrement;
        y += yIncrement;
    }
}

void GraphicsEngine::DrawLineBresenham(Point2D p1, Point2D p2, COLORREF color) {
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);
    int sx = p1.x < p2.x ? 1 : -1;
    int sy = p1.y < p2.y ? 1 : -1;
    int err = dx - dy;
    
    int x = p1.x;
    int y = p1.y;
    
    while (true) {
        SetPixel(x, y, color);
        
        if (x == p2.x && y == p2.y) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void GraphicsEngine::DrawCircleMidpoint(Point2D center, int radius, COLORREF color) {
    int x = 0;
    int y = radius;
    int d = 1 - radius;
    
    while (x <= y) {
        // 绘制8个对称点
        SetPixel(center.x + x, center.y + y, color);
        SetPixel(center.x - x, center.y + y, color);
        SetPixel(center.x + x, center.y - y, color);
        SetPixel(center.x - x, center.y - y, color);
        SetPixel(center.x + y, center.y + x, color);
        SetPixel(center.x - y, center.y + x, color);
        SetPixel(center.x + y, center.y - x, color);
        SetPixel(center.x - y, center.y - x, color);
        
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

void GraphicsEngine::DrawCircleBresenham(Point2D center, int radius, COLORREF color) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    while (x <= y) {
        // 绘制8个对称点
        SetPixel(center.x + x, center.y + y, color);
        SetPixel(center.x - x, center.y + y, color);
        SetPixel(center.x + x, center.y - y, color);
        SetPixel(center.x - x, center.y - y, color);
        SetPixel(center.x + y, center.y + x, color);
        SetPixel(center.x - y, center.y + x, color);
        SetPixel(center.x + y, center.y - x, color);
        SetPixel(center.x - y, center.y - x, color);
        
        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void GraphicsEngine::DrawRectangle(Point2D p1, Point2D p2, COLORREF color) {
    DrawLineBresenham(Point2D(p1.x, p1.y), Point2D(p2.x, p1.y), color);
    DrawLineBresenham(Point2D(p2.x, p1.y), Point2D(p2.x, p2.y), color);
    DrawLineBresenham(Point2D(p2.x, p2.y), Point2D(p1.x, p2.y), color);
    DrawLineBresenham(Point2D(p1.x, p2.y), Point2D(p1.x, p1.y), color);
}

void GraphicsEngine::DrawPolyline(const std::vector<Point2D>& points, COLORREF color) {
    for (size_t i = 1; i < points.size(); i++) {
        DrawLineBresenham(points[i-1], points[i], color);
    }
}

void GraphicsEngine::DrawBSpline(const std::vector<Point2D>& controlPoints, COLORREF color) {
    if (controlPoints.size() < 4) return;
    
    const int segments = 100;
    std::vector<Point2D> curvePoints;
    
    for (int i = 0; i <= segments; i++) {
        float t = (float)i / segments;
        Point2D point = CalculateBSplinePoint(t, controlPoints);
        curvePoints.push_back(point);
    }
    
    DrawPolyline(curvePoints, color);
}

Point2D GraphicsEngine::CalculateBSplinePoint(float t, const std::vector<Point2D>& controlPoints) {
    // 简化的三次B样条实现
    int n = controlPoints.size();
    if (n < 4) return Point2D(0, 0);
    
    // 使用均匀节点向量
    float u = t * (n - 3);
    int k = (int)u;
    if (k >= n - 3) k = n - 4;
    u = u - k;
    
    // 三次B样条基函数
    float b0 = (1 - u) * (1 - u) * (1 - u) / 6;
    float b1 = (3 * u * u * u - 6 * u * u + 4) / 6;
    float b2 = (-3 * u * u * u + 3 * u * u + 3 * u + 1) / 6;
    float b3 = u * u * u / 6;
    
    Point2D result;
    result.x = (int)(b0 * controlPoints[k].x + b1 * controlPoints[k+1].x + 
                     b2 * controlPoints[k+2].x + b3 * controlPoints[k+3].x);
    result.y = (int)(b0 * controlPoints[k].y + b1 * controlPoints[k+1].y + 
                     b2 * controlPoints[k+2].y + b3 * controlPoints[k+3].y);
    
    return result;
}

void GraphicsEngine::BoundaryFill(int x, int y, COLORREF fillColor, COLORREF boundaryColor) {
    COLORREF currentColor = GetPixel(x, y);
    
    if (currentColor != boundaryColor && currentColor != fillColor) {
        SetPixel(x, y, fillColor);
        
        // 递归填充四个方向
        BoundaryFill(x + 1, y, fillColor, boundaryColor);
        BoundaryFill(x - 1, y, fillColor, boundaryColor);
        BoundaryFill(x, y + 1, fillColor, boundaryColor);
        BoundaryFill(x, y - 1, fillColor, boundaryColor);
    }
}

void GraphicsEngine::SetPixel(int x, int y, COLORREF color) {
    SetPixelV(hdc, x, y, color);
}

COLORREF GraphicsEngine::GetPixel(int x, int y) {
    return ::GetPixel(hdc, x, y);
}

void GraphicsEngine::ClearCanvas() {
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
}