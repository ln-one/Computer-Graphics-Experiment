#pragma once
#include "../core/Point2D.h"
#include <windows.h>

/**
 * @file CircleDrawer.h
 * @brief 圆形绘制算法类定义
 * @author 计算机图形学项目组
 */

/**
 * @class CircleDrawer
 * @brief 圆形绘制算法实现类
 * 
 * 提供多种经典的圆形绘制算法实现，包括中点圆算法和Bresenham圆算法
 * 利用圆的八分对称性来提高绘制效率
 */
class CircleDrawer {
public:
    /**
     * @brief 中点圆绘制算法
     * @param hdc Windows设备上下文句柄
     * @param center 圆心坐标
     * @param radius 圆的半径
     * @param color 圆的颜色，默认为黑色
     * 
     * 使用中点圆算法绘制圆形。该算法基于圆的隐式方程，
     * 通过判断中点位置来决定下一个像素的选择
     */
    static void DrawMidpoint(HDC hdc, Point2D center, int radius, COLORREF color = RGB(0, 0, 0));
    
    /**
     * @brief Bresenham圆绘制算法
     * @param hdc Windows设备上下文句柄
     * @param center 圆心坐标
     * @param radius 圆的半径
     * @param color 圆的颜色，默认为黑色
     * 
     * 使用Bresenham圆算法绘制圆形。该算法是中点圆算法的改进版本，
     * 只使用整数运算，效率更高
     */
    static void DrawBresenham(HDC hdc, Point2D center, int radius, COLORREF color = RGB(0, 0, 0));

private:
    /**
     * @brief 设置指定位置的像素颜色
     * @param hdc Windows设备上下文句柄
     * @param x 像素x坐标
     * @param y 像素y坐标
     * @param color 像素颜色
     */
    static void SetPixel(HDC hdc, int x, int y, COLORREF color);
    
    /**
     * @brief 利用圆的八分对称性绘制八个对称点
     * @param hdc Windows设备上下文句柄
     * @param center 圆心坐标
     * @param x 相对于圆心的x偏移
     * @param y 相对于圆心的y偏移
     * @param color 像素颜色
     * 
     * 根据圆的对称性，一次计算可以绘制八个对称的像素点
     */
    static void DrawCirclePoints(HDC hdc, Point2D center, int x, int y, COLORREF color);
};
