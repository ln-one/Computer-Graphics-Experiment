#include "GraphicsEngine.h"
#include "ShapeRenderer.h"
#include "ShapeSelector.h"
#include "../algorithms/LineDrawer.h"
#include "../algorithms/CircleDrawer.h"
#include "../algorithms/FillAlgorithms.h"
#include "../algorithms/TransformAlgorithms.h"
#include "../algorithms/ClippingAlgorithms.h"
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
        ShapeRenderer::DrawShape(hdc, shape, color);
        if (shape.selected) {
            ShapeSelector::DrawSelectionIndicator(hdc, shape);
        }
    }
}

void GraphicsEngine::OnLButtonDown(int x, int y) {
    Point2D clickPoint(x, y);
    
    switch (currentMode) {
        case MODE_LINE_DDA:
        case MODE_LINE_BRESENHAM:
            HandleLineDrawing(clickPoint);
            break;
        case MODE_CIRCLE_MIDPOINT:
        case MODE_CIRCLE_BRESENHAM:
            HandleCircleDrawing(clickPoint);
            break;
        case MODE_RECTANGLE:
            HandleRectangleDrawing(clickPoint);
            break;
        case MODE_POLYLINE:
        case MODE_POLYGON:
            HandlePolyDrawing(clickPoint);
            break;
        case MODE_FILL_BOUNDARY:
            FillAlgorithms::BoundaryFill(hdc, hwnd, x, y, RGB(255, 0, 0), RGB(0, 0, 0));
            break;
        case MODE_FILL_SCANLINE:
            HandleScanlineFillDrawing(clickPoint);
            break;
        case MODE_SELECT:
            HandleSelection(clickPoint);
            break;
        case MODE_TRANSLATE:
            HandleTranslation(clickPoint);
            break;
        case MODE_SCALE:
            HandleScaling(clickPoint);
            break;
        case MODE_ROTATE:
            HandleRotation(clickPoint);
            break;
        case MODE_CLIP_COHEN_SUTHERLAND:
        case MODE_CLIP_MIDPOINT:
        case MODE_CLIP_SUTHERLAND_HODGMAN:
        case MODE_CLIP_WEILER_ATHERTON:
            HandleClippingWindow(clickPoint);
            break;
    }
}

void GraphicsEngine::OnMouseMove(int x, int y) {
    Point2D currentPoint(x, y);
    
    // Handle rotation preview
    if (currentMode == MODE_ROTATE && isTransforming && hasSelection) {
        // Redraw everything
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
        RenderAll();
        
        // Draw rotation center marker
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        int markerSize = 5;
        MoveToEx(hdc, transformAnchorPoint.x - markerSize, transformAnchorPoint.y, NULL);
        LineTo(hdc, transformAnchorPoint.x + markerSize, transformAnchorPoint.y);
        MoveToEx(hdc, transformAnchorPoint.x, transformAnchorPoint.y - markerSize, NULL);
        LineTo(hdc, transformAnchorPoint.x, transformAnchorPoint.y + markerSize);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
        
        // Calculate rotation angle
        int dx = currentPoint.x - transformAnchorPoint.x;
        int dy = currentPoint.y - transformAnchorPoint.y;
        double currentAngle = atan2(dy, dx);
        
        // On first mouse move, record initial angle
        static bool firstMove = true;
        static Point2D lastAnchor;
        if (firstMove || lastAnchor.x != transformAnchorPoint.x || lastAnchor.y != transformAnchorPoint.y) {
            initialAngle = currentAngle;
            firstMove = false;
            lastAnchor = transformAnchorPoint;
        }
        
        double rotationAngle = currentAngle - initialAngle;
        
        // Create and draw preview
        Shape preview = shapes[selectedShapeIndex];
        TransformAlgorithms::ApplyRotation(preview, rotationAngle, transformAnchorPoint);
        ShapeRenderer::DrawShape(hdc, preview, RGB(128, 128, 255));
        
        // Draw line from center to current mouse
        DrawLineBresenham(transformAnchorPoint, currentPoint, RGB(255, 0, 0));
    }
}

void GraphicsEngine::OnRButtonDown(int x, int y) {
    if (currentMode == MODE_POLYLINE && tempPoints.size() >= 2) {
        Shape polyline;
        polyline.type = SHAPE_POLYLINE;
        polyline.points = tempPoints;
        polyline.color = RGB(0, 0, 0);
        polyline.selected = false;
        shapes.push_back(polyline);
        tempPoints.clear();
        isDrawing = false;
    }
    else if (currentMode == MODE_POLYGON && tempPoints.size() >= 3) {
        DrawPolygon(tempPoints);
        Shape polygon;
        polygon.type = SHAPE_POLYGON;
        polygon.points = tempPoints;
        polygon.color = RGB(0, 0, 0);
        polygon.selected = false;
        shapes.push_back(polygon);
        tempPoints.clear();
        isDrawing = false;
    }
    else if (currentMode == MODE_FILL_SCANLINE && tempPoints.size() >= 3) {
        DrawLineBresenham(tempPoints.back(), tempPoints.front());
        FillAlgorithms::ScanlineFill(hdc, tempPoints, RGB(255, 0, 0));
        tempPoints.clear();
        isDrawing = false;
    }
    else if (currentMode == MODE_ROTATE && isTransforming && hasSelection) {
        // Complete rotation with right click
        Point2D currentPoint(x, y);
        int dx = currentPoint.x - transformAnchorPoint.x;
        int dy = currentPoint.y - transformAnchorPoint.y;
        double angle = atan2(dy, dx);
        
        // Apply rotation
        TransformAlgorithms::ApplyRotation(shapes[selectedShapeIndex], angle - initialAngle, transformAnchorPoint);
        
        isTransforming = false;
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void GraphicsEngine::DrawExpr1Graphics() {
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    int offsetX = 100, offsetY = 100, scale = 5;
    RoundRect(hdc, offsetX, offsetY, offsetX + 66 * scale, offsetY + 46 * scale, 7 * scale, 7 * scale);
    RoundRect(hdc, offsetX + (66 - 43) / 2 * scale, offsetY + (46 - 30) / 2 * scale,
              offsetX + (66 + 43) / 2 * scale, offsetY + (46 + 30) / 2 * scale, 3 * scale, 3 * scale);
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

// Private helper methods
void GraphicsEngine::HandleLineDrawing(Point2D clickPoint) {
    if (!isDrawing) {
        tempPoints.clear();
        tempPoints.push_back(clickPoint);
        isDrawing = true;
    } else {
        tempPoints.push_back(clickPoint);
        if (currentMode == MODE_LINE_DDA)
            DrawLineDDA(tempPoints[0], tempPoints[1]);
        else
            DrawLineBresenham(tempPoints[0], tempPoints[1]);
        
        Shape line;
        line.type = SHAPE_LINE;
        line.points = tempPoints;
        line.color = RGB(0, 0, 0);
        line.selected = false;
        shapes.push_back(line);
        isDrawing = false;
    }
}

void GraphicsEngine::HandleCircleDrawing(Point2D clickPoint) {
    if (!isDrawing) {
        tempPoints.clear();
        tempPoints.push_back(clickPoint);
        isDrawing = true;
    } else {
        tempPoints.push_back(clickPoint);
        int radius = (int)sqrt(pow(tempPoints[1].x - tempPoints[0].x, 2) +
                             pow(tempPoints[1].y - tempPoints[0].y, 2));
        if (currentMode == MODE_CIRCLE_MIDPOINT)
            DrawCircleMidpoint(tempPoints[0], radius);
        else
            DrawCircleBresenham(tempPoints[0], radius);
        
        Shape circle;
        circle.type = SHAPE_CIRCLE;
        circle.points.push_back(tempPoints[0]);
        circle.radius = radius;
        circle.color = RGB(0, 0, 0);
        circle.selected = false;
        shapes.push_back(circle);
        isDrawing = false;
    }
}

void GraphicsEngine::HandleRectangleDrawing(Point2D clickPoint) {
    if (!isDrawing) {
        tempPoints.clear();
        tempPoints.push_back(clickPoint);
        isDrawing = true;
    } else {
        tempPoints.push_back(clickPoint);
        DrawRectangle(tempPoints[0], tempPoints[1]);
        
        Shape rectangle;
        rectangle.type = SHAPE_RECTANGLE;
        rectangle.points = tempPoints;
        rectangle.color = RGB(0, 0, 0);
        rectangle.selected = false;
        shapes.push_back(rectangle);
        isDrawing = false;
    }
}

void GraphicsEngine::HandlePolyDrawing(Point2D clickPoint) {
    tempPoints.push_back(clickPoint);
    if (!isDrawing) isDrawing = true;
    if (tempPoints.size() >= 2) {
        DrawLineBresenham(tempPoints[tempPoints.size()-2], tempPoints.back());
    }
}

void GraphicsEngine::HandleScanlineFillDrawing(Point2D clickPoint) {
    tempPoints.push_back(clickPoint);
    if (!isDrawing) isDrawing = true;
    if (tempPoints.size() >= 2) {
        DrawLineBresenham(tempPoints[tempPoints.size()-2], tempPoints.back());
    }
}

void GraphicsEngine::HandleSelection(Point2D clickPoint) {
    int hitIndex = ShapeSelector::SelectShapeAt(clickPoint, shapes);
    if (hitIndex >= 0) {
        for (auto& shape : shapes) shape.selected = false;
        shapes[hitIndex].selected = true;
        selectedShapeIndex = hitIndex;
        hasSelection = true;
    } else if (hasSelection) {
        for (auto& shape : shapes) shape.selected = false;
        hasSelection = false;
        selectedShapeIndex = -1;
    }
    InvalidateRect(hwnd, NULL, TRUE);
}

void GraphicsEngine::HandleTranslation(Point2D clickPoint) {
    if (!hasSelection) {
        MessageBoxW(hwnd, L"Please select a shape first", L"Translation", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (!isTransforming) {
        transformStartPoint = clickPoint;
        isTransforming = true;
    } else {
        int dx = clickPoint.x - transformStartPoint.x;
        int dy = clickPoint.y - transformStartPoint.y;
        TransformAlgorithms::ApplyTranslation(shapes[selectedShapeIndex], dx, dy);
        isTransforming = false;
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void GraphicsEngine::HandleScaling(Point2D clickPoint) {
    if (!hasSelection) {
        MessageBoxW(hwnd, L"Please select a shape first", L"Scaling", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (!isTransforming) {
        transformAnchorPoint = TransformAlgorithms::CalculateShapeCenter(shapes[selectedShapeIndex]);
        transformStartPoint = clickPoint;
        int dx = clickPoint.x - transformAnchorPoint.x;
        int dy = clickPoint.y - transformAnchorPoint.y;
        initialDistance = sqrt(dx * dx + dy * dy);
        if (initialDistance < 1.0) initialDistance = 1.0;
        isTransforming = true;
    } else {
        int dx = clickPoint.x - transformAnchorPoint.x;
        int dy = clickPoint.y - transformAnchorPoint.y;
        double currentDistance = sqrt(dx * dx + dy * dy);
        double scale = currentDistance / initialDistance;
        TransformAlgorithms::ApplyScaling(shapes[selectedShapeIndex], scale, transformAnchorPoint);
        isTransforming = false;
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void GraphicsEngine::HandleRotation(Point2D clickPoint) {
    if (!hasSelection) {
        MessageBoxW(hwnd, L"Please select a shape first", L"Rotation", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (!isTransforming) {
        // First click - set rotation center
        transformAnchorPoint = clickPoint;
        isTransforming = true;
        
        // Redraw and show rotation center marker
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
        RenderAll();
        
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        int markerSize = 5;
        MoveToEx(hdc, clickPoint.x - markerSize, clickPoint.y, NULL);
        LineTo(hdc, clickPoint.x + markerSize, clickPoint.y);
        MoveToEx(hdc, clickPoint.x, clickPoint.y - markerSize, NULL);
        LineTo(hdc, clickPoint.x, clickPoint.y + markerSize);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
}

void GraphicsEngine::HandleClippingWindow(Point2D clickPoint) {
    if (!isDefiningClipWindow) {
        clipWindowStart = clickPoint;
        isDefiningClipWindow = true;
    } else {
        clipWindowEnd = clickPoint;
        isDefiningClipWindow = false;
        hasClipWindow = true;
        
        // Redraw with clipping window
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
        RenderAll();
        DrawClipWindow(clipWindowStart, clipWindowEnd);
        
        // Execute clipping
        if (currentMode == MODE_CLIP_COHEN_SUTHERLAND)
            ExecuteCohenSutherlandClipping();
        else if (currentMode == MODE_CLIP_MIDPOINT)
            ExecuteMidpointClipping();
        else if (currentMode == MODE_CLIP_SUTHERLAND_HODGMAN)
            ExecuteSutherlandHodgmanClipping();
        else if (currentMode == MODE_CLIP_WEILER_ATHERTON)
            ExecuteWeilerAthertonClipping();
    }
}

void GraphicsEngine::DrawClipWindow(Point2D p1, Point2D p2) {
    int xmin = (p1.x < p2.x) ? p1.x : p2.x;
    int ymin = (p1.y < p2.y) ? p1.y : p2.y;
    int xmax = (p1.x > p2.x) ? p1.x : p2.x;
    int ymax = (p1.y > p2.y) ? p1.y : p2.y;

    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, xmin, ymin, NULL);
    LineTo(hdc, xmax, ymin);
    LineTo(hdc, xmax, ymax);
    LineTo(hdc, xmin, ymax);
    LineTo(hdc, xmin, ymin);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void GraphicsEngine::ExecuteCohenSutherlandClipping() {
    int xmin = (clipWindowStart.x < clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymin = (clipWindowStart.y < clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;
    int xmax = (clipWindowStart.x > clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymax = (clipWindowStart.y > clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;

    std::vector<Shape> clippedShapes;
    for (Shape& shape : shapes) {
        if (shape.type == SHAPE_LINE && shape.points.size() >= 2) {
            Point2D p1 = shape.points[0], p2 = shape.points[1];
            if (ClippingAlgorithms::ClipLineCohenSutherland(p1, p2, xmin, ymin, xmax, ymax)) {
                Shape clippedLine = shape;
                clippedLine.points[0] = p1;
                clippedLine.points[1] = p2;
                clippedShapes.push_back(clippedLine);
            }
        } else {
            clippedShapes.push_back(shape);
        }
    }
    shapes = clippedShapes;
    hasClipWindow = false;
    InvalidateRect(hwnd, NULL, TRUE);
    MessageBoxW(hwnd, L"Cohen-Sutherland clipping completed!", L"Complete", MB_OK | MB_ICONINFORMATION);
}

void GraphicsEngine::ExecuteMidpointClipping() {
    int xmin = (clipWindowStart.x < clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymin = (clipWindowStart.y < clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;
    int xmax = (clipWindowStart.x > clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymax = (clipWindowStart.y > clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;

    std::vector<Shape> clippedShapes;
    for (Shape& shape : shapes) {
        if (shape.type == SHAPE_LINE && shape.points.size() >= 2) {
            std::vector<std::pair<Point2D, Point2D>> segments;
            ClippingAlgorithms::ClipLineMidpoint(shape.points[0], shape.points[1], xmin, ymin, xmax, ymax, segments);
            for (const auto& seg : segments) {
                Shape clippedLine = shape;
                clippedLine.points.clear();
                clippedLine.points.push_back(seg.first);
                clippedLine.points.push_back(seg.second);
                clippedShapes.push_back(clippedLine);
            }
        } else {
            clippedShapes.push_back(shape);
        }
    }
    shapes = clippedShapes;
    hasClipWindow = false;
    InvalidateRect(hwnd, NULL, TRUE);
    MessageBoxW(hwnd, L"Midpoint clipping completed!", L"Complete", MB_OK | MB_ICONINFORMATION);
}

void GraphicsEngine::ExecuteSutherlandHodgmanClipping() {
    int xmin = (clipWindowStart.x < clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymin = (clipWindowStart.y < clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;
    int xmax = (clipWindowStart.x > clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymax = (clipWindowStart.y > clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;

    std::vector<Shape> clippedShapes;
    for (Shape& shape : shapes) {
        if (shape.type == SHAPE_POLYGON && shape.points.size() >= 3) {
            std::vector<Point2D> clipped = ClippingAlgorithms::ClipPolygonSutherlandHodgman(
                shape.points, xmin, ymin, xmax, ymax);
            if (clipped.size() >= 3) {
                Shape clippedShape = shape;
                clippedShape.points = clipped;
                clippedShapes.push_back(clippedShape);
            }
        } else {
            clippedShapes.push_back(shape);
        }
    }
    shapes = clippedShapes;
    hasClipWindow = false;
    InvalidateRect(hwnd, NULL, TRUE);
    MessageBoxW(hwnd, L"Sutherland-Hodgman clipping completed!", L"Complete", MB_OK | MB_ICONINFORMATION);
}


// 基础绘图方法包装
void GraphicsEngine::DrawLineDDA(Point2D p1, Point2D p2, COLORREF color) {
    LineDrawer::DrawDDA(hdc, p1, p2, color);
}

void GraphicsEngine::DrawLineBresenham(Point2D p1, Point2D p2, COLORREF color) {
    LineDrawer::DrawBresenham(hdc, p1, p2, color);
}

void GraphicsEngine::DrawCircleMidpoint(Point2D center, int radius, COLORREF color) {
    CircleDrawer::DrawMidpoint(hdc, center, radius, color);
}

void GraphicsEngine::DrawCircleBresenham(Point2D center, int radius, COLORREF color) {
    CircleDrawer::DrawBresenham(hdc, center, radius, color);
}

void GraphicsEngine::DrawRectangle(Point2D p1, Point2D p2, COLORREF color) {
    LineDrawer::DrawBresenham(hdc, Point2D(p1.x, p1.y), Point2D(p2.x, p1.y), color);
    LineDrawer::DrawBresenham(hdc, Point2D(p2.x, p1.y), Point2D(p2.x, p2.y), color);
    LineDrawer::DrawBresenham(hdc, Point2D(p2.x, p2.y), Point2D(p1.x, p2.y), color);
    LineDrawer::DrawBresenham(hdc, Point2D(p1.x, p2.y), Point2D(p1.x, p1.y), color);
}

void GraphicsEngine::DrawPolyline(const std::vector<Point2D>& points, COLORREF color) {
    for (size_t i = 1; i < points.size(); i++) {
        LineDrawer::DrawBresenham(hdc, points[i-1], points[i], color);
    }
}

void GraphicsEngine::DrawPolygon(const std::vector<Point2D>& points, COLORREF color) {
    if (points.size() < 3) return;
    for (size_t i = 0; i < points.size(); i++) {
        Point2D p1 = points[i];
        Point2D p2 = points[(i + 1) % points.size()];
        LineDrawer::DrawBresenham(hdc, p1, p2, color);
    }
}


void GraphicsEngine::ExecuteWeilerAthertonClipping() {
    if (!hasClipWindow) {
        MessageBoxW(hwnd, L"Please define a clipping window first", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    
    int xmin = (clipWindowStart.x < clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymin = (clipWindowStart.y < clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;
    int xmax = (clipWindowStart.x > clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymax = (clipWindowStart.y > clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;

    std::vector<Shape> clippedShapes;
    
    for (Shape& shape : shapes) {
        if (shape.type == SHAPE_POLYGON && shape.points.size() >= 3) {
            // Check if completely inside
            bool allInside = true;
            for (const Point2D& pt : shape.points) {
                if (pt.x < xmin || pt.x > xmax || pt.y < ymin || pt.y > ymax) {
                    allInside = false;
                    break;
                }
            }
            
            if (allInside) {
                clippedShapes.push_back(shape);
                continue;
            }
            
            // Check if completely outside
            bool allOutside = true;
            for (const Point2D& pt : shape.points) {
                if (pt.x >= xmin && pt.x <= xmax && pt.y >= ymin && pt.y <= ymax) {
                    allOutside = false;
                    break;
                }
            }
            
            if (allOutside) {
                continue; // Skip this polygon
            }
            
            // Polygon intersects - apply Weiler-Atherton
            std::vector<std::vector<Point2D>> clippedPolygons = 
                ClippingAlgorithms::ClipPolygonWeilerAtherton(shape.points, xmin, ymin, xmax, ymax);
            
            if (!clippedPolygons.empty()) {
                for (const auto& poly : clippedPolygons) {
                    if (poly.size() >= 3) {
                        Shape clippedShape = shape;
                        clippedShape.points = poly;
                        clippedShapes.push_back(clippedShape);
                    }
                }
            } else {
                // Fallback: if algorithm fails, keep original if mostly inside
                int insideCount = 0;
                for (const Point2D& pt : shape.points) {
                    if (pt.x >= xmin && pt.x <= xmax && pt.y >= ymin && pt.y <= ymax) {
                        insideCount++;
                    }
                }
                if (insideCount > (int)shape.points.size() / 2) {
                    clippedShapes.push_back(shape);
                }
            }
        } else {
            clippedShapes.push_back(shape);
        }
    }
    
    shapes = clippedShapes;
    hasClipWindow = false;
    InvalidateRect(hwnd, NULL, TRUE);
    MessageBoxW(hwnd, L"Weiler-Atherton clipping completed!", L"Complete", MB_OK | MB_ICONINFORMATION);
}
