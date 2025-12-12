#pragma once
#include "../core/Point2D.h"
#include "../core/Shape.h"
#include <vector>

/**
 * @file ShapeSelector.h
 * @brief 图形选择器类定义
 * @author 计算机图形学项目组
 */

/**
 * @class ShapeSelector
 * @brief 图形选择器类
 * 
 * 负责处理图形的选择逻辑和选择状态的可视化显示
 * 提供点击测试功能来确定用户点击了哪个图形
 */
class ShapeSelector {
public:
    /**
     * @brief 在指定位置选择图形
     * @param clickPoint 鼠标点击位置
     * @param shapes 图形对象集合
     * @return 被选中图形的索引，如果没有图形被选中则返回-1
     * 
     * 遍历所有图形，找到第一个包含点击点的图形
     * 优先选择最后绘制的图形（视觉上在最上层）
     */
    static int SelectShapeAt(Point2D clickPoint, const std::vector<Shape>& shapes);
    
    /**
     * @brief 绘制选择指示器
     * @param hdc Windows设备上下文句柄
     * @param shape 被选中的图形对象
     * 
     * 在选中的图形周围绘制选择指示器（如控制点、边框等）
     */
    static void DrawSelectionIndicator(HDC hdc, const Shape& shape);

private:
    /**
     * @brief 直线的点击测试
     * @param point 测试点
     * @param p1 直线起点
     * @param p2 直线终点
     * @param tolerance 容差范围（像素）
     * @return 如果点在直线附近返回true
     */
    static bool HitTestLine(Point2D point, Point2D p1, Point2D p2, int tolerance = 5);
    
    /**
     * @brief 圆形的点击测试
     * @param point 测试点
     * @param center 圆心
     * @param radius 半径
     * @param tolerance 容差范围（像素）
     * @return 如果点在圆形边界附近返回true
     */
    static bool HitTestCircle(Point2D point, Point2D center, int radius, int tolerance = 5);
    
    /**
     * @brief 多边形的点击测试
     * @param point 测试点
     * @param polygon 多边形顶点序列
     * @return 如果点在多边形内部返回true
     * 
     * 使用射线法判断点是否在多边形内部
     */
    static bool HitTestPolygon(Point2D point, const std::vector<Point2D>& polygon);
    
    /**
     * @brief 计算点到直线的距离
     * @param point 测试点
     * @param lineStart 直线起点
     * @param lineEnd 直线终点
     * @return 点到直线的最短距离
     */
    static double PointToLineDistance(Point2D point, Point2D lineStart, Point2D lineEnd);
};
