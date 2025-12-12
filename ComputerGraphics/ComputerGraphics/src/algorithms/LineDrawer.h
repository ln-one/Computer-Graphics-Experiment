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
