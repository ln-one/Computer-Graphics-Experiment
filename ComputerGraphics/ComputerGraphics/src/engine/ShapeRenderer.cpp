#include "ShapeRenderer.h"
#include "../algorithms/LineDrawer.h"
#include "../algorithms/CircleDrawer.h"

void ShapeRenderer::DrawShape(HDC hdc, const Shape& shape, COLORREF color) {
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
            for (size_t i = 1; i < shape.points.size(); i++)
                LineDrawer::DrawBresenham(hdc, shape.points[i-1], shape.points[i], color);
            break;
        case SHAPE_POLYGON:
            for (size_t i = 1; i < shape.points.size(); i++)
                LineDrawer::DrawBresenham(hdc, shape.points[i-1], shape.points[i], color);
            if (shape.points.size() >= 3)
                LineDrawer::DrawBresenham(hdc, shape.points.back(), shape.points.front(), color);
            break;
        case SHAPE_BSPLINE:
            break;
    }
}
