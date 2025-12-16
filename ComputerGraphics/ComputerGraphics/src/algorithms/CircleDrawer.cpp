/**
 * @file CircleDrawer.cpp
 * @brief 圆形绘制算法实现
 * @author 计算机图形学项目组
 * 
 * 本文件实现了两种经典的圆形绘制算法：
 * 1. 中点圆算法（Midpoint Circle Algorithm）
 * 2. Bresenham圆算法（Bresenham Circle Algorithm）
 * 
 * 【圆的八分对称性】
 * 圆具有高度的对称性，以圆心为原点，圆上任意一点(x,y)对应7个对称点：
 * (x,y), (-x,y), (x,-y), (-x,-y), (y,x), (-y,x), (y,-x), (-y,-x)
 * 因此只需计算1/8圆弧（从0°到45°），就可以通过对称性得到整个圆。
 * 这大大减少了计算量，提高了绘制效率。
 */

#include "CircleDrawer.h"

/**
 * @brief 设置指定位置的像素颜色
 * @param hdc Windows设备上下文句柄
 * @param x 像素x坐标
 * @param y 像素y坐标
 * @param color 像素颜色
 * 
 * 封装Windows GDI的SetPixel函数，用于在指定位置绘制单个像素点
 */
void CircleDrawer::SetPixel(HDC hdc, int x, int y, COLORREF color) {
    ::SetPixel(hdc, x, y, color);
}

/**
 * @brief 利用圆的八分对称性绘制八个对称点
 * @param hdc Windows设备上下文句柄
 * @param center 圆心坐标
 * @param x 相对于圆心的x偏移（第一象限45°以下的点）
 * @param y 相对于圆心的y偏移（第一象限45°以下的点）
 * @param color 像素颜色
 * 
 * 【八分对称性原理】
 * 对于圆心在原点的圆，如果(x,y)在圆上，则以下8个点都在圆上：
 * 
 *        (-x,y)  |  (x,y)
 *     (-y,x)     |     (y,x)
 *   -------------|-------------
 *     (-y,-x)    |     (y,-x)
 *        (-x,-y) |  (x,-y)
 * 
 * 这8个点分别对应圆的8个45°扇区，通过一次计算绘制8个点
 */
void CircleDrawer::DrawCirclePoints(HDC hdc, Point2D center, int x, int y, COLORREF color) {
    // 第一象限：0°-45°区域的点 (x, y)
    SetPixel(hdc, center.x + x, center.y + y, color);
    // 第二象限：135°-180°区域的点 (-x, y)
    SetPixel(hdc, center.x - x, center.y + y, color);
    // 第四象限：315°-360°区域的点 (x, -y)
    SetPixel(hdc, center.x + x, center.y - y, color);
    // 第三象限：180°-225°区域的点 (-x, -y)
    SetPixel(hdc, center.x - x, center.y - y, color);
    // 第一象限：45°-90°区域的点 (y, x) - x和y交换
    SetPixel(hdc, center.x + y, center.y + x, color);
    // 第二象限：90°-135°区域的点 (-y, x)
    SetPixel(hdc, center.x - y, center.y + x, color);
    // 第四象限：270°-315°区域的点 (y, -x)
    SetPixel(hdc, center.x + y, center.y - x, color);
    // 第三象限：225°-270°区域的点 (-y, -x)
    SetPixel(hdc, center.x - y, center.y - x, color);
}

/**
 * @brief 中点圆绘制算法
 * @param hdc Windows设备上下文句柄
 * @param center 圆心坐标
 * @param radius 圆的半径
 * @param color 圆的颜色
 * 
 * 【算法原理】
 * 中点圆算法基于圆的隐式方程：F(x,y) = x² + y² - r² = 0
 * - F(x,y) < 0：点在圆内
 * - F(x,y) = 0：点在圆上
 * - F(x,y) > 0：点在圆外
 * 
 * 从点(0, r)开始，沿顺时针方向绘制1/8圆弧（到45°位置）。
 * 每一步x增加1，根据中点的位置决定y是否减少1。
 * 
 * 【算法步骤】
 * 1. 初始化：x=0, y=r, 决策参数 d = 1 - r
 * 2. 绘制当前点及其7个对称点
 * 3. 计算下一个像素的中点M(x+1, y-0.5)处的判别值：
 *    - 如果 d < 0：中点在圆内，选择E点(x+1, y)，d += 2x + 3
 *    - 如果 d >= 0：中点在圆外，选择SE点(x+1, y-1)，d += 2(x-y) + 5，y--
 * 4. x++，重复步骤2-3直到 x > y
 * 
 * 【决策参数推导】
 * 初始决策参数 d₀ = F(1, r-0.5) = 1 + (r-0.5)² - r² = 1.25 - r
 * 为避免浮点运算，取 d₀ = 1 - r（近似处理）
 */
void CircleDrawer::DrawMidpoint(HDC hdc, Point2D center, int radius, COLORREF color) {
    int x = 0, y = radius;  // 从圆的最上方点(0, r)开始
    
    // 初始决策参数 d = 1 - r
    // 这是中点(1, r-0.5)代入圆方程的近似值
    int d = 1 - radius;

    // 只需绘制1/8圆弧（从90°到45°），利用对称性绘制完整圆
    // 当 x > y 时，已经超过45°，停止循环
    while (x <= y) {
        // 利用八分对称性绘制8个点
        DrawCirclePoints(hdc, center, x, y, color);
        
        if (d < 0) {
            // 中点在圆内，选择正右方的点E(x+1, y)
            // 下一个中点的决策参数增量：d_new = d + 2x + 3
            d += 2 * x + 3;
        } else {
            // 中点在圆外或圆上，选择右下方的点SE(x+1, y-1)
            // 下一个中点的决策参数增量：d_new = d + 2(x-y) + 5
            d += 2 * (x - y) + 5;
            y--;  // y坐标减1
        }
        x++;  // x坐标始终增加1
    }
}

/**
 * @brief Bresenham圆绘制算法
 * @param hdc Windows设备上下文句柄
 * @param center 圆心坐标
 * @param radius 圆的半径
 * @param color 圆的颜色
 * 
 * 【算法原理】
 * Bresenham圆算法是中点圆算法的变体，同样基于圆的隐式方程，
 * 但使用不同的决策参数初始值和增量计算方式。
 * 
 * 【算法步骤】
 * 1. 初始化：x=0, y=r, 决策参数 d = 3 - 2r
 * 2. 绘制当前点及其7个对称点
 * 3. 根据决策参数选择下一个像素：
 *    - 如果 d < 0：选择E点(x+1, y)，d += 4x + 6
 *    - 如果 d >= 0：选择SE点(x+1, y-1)，d += 4(x-y) + 10，y--
 * 4. x++，重复步骤2-3直到 x > y
 * 
 * 【与中点圆算法的区别】
 * - 初始决策参数不同：Bresenham用 3-2r，中点用 1-r
 * - 增量系数不同：Bresenham的增量是中点算法的2倍
 * - 两种算法本质相同，只是决策参数的定义方式不同
 * 
 * 【优点】
 * - 只使用整数加法和移位运算
 * - 计算效率高，适合硬件实现
 */
void CircleDrawer::DrawBresenham(HDC hdc, Point2D center, int radius, COLORREF color) {
    int x = 0, y = radius;  // 从圆的最上方点(0, r)开始
    
    // Bresenham算法的初始决策参数
    // d = 3 - 2r 是该算法的特征初始值
    int d = 3 - 2 * radius;

    // 只需绘制1/8圆弧，利用对称性绘制完整圆
    while (x <= y) {
        // 利用八分对称性绘制8个点
        DrawCirclePoints(hdc, center, x, y, color);
        
        if (d < 0) {
            // 选择正右方的点E(x+1, y)
            // 决策参数增量：4x + 6
            d += 4 * x + 6;
        } else {
            // 选择右下方的点SE(x+1, y-1)
            // 决策参数增量：4(x-y) + 10
            d += 4 * (x - y) + 10;
            y--;  // y坐标减1
        }
        x++;  // x坐标始终增加1
    }
}
