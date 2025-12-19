/**
 * @file ShapeSelector.cpp
 * @brief 图形选择器实现
 * @author ln1.opensource@gmail.com
 * 
 * 本文件实现了图形选择器的核心功能，包括：
 * 1. 点击测试（判断点击位置是否在图形上）
 * 2. 图形选择（找到被点击的图形）
 * 3. 选择指示器绑定（显示选中状态）
 */

#include "ShapeSelector.h"
#include <cmath>
#include <windows.h>

/**
 * @brief 在指定位置选择图形
 * @param clickPoint 鼠标点击位置
 * @param shapes 图形对象集合
 * @return 被选中图形的索引，如果没有图形被选中则返回-1
 * 
 * 从后向前遍历图形集合（后绑定的图形在视觉上位于上层），
 * 找到第一个包含点击点的图形。
 */
int ShapeSelector::SelectShapeAt(Point2D clickPoint, const std::vector<Shape>& shapes) {
    // 从后向前遍历，优先选择最后绑定的图形（视觉上在最上层）
    for (int i = (int)shapes.size() - 1; i >= 0; i--) {
        const Shape& shape = shapes[i];
        switch (shape.type) {
            case SHAPE_LINE:
                // 直线：检测点是否在直线附近
                if (shape.points.size() >= 2 && HitTestLine(clickPoint, shape.points[0], shape.points[1]))
                    return i;
                break;
                
            case SHAPE_CIRCLE:
                // 圆形：检测点是否在圆周附近
                if (shape.points.size() >= 1 && HitTestCircle(clickPoint, shape.points[0], shape.radius))
                    return i;
                break;
                
            case SHAPE_RECTANGLE:
                // 矩形：检测点是否在四条边附近
                if (shape.points.size() >= 2) {
                    Point2D p1 = shape.points[0], p2 = shape.points[1];
                    if (HitTestLine(clickPoint, Point2D(p1.x, p1.y), Point2D(p2.x, p1.y)) ||  // 上边
                        HitTestLine(clickPoint, Point2D(p2.x, p1.y), Point2D(p2.x, p2.y)) ||  // 右边
                        HitTestLine(clickPoint, Point2D(p2.x, p2.y), Point2D(p1.x, p2.y)) ||  // 下边
                        HitTestLine(clickPoint, Point2D(p1.x, p2.y), Point2D(p1.x, p1.y)))    // 左边
                        return i;
                }
                break;
                
            case SHAPE_POLYLINE:
                // 折线：检测点是否在任意线段附近
                if (shape.points.size() >= 2) {
                    for (size_t j = 1; j < shape.points.size(); j++) {
                        if (HitTestLine(clickPoint, shape.points[j-1], shape.points[j]))
                            return i;
                    }
                }
                break;
                
            case SHAPE_POLYGON:
                // 多边形：检测点是否在多边形内部
                if (shape.points.size() >= 3 && HitTestPolygon(clickPoint, shape.points))
                    return i;
                break;
        }
    }
    return -1;  // 没有找到被点击的图形
}

/**
 * @brief 绑定选择指示器
 * @param hdc Windows设备上下文句柄
 * @param shape 被选中的图形对象
 * 
 * 在选中的图形周围绑定一个蓝色虚线边框，
 * 帮助用户识别当前选中的图形。
 */
void ShapeSelector::DrawSelectionIndicator(HDC hdc, const Shape& shape) {
    if (shape.points.empty()) return;
    
    // 创建蓝色虚线画笔
    HPEN hDashedPen = CreatePen(PS_DASH, 1, RGB(0, 0, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hDashedPen);

    // 计算图形的包围盒
    int minX = shape.points[0].x, maxX = shape.points[0].x;
    int minY = shape.points[0].y, maxY = shape.points[0].y;
    
    for (const Point2D& pt : shape.points) {
        if (pt.x < minX) minX = pt.x;
        if (pt.x > maxX) maxX = pt.x;
        if (pt.y < minY) minY = pt.y;
        if (pt.y > maxY) maxY = pt.y;
    }
    
    // 圆形需要考虑半径
    if (shape.type == SHAPE_CIRCLE) {
        minX -= shape.radius; maxX += shape.radius;
        minY -= shape.radius; maxY += shape.radius;
    }
    
    // 添加边距
    int padding = 5;
    minX -= padding; minY -= padding;
    maxX += padding; maxY += padding;

    // 绑定选择边框
    MoveToEx(hdc, minX, minY, NULL);
    LineTo(hdc, maxX, minY);
    LineTo(hdc, maxX, maxY);
    LineTo(hdc, minX, maxY);
    LineTo(hdc, minX, minY);

    // 恢复原画笔
    SelectObject(hdc, hOldPen);
    DeleteObject(hDashedPen);
}

/**
 * @brief 直线的点击测试
 * @param point 测试点
 * @param p1 直线起点
 * @param p2 直线终点
 * @param tolerance 容差范围（像素）
 * @return 如果点在直线附近返回true
 */
bool ShapeSelector::HitTestLine(Point2D point, Point2D p1, Point2D p2, int tolerance) {
    return PointToLineDistance(point, p1, p2) <= tolerance;
}

/**
 * @brief 圆形的点击测试
 * @param point 测试点
 * @param center 圆心
 * @param radius 半径
 * @param tolerance 容差范围（像素）
 * @return 如果点在圆形边界附近返回true
 * 
 * 计算点到圆心的距离，判断是否接近圆周
 */
bool ShapeSelector::HitTestCircle(Point2D point, Point2D center, int radius, int tolerance) {
    int dx = point.x - center.x;
    int dy = point.y - center.y;
    double distance = sqrt(dx * dx + dy * dy);
    // 判断点到圆周的距离是否在容差范围内
    return abs(distance - radius) <= tolerance;
}

/**
 * @brief 多边形的点击测试（射线法）
 * @param point 测试点
 * @param polygon 多边形顶点序列
 * @return 如果点在多边形内部返回true
 * 
 * 使用射线法判断点是否在多边形内部：
 * 从测试点向右发射一条水平射线，计算与多边形边的交点数。
 * 如果交点数为奇数，则点在多边形内部。
 */
bool ShapeSelector::HitTestPolygon(Point2D point, const std::vector<Point2D>& polygon) {
    int n = (int)polygon.size();
    bool inside = false;
    
    // 遍历多边形的每条边
    for (int i = 0, j = n - 1; i < n; j = i++) {
        int xi = polygon[i].x, yi = polygon[i].y;
        int xj = polygon[j].x, yj = polygon[j].y;
        
        // 判断射线是否与边相交
        bool intersect = ((yi > point.y) != (yj > point.y)) &&
                        (point.x < (xj - xi) * (point.y - yi) / (yj - yi) + xi);
        if (intersect) inside = !inside;  // 每次相交翻转状态
    }
    return inside;
}

/**
 * @brief 计算点到线段的距离
 * @param point 测试点
 * @param lineStart 线段起点
 * @param lineEnd 线段终点
 * @return 点到线段的最短距离
 * 
 * 算法说明：
 * 1. 计算点在线段上的投影位置（参数t）
 * 2. 将t限制在[0,1]范围内（确保投影点在线段上）
 * 3. 计算点到投影点的距离
 */
double ShapeSelector::PointToLineDistance(Point2D point, Point2D lineStart, Point2D lineEnd) {
    int dx = lineEnd.x - lineStart.x;
    int dy = lineEnd.y - lineStart.y;
    
    // 如果线段退化为点
    if (dx == 0 && dy == 0) {
        int px = point.x - lineStart.x;
        int py = point.y - lineStart.y;
        return sqrt(px * px + py * py);
    }
    
    // 计算投影参数t
    double t = ((point.x - lineStart.x) * dx + (point.y - lineStart.y) * dy) / (double)(dx * dx + dy * dy);
    
    // 将t限制在[0,1]范围内
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    
    // 计算投影点坐标
    double closestX = lineStart.x + t * dx;
    double closestY = lineStart.y + t * dy;
    
    // 计算点到投影点的距离
    double distX = point.x - closestX;
    double distY = point.y - closestY;
    return sqrt(distX * distX + distY * distY);
}
