#include "ShapeSelector.h"
#include <cmath>
#include <windows.h>

int ShapeSelector::SelectShapeAt(Point2D clickPoint, const std::vector<Shape>& shapes) {
    for (int i = (int)shapes.size() - 1; i >= 0; i--) {
        const Shape& shape = shapes[i];
        switch (shape.type) {
            case SHAPE_LINE:
                if (shape.points.size() >= 2 && HitTestLine(clickPoint, shape.points[0], shape.points[1]))
                    return i;
                break;
            case SHAPE_CIRCLE:
                if (shape.points.size() >= 1 && HitTestCircle(clickPoint, shape.points[0], shape.radius))
                    return i;
                break;
            case SHAPE_RECTANGLE:
                if (shape.points.size() >= 2) {
                    Point2D p1 = shape.points[0], p2 = shape.points[1];
                    if (HitTestLine(clickPoint, Point2D(p1.x, p1.y), Point2D(p2.x, p1.y)) ||
                        HitTestLine(clickPoint, Point2D(p2.x, p1.y), Point2D(p2.x, p2.y)) ||
                        HitTestLine(clickPoint, Point2D(p2.x, p2.y), Point2D(p1.x, p2.y)) ||
                        HitTestLine(clickPoint, Point2D(p1.x, p2.y), Point2D(p1.x, p1.y)))
                        return i;
                }
                break;
            case SHAPE_POLYLINE:
                if (shape.points.size() >= 2) {
                    for (size_t j = 1; j < shape.points.size(); j++) {
                        if (HitTestLine(clickPoint, shape.points[j-1], shape.points[j]))
                            return i;
                    }
                }
                break;
            case SHAPE_POLYGON:
                if (shape.points.size() >= 3 && HitTestPolygon(clickPoint, shape.points))
                    return i;
                break;
        }
    }
    return -1;
}

void ShapeSelector::DrawSelectionIndicator(HDC hdc, const Shape& shape) {
    if (shape.points.empty()) return;
    
    HPEN hDashedPen = CreatePen(PS_DASH, 1, RGB(0, 0, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hDashedPen);

    int minX = shape.points[0].x, maxX = shape.points[0].x;
    int minY = shape.points[0].y, maxY = shape.points[0].y;
    
    for (const Point2D& pt : shape.points) {
        if (pt.x < minX) minX = pt.x;
        if (pt.x > maxX) maxX = pt.x;
        if (pt.y < minY) minY = pt.y;
        if (pt.y > maxY) maxY = pt.y;
    }
    
    if (shape.type == SHAPE_CIRCLE) {
        minX -= shape.radius; maxX += shape.radius;
        minY -= shape.radius; maxY += shape.radius;
    }
    
    int padding = 5;
    minX -= padding; minY -= padding;
    maxX += padding; maxY += padding;

    MoveToEx(hdc, minX, minY, NULL);
    LineTo(hdc, maxX, minY);
    LineTo(hdc, maxX, maxY);
    LineTo(hdc, minX, maxY);
    LineTo(hdc, minX, minY);

    SelectObject(hdc, hOldPen);
    DeleteObject(hDashedPen);
}

bool ShapeSelector::HitTestLine(Point2D point, Point2D p1, Point2D p2, int tolerance) {
    return PointToLineDistance(point, p1, p2) <= tolerance;
}

bool ShapeSelector::HitTestCircle(Point2D point, Point2D center, int radius, int tolerance) {
    int dx = point.x - center.x;
    int dy = point.y - center.y;
    double distance = sqrt(dx * dx + dy * dy);
    return abs(distance - radius) <= tolerance;
}

bool ShapeSelector::HitTestPolygon(Point2D point, const std::vector<Point2D>& polygon) {
    int n = (int)polygon.size();
    bool inside = false;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        int xi = polygon[i].x, yi = polygon[i].y;
        int xj = polygon[j].x, yj = polygon[j].y;
        bool intersect = ((yi > point.y) != (yj > point.y)) &&
                        (point.x < (xj - xi) * (point.y - yi) / (yj - yi) + xi);
        if (intersect) inside = !inside;
    }
    return inside;
}

double ShapeSelector::PointToLineDistance(Point2D point, Point2D lineStart, Point2D lineEnd) {
    int dx = lineEnd.x - lineStart.x;
    int dy = lineEnd.y - lineStart.y;
    if (dx == 0 && dy == 0) {
        int px = point.x - lineStart.x;
        int py = point.y - lineStart.y;
        return sqrt(px * px + py * py);
    }
    double t = ((point.x - lineStart.x) * dx + (point.y - lineStart.y) * dy) / (double)(dx * dx + dy * dy);
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    double closestX = lineStart.x + t * dx;
    double closestY = lineStart.y + t * dy;
    double distX = point.x - closestX;
    double distY = point.y - closestY;
    return sqrt(distX * distX + distY * distY);
}
