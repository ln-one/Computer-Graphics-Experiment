/**
 * @file LineDrawer.cpp
 * @brief 直线绘制算法实现
 * @author ln1.opensource@gmail.com
 * 
 * 本文件实现了两种经典的直线绘制算法：
 * 1. DDA（数字微分分析器）算法 - 基于浮点增量计算
 * 2. Bresenham算法 - 基于整数运算的高效算法
 * 
 * 这两种算法都是光栅化直线的基础算法，用于将数学上的连续直线
 * 转换为离散的像素点序列。
 */

#include "LineDrawer.h"
#include <cmath>

/**
 * @brief 设置指定位置的像素颜色
 * @param hdc Windows设备上下文句柄
 * @param x 像素x坐标
 * @param y 像素y坐标
 * @param color 像素颜色
 * 
 * 封装Windows GDI的SetPixel函数，用于在指定位置绘制单个像素点
 */
void LineDrawer::SetPixel(HDC hdc, int x, int y, COLORREF color) {
    ::SetPixel(hdc, x, y, color);
}

/**
 * @brief DDA（数字微分分析器）直线绘制算法
 * @param hdc Windows设备上下文句柄
 * @param p1 直线起点
 * @param p2 直线终点
 * @param color 直线颜色
 * 
 * 【算法原理】
 * DDA算法基于直线的参数方程和微分思想：
 * - 直线方程：y = kx + b，其中 k = dy/dx
 * - 核心思想：沿着变化较大的方向（x或y）每次步进1个像素，
 *   另一个方向按斜率比例增加
 * 
 * 【算法步骤】
 * 1. 计算x和y方向的增量 dx = x2-x1, dy = y2-y1
 * 2. 确定步数 steps = max(|dx|, |dy|)，选择变化大的方向作为主方向
 * 3. 计算每步的增量 xInc = dx/steps, yInc = dy/steps
 * 4. 从起点开始，每次将x和y分别加上对应增量，四舍五入后绘制像素
 * 5. 重复步骤4直到绘制完所有steps+1个像素点
 * 
 * 【优缺点】
 * 优点：算法简单直观，易于理解和实现
 * 缺点：需要浮点运算和四舍五入，效率相对较低
 */
void LineDrawer::DrawDDA(HDC hdc, Point2D p1, Point2D p2, COLORREF color) {
    // 计算x和y方向的总增量
    int dx = p2.x - p1.x;  // x方向增量
    int dy = p2.y - p1.y;  // y方向增量
    
    // 确定步数：选择变化较大的方向作为主方向
    // 这样可以保证直线的连续性，不会出现断点
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    // 如果起点和终点重合，无需绘制
    if (steps == 0) return;

    // 计算每一步x和y的增量
    // 主方向每步增加1（或-1），另一方向按比例增加
    float xInc = (float)dx / steps;  // x方向每步增量
    float yInc = (float)dy / steps;  // y方向每步增量
    
    // 使用浮点数保存当前位置，以保证精度
    float x = (float)p1.x;
    float y = (float)p1.y;

    // 循环绘制每个像素点
    for (int i = 0; i <= steps; i++) {
        // 四舍五入到最近的整数像素位置并绘制
        // 加0.5后取整实现四舍五入效果
        SetPixel(hdc, (int)(x + 0.5), (int)(y + 0.5), color);
        
        // 更新当前位置
        x += xInc;
        y += yInc;
    }
}

/**
 * @brief Bresenham直线绘制算法
 * @param hdc Windows设备上下文句柄
 * @param p1 直线起点
 * @param p2 直线终点
 * @param color 直线颜色
 * 
 * 【算法原理】
 * Bresenham算法是一种高效的直线光栅化算法，其核心思想是：
 * - 只使用整数加法和位移运算，避免浮点运算
 * - 通过误差累积来决定下一个像素的位置
 * - 每次在主方向步进1，根据误差值决定是否在另一方向也步进
 * 
 * 【算法步骤】（以|dx|>|dy|为例）
 * 1. 计算 dx = |x2-x1|, dy = |y2-y1|
 * 2. 确定x和y的步进方向 sx, sy（+1或-1）
 * 3. 初始化误差值 err = dx - dy
 * 4. 绘制当前像素点
 * 5. 计算 e2 = 2 * err
 *    - 如果 e2 > -dy：err -= dy, x += sx（x方向步进）
 *    - 如果 e2 < dx：err += dx, y += sy（y方向步进）
 * 6. 重复步骤4-5直到到达终点
 * 
 * 【误差值的含义】
 * err表示当前像素到理想直线的偏差累积：
 * - err > 0 时，偏向x轴方向，需要在x方向步进
 * - err < 0 时，偏向y轴方向，需要在y方向步进
 * 
 * 【优缺点】
 * 优点：只使用整数运算，效率高，是最常用的直线绘制算法
 * 缺点：算法理解相对复杂
 */
void LineDrawer::DrawBresenham(HDC hdc, Point2D p1, Point2D p2, COLORREF color) {
    // 计算x和y方向的绝对增量
    int dx = abs(p2.x - p1.x);  // x方向距离（绝对值）
    int dy = abs(p2.y - p1.y);  // y方向距离（绝对值）
    
    // 确定x和y的步进方向
    // sx: x增加的方向，+1表示向右，-1表示向左
    // sy: y增加的方向，+1表示向下，-1表示向上
    int sx = p1.x < p2.x ? 1 : -1;
    int sy = p1.y < p2.y ? 1 : -1;
    
    // 初始化误差值
    // err = dx - dy 是Bresenham算法的核心
    // 它综合考虑了x和y两个方向的偏差
    int err = dx - dy;
    
    // 当前绘制位置
    int x = p1.x, y = p1.y;

    // 循环绘制直到到达终点
    while (true) {
        // 绘制当前像素
        SetPixel(hdc, x, y, color);
        
        // 检查是否到达终点
        if (x == p2.x && y == p2.y) break;

        // 计算2倍误差值，用于判断步进方向
        // 使用2*err避免浮点除法，这是算法高效的关键
        int e2 = 2 * err;
        
        // 判断是否需要在x方向步进
        // 当 e2 > -dy 时，说明偏差偏向x轴，需要x步进
        if (e2 > -dy) { 
            err -= dy;   // 更新误差值
            x += sx;     // x方向步进
        }
        
        // 判断是否需要在y方向步进
        // 当 e2 < dx 时，说明偏差偏向y轴，需要y步进
        if (e2 < dx) { 
            err += dx;   // 更新误差值
            y += sy;     // y方向步进
        }
    }
}
