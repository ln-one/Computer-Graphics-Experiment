#include "TransformAlgorithms.h"
#include <cmath>

Point2D TransformAlgorithms::CalculateShapeCenter(const Shape& shape) {
    if (shape.points.empty()) return Point2D(0, 0);
    
    int sumX = 0, sumY = 0;
    for (const Point2D& pt : shape.points) {
        sumX += pt.x;
        sumY += pt.y;
    }
    return Point2D(sumX / (int)shape.points.size(), sumY / (int)shape.points.size());
}

void TransformAlgorithms::ApplyTranslation(Shape& shape, int dx, int dy) {
    for (auto& p : shape.points) {
        p.x += dx;
        p.y += dy;
    }
}

void TransformAlgorithms::ApplyScaling(Shape& shape, double scale, Point2D center) {
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

void TransformAlgorithms::ApplyRotation(Shape& shape, double angle, Point2D center) {
    double cosA = cos(angle);
    double sinA = sin(angle);
    for (auto& p : shape.points) {
        int dx = p.x - center.x;
        int dy = p.y - center.y;
        p.x = center.x + (int)(dx * cosA - dy * sinA);
        p.y = center.y + (int)(dx * sinA + dy * cosA);
    }
}
