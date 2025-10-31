#include "GraphicsEngine.h"
#include <algorithm>

GraphicsEngine::GraphicsEngine() : hdc(nullptr), hwnd(nullptr),
                                   currentMode(MODE_NONE), isDrawing(false)
{
}

GraphicsEngine::~GraphicsEngine()
{
}

void GraphicsEngine::Initialize(HWND hwnd, HDC hdc)
{
    this->hwnd = hwnd;
    this->hdc = hdc;
}

void GraphicsEngine::SetMode(DrawMode mode)
{
    currentMode = mode;
    isDrawing = false;
    tempPoints.clear();
}

void GraphicsEngine::OnLButtonDown(int x, int y)
{
    Point2D clickPoint(x, y);

    switch (currentMode)
    {
    case MODE_LINE_DDA:
    case MODE_LINE_BRESENHAM:
        if (!isDrawing)
        {
            tempPoints.clear();
            tempPoints.push_back(clickPoint);
            isDrawing = true;
        }
        else
        {
            tempPoints.push_back(clickPoint);
            if (currentMode == MODE_LINE_DDA)
            {
                DrawLineDDA(tempPoints[0], tempPoints[1]);
            }
            else
            {
                DrawLineBresenham(tempPoints[0], tempPoints[1]);
            }
            isDrawing = false;
        }
        break;

    case MODE_CIRCLE_MIDPOINT:
    case MODE_CIRCLE_BRESENHAM:
        if (!isDrawing)
        {
            tempPoints.clear();
            tempPoints.push_back(clickPoint);
            isDrawing = true;
        }
        else
        {
            tempPoints.push_back(clickPoint);
            int radius = (int)sqrt(pow(tempPoints[1].x - tempPoints[0].x, 2) +
                                   pow(tempPoints[1].y - tempPoints[0].y, 2));
            if (currentMode == MODE_CIRCLE_MIDPOINT)
            {
                DrawCircleMidpoint(tempPoints[0], radius);
            }
            else
            {
                DrawCircleBresenham(tempPoints[0], radius);
            }
            isDrawing = false;
        }
        break;

    case MODE_RECTANGLE:
        if (!isDrawing)
        {
            tempPoints.clear();
            tempPoints.push_back(clickPoint);
            isDrawing = true;
        }
        else
        {
            tempPoints.push_back(clickPoint);
            DrawRectangle(tempPoints[0], tempPoints[1]);
            isDrawing = false;
        }
        break;

    case MODE_POLYLINE:
        tempPoints.push_back(clickPoint);
        if (!isDrawing)
        {
            isDrawing = true;
        }
        break;

    case MODE_BSPLINE:
        tempPoints.push_back(clickPoint);
        if (!isDrawing)
        {
            isDrawing = true;
        }
        if (tempPoints.size() >= 4)
        {
            DrawBSpline(tempPoints);
            tempPoints.clear();
            isDrawing = false;
        }
        break;

    case MODE_FILL_BOUNDARY:
        BoundaryFill(x, y, RGB(255, 0, 0), RGB(0, 0, 0));
        break;
        
    case MODE_FILL_SCANLINE:
        tempPoints.push_back(clickPoint);
        if (!isDrawing) {
            isDrawing = true;
        }
        // 需要至少3个点形成多边形
        if (tempPoints.size() >= 3) {
            // 可以实时显示当前多边形轮廓
            DrawPolyline(tempPoints, RGB(0, 0, 0));
        }
        break;
    }
}

void GraphicsEngine::OnRButtonDown(int x, int y)
{
    if (currentMode == MODE_POLYLINE && tempPoints.size() >= 2)
    {
        DrawPolyline(tempPoints);
        tempPoints.clear();
        isDrawing = false;
    }
    else if (currentMode == MODE_FILL_SCANLINE && tempPoints.size() >= 3)
    {
        // 闭合多边形并填充
        std::vector<Point2D> polygon = tempPoints;
        DrawPolyline(polygon, RGB(0, 0, 0)); // 绘制边界
        ScanlineFill(polygon, RGB(255, 0, 0)); // 填充
        tempPoints.clear();
        isDrawing = false;
    }
}

void GraphicsEngine::OnMouseMove(int x, int y)
{
    // Real-time preview functionality
}

void GraphicsEngine::DrawExpr1Graphics()
{
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    int offsetX = 100, offsetY = 100;
    int scale = 5;

    // Outer rounded rectangle (66x46, R7)
    RoundRect(hdc,
              offsetX, offsetY,
              offsetX + 66 * scale, offsetY + 46 * scale,
              7 * scale, 7 * scale);

    // Inner rounded rectangle hole (43x30, R3)
    RoundRect(hdc,
              offsetX + (66 - 43) / 2 * scale,
              offsetY + (46 - 30) / 2 * scale,
              offsetX + (66 + 43) / 2 * scale,
              offsetY + (46 + 30) / 2 * scale,
              3 * scale, 3 * scale);

    // Four circular holes (Phi 7)
    int holeR = static_cast<int>(7.0 / 2.0 * scale);
    int holeCenterOffsetX = (66 - 52) / 2 * scale;
    int holeCenterOffsetY = (46 - 32) / 2 * scale;

    int centers[4][2] = {
        {offsetX + holeCenterOffsetX, offsetY + holeCenterOffsetY},
        {offsetX + 66 * scale - holeCenterOffsetX, offsetY + holeCenterOffsetY},
        {offsetX + holeCenterOffsetX, offsetY + 46 * scale - holeCenterOffsetY},
        {offsetX + 66 * scale - holeCenterOffsetX, offsetY + 46 * scale - holeCenterOffsetY}};

    for (int i = 0; i < 4; i++)
    {
        Ellipse(hdc,
                centers[i][0] - holeR, centers[i][1] - holeR,
                centers[i][0] + holeR, centers[i][1] + holeR);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void GraphicsEngine::DrawLineDDA(Point2D p1, Point2D p2, COLORREF color)
{
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    if (steps == 0)
        return;

    float xIncrement = (float)dx / steps;
    float yIncrement = (float)dy / steps;

    float x = (float)p1.x;
    float y = (float)p1.y;

    for (int i = 0; i <= steps; i++)
    {
        SetPixel((int)(x + 0.5), (int)(y + 0.5), color);
        x += xIncrement;
        y += yIncrement;
    }
}

void GraphicsEngine::DrawLineBresenham(Point2D p1, Point2D p2, COLORREF color)
{
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);
    int sx = p1.x < p2.x ? 1 : -1;
    int sy = p1.y < p2.y ? 1 : -1;
    int err = dx - dy;

    int x = p1.x;
    int y = p1.y;

    while (true)
    {
        SetPixel(x, y, color);

        if (x == p2.x && y == p2.y)
            break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
}

void GraphicsEngine::DrawCircleMidpoint(Point2D center, int radius, COLORREF color)
{
    int x = 0;
    int y = radius;
    int d = 1 - radius;

    while (x <= y)
    {
        SetPixel(center.x + x, center.y + y, color);
        SetPixel(center.x - x, center.y + y, color);
        SetPixel(center.x + x, center.y - y, color);
        SetPixel(center.x - x, center.y - y, color);
        SetPixel(center.x + y, center.y + x, color);
        SetPixel(center.x - y, center.y + x, color);
        SetPixel(center.x + y, center.y - x, color);
        SetPixel(center.x - y, center.y - x, color);

        if (d < 0)
        {
            d += 2 * x + 3;
        }
        else
        {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

void GraphicsEngine::DrawCircleBresenham(Point2D center, int radius, COLORREF color)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y)
    {
        SetPixel(center.x + x, center.y + y, color);
        SetPixel(center.x - x, center.y + y, color);
        SetPixel(center.x + x, center.y - y, color);
        SetPixel(center.x - x, center.y - y, color);
        SetPixel(center.x + y, center.y + x, color);
        SetPixel(center.x - y, center.y + x, color);
        SetPixel(center.x + y, center.y - x, color);
        SetPixel(center.x - y, center.y - x, color);

        if (d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void GraphicsEngine::DrawRectangle(Point2D p1, Point2D p2, COLORREF color)
{
    DrawLineBresenham(Point2D(p1.x, p1.y), Point2D(p2.x, p1.y), color);
    DrawLineBresenham(Point2D(p2.x, p1.y), Point2D(p2.x, p2.y), color);
    DrawLineBresenham(Point2D(p2.x, p2.y), Point2D(p1.x, p2.y), color);
    DrawLineBresenham(Point2D(p1.x, p2.y), Point2D(p1.x, p1.y), color);
}

void GraphicsEngine::DrawPolyline(const std::vector<Point2D> &points, COLORREF color)
{
    for (int i = 1; i < static_cast<int>(points.size()); i++)
    {
        DrawLineBresenham(points[i - 1], points[i], color);
    }
}

void GraphicsEngine::DrawBSpline(const std::vector<Point2D> &controlPoints, COLORREF color)
{
    if (controlPoints.size() < 4)
        return;

    const int segments = 100;
    std::vector<Point2D> curvePoints;

    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        Point2D point = CalculateBSplinePoint(t, controlPoints);
        curvePoints.push_back(point);
    }

    DrawPolyline(curvePoints, color);
}

Point2D GraphicsEngine::CalculateBSplinePoint(float t, const std::vector<Point2D> &controlPoints)
{
    int n = static_cast<int>(controlPoints.size());
    if (n < 4)
        return Point2D(0, 0);

    float u = t * (n - 3);
    int k = (int)u;
    if (k >= n - 3)
        k = n - 4;
    u = u - k;

    float b0 = (1 - u) * (1 - u) * (1 - u) / 6;
    float b1 = (3 * u * u * u - 6 * u * u + 4) / 6;
    float b2 = (-3 * u * u * u + 3 * u * u + 3 * u + 1) / 6;
    float b3 = u * u * u / 6;

    Point2D result;
    result.x = (int)(b0 * controlPoints[k].x + b1 * controlPoints[k + 1].x +
                     b2 * controlPoints[k + 2].x + b3 * controlPoints[k + 3].x);
    result.y = (int)(b0 * controlPoints[k].y + b1 * controlPoints[k + 1].y +
                     b2 * controlPoints[k + 2].y + b3 * controlPoints[k + 3].y);

    return result;
}

void GraphicsEngine::BoundaryFill(int x, int y, COLORREF fillColor, COLORREF boundaryColor)
{
    // 使用栈避免递归栈溢出
    std::stack<Point2D> pixelStack;
    pixelStack.push(Point2D(x, y));
    
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    
    while (!pixelStack.empty()) {
        Point2D current = pixelStack.top();
        pixelStack.pop();
        
        // 边界检查
        if (current.x < 0 || current.x >= clientRect.right || 
            current.y < 0 || current.y >= clientRect.bottom) {
            continue;
        }
        
        COLORREF currentColor = GetPixel(current.x, current.y);
        
        if (currentColor != boundaryColor && currentColor != fillColor) {
            SetPixel(current.x, current.y, fillColor);
            
            // 添加四个方向的邻接点
            pixelStack.push(Point2D(current.x + 1, current.y));
            pixelStack.push(Point2D(current.x - 1, current.y));
            pixelStack.push(Point2D(current.x, current.y + 1));
            pixelStack.push(Point2D(current.x, current.y - 1));
        }
    }
}

void GraphicsEngine::SetPixel(int x, int y, COLORREF color)
{
    SetPixelV(hdc, x, y, color);
}

COLORREF GraphicsEngine::GetPixel(int x, int y)
{
    return ::GetPixel(hdc, x, y);
}

void GraphicsEngine::ScanlineFill(const std::vector<Point2D>& polygon, COLORREF fillColor)
{
    if (polygon.size() < 3) return;
    
    // 找到多边形的边界
    int minY = polygon[0].y, maxY = polygon[0].y;
    for (const auto& point : polygon) {
        minY = std::min(minY, point.y);
        maxY = std::max(maxY, point.y);
    }
    
    // 对每条扫描线
    for (int y = minY; y <= maxY; y++) {
        std::vector<int> intersections;
        
        // 找到与扫描线的交点
        for (size_t i = 0; i < polygon.size(); i++) {
            Point2D p1 = polygon[i];
            Point2D p2 = polygon[(i + 1) % polygon.size()];
            
            if ((p1.y <= y && p2.y > y) || (p2.y <= y && p1.y > y)) {
                // 计算交点的x坐标
                int x = p1.x + (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
                intersections.push_back(x);
            }
        }
        
        // 排序交点
        std::sort(intersections.begin(), intersections.end());
        
        // 填充交点之间的像素
        for (size_t i = 0; i < intersections.size(); i += 2) {
            if (i + 1 < intersections.size()) {
                for (int x = intersections[i]; x <= intersections[i + 1]; x++) {
                    SetPixel(x, y, fillColor);
                }
            }
        }
    }
}

void GraphicsEngine::ClearCanvas()
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
}