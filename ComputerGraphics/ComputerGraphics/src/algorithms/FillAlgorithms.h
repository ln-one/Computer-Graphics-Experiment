#pragma once
#include "../core/Point2D.h"
#include <windows.h>
#include <vector>

/**
 * @file FillAlgorithms.h
 * @brief 图形填充算法类定义
 * @author 计算机图形学项目组
 */

/**
 * @class FillAlgorithms
 * @brief 图形填充算法实现类
 * 
 * 提供多种经典的图形填充算法，包括边界填充和扫描线填充
 */
class FillAlgorithms {
public:
    /**
     * @brief 边界填充算法（种子填充）
     * @param hdc Windows设备上下文句柄
     * @param hwnd 窗口句柄
     * @param x 种子点x坐标
     * @param y 种子点y坐标
     * @param fillColor 填充颜色
     * @param boundaryColor 边界颜色
     * 
     * 从指定种子点开始，向四个方向扩散填充，直到遇到边界颜色为止
     * 适用于填充封闭区域
     */
    static void BoundaryFill(HDC hdc, HWND hwnd, int x, int y, COLORREF fillColor, COLORREF boundaryColor);
    
    /**
     * @brief 扫描线填充算法
     * @param hdc Windows设备上下文句柄
     * @param polygon 多边形顶点序列
     * @param fillColor 填充颜色
     * 
     * 使用扫描线算法填充多边形内部，通过构建边表和活性边表
     * 来确定每条扫描线与多边形的交点，然后填充交点间的区域
     */
    static void ScanlineFill(HDC hdc, const std::vector<Point2D>& polygon, COLORREF fillColor);

private:
    /**
     * @struct EdgeTableEntry
     * @brief 边表项结构
     * 
     * 用于扫描线填充算法中的边表和活性边表
     */
    struct EdgeTableEntry {
        int ymax;    ///< 边的最大y坐标
        float x;     ///< 当前扫描线与边的交点x坐标
        float dx;    ///< x坐标的增量（斜率的倒数）
    };
};
