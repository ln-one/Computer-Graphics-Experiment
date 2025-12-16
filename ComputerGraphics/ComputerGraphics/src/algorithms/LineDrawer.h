/**
 * @dir algorithms
 * @brief 图形算法实现目录
 * 
 * 本目录包含各种图形学算法的具体实现，是项目的算法核心。
 * 
 * 目录内容：
 * 
 * 【2D绘图算法】
 * - LineDrawer.*        - 直线绘制算法（DDA、Bresenham）
 * - CircleDrawer.*      - 圆形绘制算法（中点圆、Bresenham圆）
 * - FillAlgorithms.*    - 区域填充算法（边界填充、扫描线填充）
 * - TransformAlgorithms.* - 几何变换算法（平移、旋转、缩放）
 * 
 * 【裁剪算法】
 * - ClippingAlgorithms.* - 裁剪算法集合
 *   - Cohen-Sutherland 直线裁剪
 *   - 中点分割直线裁剪
 *   - Sutherland-Hodgman 多边形裁剪
 *   - Weiler-Atherton 多边形裁剪
 * 
 * 【3D图形算法】
 * - MeshGenerator.*     - 3D网格生成器（立方体、球体、圆柱体、平面）
 * - ShaderManager.*     - OpenGL着色器管理（编译、链接、使用）
 * - TextureLoader.*     - 纹理加载器（支持常见图片格式）
 * 
 * 使用说明：
 * 所有算法类都提供静态方法，可以直接调用而无需实例化。
 * 算法实现遵循计算机图形学教材中的经典算法描述。
 */

#pragma once
#include "../core/Point2D.h"
#include <windows.h>

/**
 * @file LineDrawer.h
 * @brief 直线绘制算法类定义
 * @author 计算机图形学项目组
 */

/**
 * @class LineDrawer
 * @brief 直线绘制算法实现类
 * 
 * 提供多种经典的直线绘制算法实现，包括DDA算法和Bresenham算法
 * 所有方法都是静态方法，可以直接调用而无需实例化
 */
class LineDrawer {
public:
    /**
     * @brief DDA（数字微分分析器）直线绘制算法
     * @param hdc Windows设备上下文句柄
     * @param p1 直线起点
     * @param p2 直线终点
     * @param color 直线颜色，默认为黑色
     * 
     * 使用DDA算法绘制从p1到p2的直线。该算法基于直线的微分方程，
     * 通过增量计算来确定像素位置，适用于浮点运算
     */
    static void DrawDDA(HDC hdc, Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));
    
    /**
     * @brief Bresenham直线绘制算法
     * @param hdc Windows设备上下文句柄
     * @param p1 直线起点
     * @param p2 直线终点
     * @param color 直线颜色，默认为黑色
     * 
     * 使用Bresenham算法绘制从p1到p2的直线。该算法只使用整数运算，
     * 效率更高，是最常用的直线绘制算法
     */
    static void DrawBresenham(HDC hdc, Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));

private:
    /**
     * @brief 设置指定位置的像素颜色
     * @param hdc Windows设备上下文句柄
     * @param x 像素x坐标
     * @param y 像素y坐标
     * @param color 像素颜色
     */
    static void SetPixel(HDC hdc, int x, int y, COLORREF color);
};
