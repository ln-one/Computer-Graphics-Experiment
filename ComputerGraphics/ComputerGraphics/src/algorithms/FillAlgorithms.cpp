/**
 * @file FillAlgorithms.cpp
 * @brief 图形填充算法实现
 * @author 计算机图形学项目组
 * 
 * 本文件实现了两种经典的图形填充算法：
 * 1. 边界填充算法（Boundary Fill）- 基于种子点的区域填充
 * 2. 扫描线填充算法（Scanline Fill）- 基于扫描线的多边形填充
 * 
 * 【填充算法分类】
 * - 种子填充算法：从内部一点开始，向外扩散填充（如边界填充、泛洪填充）
 * - 扫描线算法：逐行扫描，计算与边界的交点进行填充
 */

#include "FillAlgorithms.h"
#include <stack>
#include <algorithm>

/**
 * @brief 边界填充算法（扫描线种子填充优化版）
 * @param hdc Windows设备上下文句柄
 * @param hwnd 窗口句柄，用于获取客户区大小
 * @param x 种子点x坐标
 * @param y 种子点y坐标
 * @param fillColor 填充颜色
 * @param boundaryColor 边界颜色
 * 
 * 【算法原理】
 * 边界填充算法是一种种子填充算法，从指定的种子点开始，
 * 检查相邻像素，如果不是边界色也不是填充色，则进行填充。
 * 
 * 本实现采用扫描线优化：
 * - 不是逐像素压栈，而是找到当前行的连续可填充区间
 * - 一次性填充整行，然后将上下行的种子点压栈
 * - 大大减少了栈操作次数，提高效率
 * 
 * 【算法步骤】
 * 1. 检查种子点是否有效（不是边界色也不是填充色）
 * 2. 将种子点压入栈中
 * 3. 循环处理栈中的种子点：
 *    a. 弹出一个种子点
 *    b. 向左右扩展，找到当前行的可填充区间[left, right]
 *    c. 填充该区间
 *    d. 检查上下两行，将新的种子点压栈
 * 4. 重复直到栈为空
 * 
 * 【边界条件】
 * - 遇到边界颜色停止
 * - 遇到已填充颜色停止
 * - 超出窗口范围停止
 * - 设置最大迭代次数防止死循环
 */
void FillAlgorithms::BoundaryFill(HDC hdc, HWND hwnd, int x, int y, COLORREF fillColor, COLORREF boundaryColor) {
    // 获取窗口客户区大小，用于边界检查
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    
    // 检查种子点的初始颜色
    // 如果已经是边界色或填充色，则无需填充
    COLORREF startColor = GetPixel(hdc, x, y);
    if (startColor == boundaryColor || startColor == fillColor) return;
    
    // 使用栈来存储待处理的种子点（非递归实现，避免栈溢出）
    std::stack<Point2D> seedStack;
    seedStack.push(Point2D(x, y));
    
    // 创建画笔用于绘制填充线
    HPEN hPen = CreatePen(PS_SOLID, 1, fillColor);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    
    // 设置最大迭代次数，防止无限循环（例如边界不封闭的情况）
    int maxIterations = 100000, iterations = 0;
    
    // 主循环：处理栈中的所有种子点
    while (!seedStack.empty() && iterations < maxIterations) {
        // 弹出一个种子点
        Point2D seed = seedStack.top();
        seedStack.pop();
        iterations++;
        
        // 边界检查：确保种子点在窗口范围内
        if (seed.x < 0 || seed.x >= clientRect.right || 
            seed.y < 0 || seed.y >= clientRect.bottom) continue;
        
        // 检查当前像素是否可以填充
        COLORREF c = GetPixel(hdc, seed.x, seed.y);
        if (c == boundaryColor || c == fillColor) continue;
        
        // 【扫描线优化】找到当前行的可填充区间
        int left = seed.x, right = seed.x;
        
        // 向左扩展，直到遇到边界或已填充区域
        while (left > 0 && GetPixel(hdc, left - 1, seed.y) != boundaryColor && 
               GetPixel(hdc, left - 1, seed.y) != fillColor) left--;
        
        // 向右扩展，直到遇到边界或已填充区域
        while (right < clientRect.right - 1 && GetPixel(hdc, right + 1, seed.y) != boundaryColor && 
               GetPixel(hdc, right + 1, seed.y) != fillColor) right++;
        
        // 一次性填充整个区间[left, right]
        // 使用LineTo比逐像素SetPixel效率更高
        MoveToEx(hdc, left, seed.y, NULL);
        LineTo(hdc, right + 1, seed.y);
        
        // 检查上下两行，将新的种子点压栈
        // 这里简化处理：将区间内每个点都检查（可进一步优化为区间合并）
        for (int i = left; i <= right; i++) {
            // 检查上一行
            if (seed.y > 0) {
                c = GetPixel(hdc, i, seed.y - 1);
                if (c != boundaryColor && c != fillColor) seedStack.push(Point2D(i, seed.y - 1));
            }
            // 检查下一行
            if (seed.y < clientRect.bottom - 1) {
                c = GetPixel(hdc, i, seed.y + 1);
                if (c != boundaryColor && c != fillColor) seedStack.push(Point2D(i, seed.y + 1));
            }
        }
    }
    
    // 恢复原画笔并释放资源
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

/**
 * @brief 扫描线填充算法
 * @param hdc Windows设备上下文句柄
 * @param polygon 多边形顶点序列（按顺序连接）
 * @param fillColor 填充颜色
 * 
 * 【算法原理】
 * 扫描线填充算法是一种基于扫描线的多边形填充方法：
 * - 从多边形的最低点到最高点，逐行扫描
 * - 计算每条扫描线与多边形边界的交点
 * - 将交点排序后，两两配对填充中间区域
 * 
 * 【算法步骤】
 * 1. 找到多边形的y坐标范围[ymin, ymax]
 * 2. 对于每条扫描线y（从ymin到ymax）：
 *    a. 计算扫描线与所有边的交点
 *    b. 将交点按x坐标排序
 *    c. 将交点两两配对，填充每对交点之间的区域
 * 
 * 【交点计算】
 * 对于边(p1, p2)，如果扫描线y穿过该边：
 * - 条件：(p1.y <= y < p2.y) 或 (p2.y <= y < p1.y)
 * - 交点x = p1.x + (y - p1.y) / (p2.y - p1.y) * (p2.x - p1.x)
 * 
 * 【奇偶规则】
 * 交点排序后，第1-2个交点之间填充，第3-4个之间填充...
 * 这就是"奇偶规则"：从左到右，奇数次穿越边界进入多边形，
 * 偶数次穿越边界离开多边形。
 * 
 * 【注意事项】
 * - 本实现是简化版本，适用于简单多边形
 * - 对于复杂多边形（自相交），可能需要更复杂的处理
 */
void FillAlgorithms::ScanlineFill(HDC hdc, const std::vector<Point2D>& polygon, COLORREF fillColor) {
    // 多边形至少需要3个顶点
    if (polygon.size() < 3) return;
    
    // 【步骤1】找到多边形的y坐标范围
    int ymin = polygon[0].y, ymax = polygon[0].y;
    for (const auto& p : polygon) {
        if (p.y < ymin) ymin = p.y;  // 更新最小y
        if (p.y > ymax) ymax = p.y;  // 更新最大y
    }
    
    // 【步骤2】逐条扫描线处理
    for (int y = ymin; y <= ymax; y++) {
        // 存储当前扫描线与所有边的交点x坐标
        std::vector<int> intersections;
        
        // 【步骤2a】计算扫描线与每条边的交点
        for (size_t i = 0; i < polygon.size(); i++) {
            // 获取当前边的两个端点
            Point2D p1 = polygon[i];
            Point2D p2 = polygon[(i + 1) % polygon.size()];  // 下一个顶点，形成闭合
            
            // 判断扫描线是否穿过该边
            // 使用半开区间避免顶点重复计算：[ymin, ymax)
            if ((p1.y <= y && p2.y > y) || (p2.y <= y && p1.y > y)) {
                // 计算交点的x坐标（线性插值）
                // x = x1 + (y - y1) / (y2 - y1) * (x2 - x1)
                float x = p1.x + (float)(y - p1.y) / (p2.y - p1.y) * (p2.x - p1.x);
                intersections.push_back((int)(x + 0.5));  // 四舍五入
            }
        }
        
        // 【步骤2b】将交点按x坐标排序
        std::sort(intersections.begin(), intersections.end());
        
        // 【步骤2c】两两配对填充
        // 根据奇偶规则：第0-1对、第2-3对...之间的区域需要填充
        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            // 填充从intersections[i]到intersections[i+1]的水平线段
            for (int x = intersections[i]; x <= intersections[i + 1]; x++) {
                SetPixel(hdc, x, y, fillColor);
            }
        }
    }
}
