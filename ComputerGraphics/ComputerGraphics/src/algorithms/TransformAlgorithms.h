#pragma once
#include "../core/Point2D.h"
#include "../core/Shape.h"

class TransformAlgorithms {
public:
    static Point2D CalculateShapeCenter(const Shape& shape);
    static void ApplyTranslation(Shape& shape, int dx, int dy);
    static void ApplyScaling(Shape& shape, double scale, Point2D center);
    static void ApplyRotation(Shape& shape, double angle, Point2D center);
};
