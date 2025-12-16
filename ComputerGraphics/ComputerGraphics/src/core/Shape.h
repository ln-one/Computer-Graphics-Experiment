#pragma once
#include "Point2D.h"
#include <windows.h>
#include <vector>

/**
 * @file Shape.h
 * @brief 二维图形数据结构定义
 * @author 计算机图形学项目组
 */

/**
 * @enum ShapeType
 * @brief 图形类型枚举
 * 
 * 定义了系统支持的所有二维图形类型
 */
enum ShapeType {
    SHAPE_LINE,      ///< 直线
    SHAPE_CIRCLE,    ///< 圆形
    SHAPE_RECTANGLE, ///< 矩形
    SHAPE_POLYLINE,  ///< 折线（多段线）
    SHAPE_POLYGON,   ///< 多边形
    SHAPE_BSPLINE    ///< B样条曲线
};

/**
 * @struct Shape
 * @brief 二维图形结构体
 * 
 * 包含了绘制和管理二维图形所需的所有信息
 * 支持多种图形类型和属性设置
 */
struct Shape {
    ShapeType type;                ///< 图形类型
    std::vector<Point2D> points;   ///< 构成图形的关键点集合
    COLORREF color;                ///< 图形颜色（Windows颜色格式）
    int radius;                    ///< 圆形半径（仅对圆形有效）
    bool selected;                 ///< 是否被选中状态标志

    /**
     * @brief 默认构造函数
     * 初始化为黑色直线，未选中状态
     */
    Shape() : type(SHAPE_LINE), color(RGB(0, 0, 0)), radius(0), selected(false) {}
};
