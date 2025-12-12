#pragma once

/**
 * @file Point2D.h
 * @brief 二维点数据结构定义
 * @author 计算机图形学项目组
 */

/**
 * @struct Point2D
 * @brief 二维点结构体
 * 
 * 用于表示二维坐标系中的一个点，包含x和y坐标
 * 主要用于2D图形绘制和几何计算
 */
struct Point2D {
    int x, y;  ///< x坐标和y坐标，使用整数类型以适配像素坐标系统
    
    /**
     * @brief 构造函数
     * @param x x坐标值，默认为0
     * @param y y坐标值，默认为0
     */
    Point2D(int x = 0, int y = 0) : x(x), y(y) {}
};
