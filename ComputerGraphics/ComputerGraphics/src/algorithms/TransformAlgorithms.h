#pragma once
#include "../core/Point2D.h"
#include "../core/Shape.h"

/**
 * @file TransformAlgorithms.h
 * @brief 几何变换算法类定义
 * @author 计算机图形学项目组
 */

/**
 * @class TransformAlgorithms
 * @brief 几何变换算法实现类
 * 
 * 提供二维图形的基本几何变换操作，包括平移、缩放和旋转
 * 所有变换都基于齐次坐标系统实现
 */
class TransformAlgorithms {
public:
    /**
     * @brief 计算图形的几何中心
     * @param shape 待计算的图形对象
     * @return 图形的几何中心坐标
     * 
     * 通过计算图形所有顶点坐标的平均值来确定几何中心
     */
    static Point2D CalculateShapeCenter(const Shape& shape);
    
    /**
     * @brief 应用平移变换
     * @param shape 待变换的图形对象（引用，会被修改）
     * @param dx x方向的平移距离
     * @param dy y方向的平移距离
     * 
     * 将图形的所有顶点按指定的偏移量进行平移
     */
    static void ApplyTranslation(Shape& shape, int dx, int dy);
    
    /**
     * @brief 应用缩放变换
     * @param shape 待变换的图形对象（引用，会被修改）
     * @param scale 缩放因子（大于1放大，小于1缩小）
     * @param center 缩放中心点
     * 
     * 以指定点为中心对图形进行等比例缩放
     */
    static void ApplyScaling(Shape& shape, double scale, Point2D center);
    
    /**
     * @brief 应用旋转变换
     * @param shape 待变换的图形对象（引用，会被修改）
     * @param angle 旋转角度（弧度，逆时针为正）
     * @param center 旋转中心点
     * 
     * 以指定点为中心对图形进行旋转变换
     */
    static void ApplyRotation(Shape& shape, double angle, Point2D center);
};
