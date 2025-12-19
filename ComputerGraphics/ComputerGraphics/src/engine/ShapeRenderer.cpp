/**
 * @file ShapeRenderer.cpp
 * @brief 图形渲染器实现
 * @author ln1.opensource@gmail.com
 * 
 * 本文件实现了图形渲染器的核心功能，负责将Shape对象绑定到屏幕上。
 * 根据图形类型自动选择合适的绑定算法。
 */

#include "ShapeRenderer.h"
#include "../algorithms/LineDrawer.h"
#include "../algorithms/CircleDrawer.h"

/**
 * @brief 绑定图形对象
 * @param hdc Windows设备上下文句柄
 * @param shape 待绑定的图形对象
 * @param color 绑定颜色
 * 
 * 根据图形类型调用相应的绑定算法：
 * - 直线：使用Bresenham算法
 * - 圆形：使用Bresenham算法
 * - 矩形：绑定四条边
 * - 折线：依次连接各顶点
 * - 多边形：连接各顶点并闭合
 */
void ShapeRenderer::DrawShape(HDC hdc, const Shape& shape, COLORREF color) {
    switch (shape.type) {
        case SHAPE_LINE:
            // 直线：使用Bresenham算法绑定
            if (shape.points.size() >= 2)
                LineDrawer::DrawBresenham(hdc, shape.points[0], shape.points[1], color);
            break;
            
        case SHAPE_CIRCLE:
            // 圆形：使用Bresenham算法绑定
            if (shape.points.size() >= 1)
                CircleDrawer::DrawBresenham(hdc, shape.points[0], shape.radius, color);
            break;
            
        case SHAPE_RECTANGLE:
            // 矩形：绑定四条边
            if (shape.points.size() >= 2) {
                Point2D p1 = shape.points[0], p2 = shape.points[1];
                LineDrawer::DrawBresenham(hdc, Point2D(p1.x, p1.y), Point2D(p2.x, p1.y), color);  // 上边
                LineDrawer::DrawBresenham(hdc, Point2D(p2.x, p1.y), Point2D(p2.x, p2.y), color);  // 右边
                LineDrawer::DrawBresenham(hdc, Point2D(p2.x, p2.y), Point2D(p1.x, p2.y), color);  // 下边
                LineDrawer::DrawBresenham(hdc, Point2D(p1.x, p2.y), Point2D(p1.x, p1.y), color);  // 左边
            }
            break;
            
        case SHAPE_POLYLINE:
            // 折线：依次连接各顶点（不闭合）
            for (size_t i = 1; i < shape.points.size(); i++)
                LineDrawer::DrawBresenham(hdc, shape.points[i-1], shape.points[i], color);
            break;
            
        case SHAPE_POLYGON:
            // 多边形：连接各顶点并闭合
            for (size_t i = 1; i < shape.points.size(); i++)
                LineDrawer::DrawBresenham(hdc, shape.points[i-1], shape.points[i], color);
            // 闭合多边形（连接最后一个顶点和第一个顶点）
            if (shape.points.size() >= 3)
                LineDrawer::DrawBresenham(hdc, shape.points.back(), shape.points.front(), color);
            break;
            
        case SHAPE_BSPLINE:
            // B样条曲线：暂未实现
            break;
    }
}
