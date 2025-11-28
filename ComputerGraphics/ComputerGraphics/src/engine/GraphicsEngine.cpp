#include "GraphicsEngine.h"
#include "../algorithms/LineDrawer.h"
#include "../algorithms/CircleDrawer.h"
#include "../algorithms/FillAlgorithms.h"
#include <cmath>

GraphicsEngine::GraphicsEngine() 
    : hdc(nullptr), hwnd(nullptr), currentMode(MODE_NONE), isDrawing(false),
      selectedShapeIndex(-1), hasSelection(false), isTransforming(false),
      initialDistance(0.0), initialAngle(0.0), isDefiningClipWindow(false), 
      hasClipWindow(false) {}

GraphicsEngine::~GraphicsEngine() {}

void GraphicsEngine::Initialize(HWND hwnd, HDC hdc) {
    this->hwnd = hwnd;
    this->hdc = hdc;
}

void GraphicsEngine::SetMode(DrawMode mode) {
    currentMode = mode;
    isDrawing = false;
    tempPoints.clear();
}

void GraphicsEngine::ClearCanvas() {
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
    shapes.clear();
    hasSelection = false;
    selectedShapeIndex = -1;
}

void GraphicsEngine::RenderAll() {
    for (const auto& shape : shapes) {
        COLORREF color = shape.selected ? RGB(255, 0, 0) : shape.color;
        DrawShape(shape, color);
    }
}

void GraphicsEngine::DrawShape(const Shape& shape, COLORREF color) {
    switch (shape.type) {
        case SHAPE_LINE:
            if (shape.points.size() >= 2)
                LineDrawer::DrawBresenham(hdc, shape.points[0], shape.points[1], color);
            break;
        case SHAPE_CIRCLE:
            if (shape.points.size() >= 1)
                CircleDrawer::DrawBresenham(hdc, shape.points[0], shape.radius, color);
            break;
        case SHAPE_RECTANGLE:
            if (shape.points.size() >= 2) {
                Point2D p1 = shape.points[0], p2 = shape.points[1];
                LineDrawer::DrawBresenham(hdc, Point2D(p1.x, p1.y), Point2D(p2.x, p1.y), color);
                LineDrawer::DrawBresenham(hdc, Point2D(p2.x, p1.y), Point2D(p2.x, p2.y), color);
                LineDrawer::DrawBresenham(hdc, Point2D(p2.x, p2.y), Point2D(p1.x, p2.y), color);
                LineDrawer::DrawBresenham(hdc, Point2D(p1.x, p2.y), Point2D(p1.x, p1.y), color);
            }
            break;
        case SHAPE_POLYLINE:
        case SHAPE_POLYGON:
            for (size_t i = 1; i < shape.points.size(); i++)
                LineDrawer::DrawBresenham(hdc, shape.points[i-1], shape.points[i], color);
            if (shape.type == SHAPE_POLYGON && shape.points.size() >= 3)
                LineDrawer::DrawBresenham(hdc, shape.points.back(), shape.points.front(), color);
            break;
        case SHAPE_BSPLINE:
            // B样条实现略
            break;
    }
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
                if (currentMode == MODE_LINE_DDA)
                    LineDrawer::DrawDDA(hdc, tempPoints[0], tempPoints[1]);
                else
                    LineDrawer::DrawBresenham(hdc, tempPoints[0], tempPoints[1]);
                
                Shape line;
                line.type = SHAPE_LINE;
                line.points = tempPoints;
                shapes.push_back(line);
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
                if (currentMode == MODE_CIRCLE_MIDPOINT)
                    CircleDrawer::DrawMidpoint(hdc, tempPoints[0], radius);
                else
                    CircleDrawer::DrawBresenham(hdc, tempPoints[0], radius);
                
                Shape circle;
                circle.type = SHAPE_CIRCLE;
                circle.points.push_back(tempPoints[0]);
                circle.radius = radius;
                shapes.push_back(circle);
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
                Point2D p1 = tempPoints[0], p2 = tempPoints[1];
                LineDrawer::DrawBresenham(hdc, Point2D(p1.x, p1.y), Point2D(p2.x, p1.y));
                LineDrawer::DrawBresenham(hdc, Point2D(p2.x, p1.y), Point2D(p2.x, p2.y));
                LineDrawer::DrawBresenham(hdc, Point2D(p2.x, p2.y), Point2D(p1.x, p2.y));
                LineDrawer::DrawBresenham(hdc, Point2D(p1.x, p2.y), Point2D(p1.x, p1.y));
                
                Shape rectangle;
                rectangle.type = SHAPE_RECTANGLE;
                rectangle.points = tempPoints;
                shapes.push_back(rectangle);
                isDrawing = false;
            }
            break;
            
        case MODE_POLYLINE:
        case MODE_POLYGON:
            tempPoints.push_back(clickPoint);
            if (!isDrawing) {
                isDrawing = true;
            }
            if (tempPoints.size() >= 2) {
                LineDrawer::DrawBresenham(hdc, tempPoints[tempPoints.size()-2], tempPoints.back());
            }
            break;
            
        case MODE_FILL_BOUNDARY:
            FillAlgorithms::BoundaryFill(hdc, hwnd, x, y, RGB(255, 0, 0), RGB(0, 0, 0));
            break;
            
        case MODE_FILL_SCANLINE:
            tempPoints.push_back(clickPoint);
            if (!isDrawing) {
                isDrawing = true;
            }
            if (tempPoints.size() >= 2) {
                LineDrawer::DrawBresenham(hdc, tempPoints[tempPoints.size()-2], tempPoints.back());
            }
            break;
    }
}

void GraphicsEngine::OnMouseMove(int x, int y) {
    // 预览功能可以后续添加
}

void GraphicsEngine::OnRButtonDown(int x, int y) {
    if (currentMode == MODE_POLYLINE && tempPoints.size() >= 2) {
        Shape polyline;
        polyline.type = SHAPE_POLYLINE;
        polyline.points = tempPoints;
        shapes.push_back(polyline);
        tempPoints.clear();
        isDrawing = false;
    }
    else if (currentMode == MODE_POLYGON && tempPoints.size() >= 3) {
        Shape polygon;
        polygon.type = SHAPE_POLYGON;
        polygon.points = tempPoints;
        LineDrawer::DrawBresenham(hdc, tempPoints.back(), tempPoints.front());
        shapes.push_back(polygon);
        tempPoints.clear();
        isDrawing = false;
    }
    else if (currentMode == MODE_FILL_SCANLINE && tempPoints.size() >= 3) {
        LineDrawer::DrawBresenham(hdc, tempPoints.back(), tempPoints.front());
        FillAlgorithms::ScanlineFill(hdc, tempPoints, RGB(255, 0, 0));
        tempPoints.clear();
        isDrawing = false;
    }
}

void GraphicsEngine::DrawExpr1Graphics() {
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    int offsetX = 100, offsetY = 100;
    int scale = 5;

    // 外部圆角矩形 (66x46, R7)
    RoundRect(hdc, offsetX, offsetY, offsetX + 66 * scale, offsetY + 46 * scale, 7 * scale, 7 * scale);

    // 内部圆角矩形孔 (43x30, R3)
    RoundRect(hdc, offsetX + (66 - 43) / 2 * scale, offsetY + (46 - 30) / 2 * scale,
              offsetX + (66 + 43) / 2 * scale, offsetY + (46 + 30) / 2 * scale, 3 * scale, 3 * scale);

    // 四个圆孔 (直径7)
    int holeR = static_cast<int>(7.0 / 2.0 * scale);
    int holeCenterOffsetX = (66 - 52) / 2 * scale;
    int holeCenterOffsetY = (46 - 32) / 2 * scale;

    int centers[4][2] = {
        {offsetX + holeCenterOffsetX, offsetY + holeCenterOffsetY},
        {offsetX + 66 * scale - holeCenterOffsetX, offsetY + holeCenterOffsetY},
        {offsetX + holeCenterOffsetX, offsetY + 46 * scale - holeCenterOffsetY},
        {offsetX + 66 * scale - holeCenterOffsetX, offsetY + 46 * scale - holeCenterOffsetY}
    };

    for (int i = 0; i < 4; i++) {
        Ellipse(hdc, centers[i][0] - holeR, centers[i][1] - holeR,
                centers[i][0] + holeR, centers[i][1] + holeR);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

int GraphicsEngine::SelectShapeAt(int x, int y) {
    return -1; // 简化实现
}

void GraphicsEngine::DeselectAll() {
    for (auto& shape : shapes) shape.selected = false;
    hasSelection = false;
    selectedShapeIndex = -1;
}

Point2D GraphicsEngine::CalculateShapeCenter(const Shape& shape) {
    return Point2D(0, 0); // 简化实现
}

void GraphicsEngine::ApplyTranslation(Shape& shape, int dx, int dy) {
    for (auto& p : shape.points) {
        p.x += dx;
        p.y += dy;
    }
}

void GraphicsEngine::ApplyScaling(Shape& shape, double scale, Point2D center) {
    for (auto& p : shape.points) {
        int dx = p.x - center.x;
        int dy = p.y - center.y;
        p.x = center.x + (int)(dx * scale);
        p.y = center.y + (int)(dy * scale);
    }
    if (shape.type == SHAPE_CIRCLE) {
        shape.radius = (int)(shape.radius * scale);
    }
}

void GraphicsEngine::ApplyRotation(Shape& shape, double angle, Point2D center) {
    double cosA = cos(angle);
    double sinA = sin(angle);
    for (auto& p : shape.points) {
        int dx = p.x - center.x;
        int dy = p.y - center.y;
        p.x = center.x + (int)(dx * cosA - dy * sinA);
        p.y = center.y + (int)(dx * sinA + dy * cosA);
    }
}

void GraphicsEngine::ExecuteCohenSutherlandClipping() {}
void GraphicsEngine::ExecuteMidpointClipping() {}
void GraphicsEngine::ExecuteSutherlandHodgmanClipping() {}
void GraphicsEngine::ExecuteWeilerAthertonClipping() {}
void GraphicsEngine::DrawClipWindow(Point2D p1, Point2D p2, bool isDashed) {}
