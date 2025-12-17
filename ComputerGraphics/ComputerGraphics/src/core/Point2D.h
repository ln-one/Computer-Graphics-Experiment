/**
 * @dir core
 * @brief 核心数据结构目录
 * 
 * 本目录包含项目的基础数据类型定义，是整个图形系统的数据基础。
 * 
 * 目录内容：
 * - Point2D.h   - 二维点结构，用于2D图形绘制
 * - Point3D.h   - 三维点结构，用于3D图形绘制
 * - Shape.h     - 二维图形结构，包含类型、顶点、颜色等属性
 * - Shape3D.h   - 三维图形结构，包含变换、材质、纹理等属性
 * - DrawMode.h  - 绘图模式枚举，定义各种绘图操作类型
 * 
 * 使用说明：
 * 这些数据结构被 algorithms/ 和 engine/ 目录中的代码广泛使用，
 * 是连接用户界面和图形算法的桥梁。
 */

#pragma once

/**
 * @file Point2D.h
 * @brief 二维点数据结构定义
 * @author ln1.opensource@gmail.com
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
