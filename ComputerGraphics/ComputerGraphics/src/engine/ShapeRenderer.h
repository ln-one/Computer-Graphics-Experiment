#pragma once
#include "../core/Shape.h"
#include <windows.h>

/**
 * @file ShapeRenderer.h
 * @brief 图形渲染器类定义
 * @author ln1.opensource@gmail.com
 */

/**
 * @class ShapeRenderer
 * @brief 图形渲染器类
 * 
 * 负责将Shape对象渲染到屏幕上，根据图形类型调用相应的绘制算法
 * 提供统一的图形绘制接口，隐藏具体的绘制实现细节
 */
class ShapeRenderer {
public:
    /**
     * @brief 绘制图形对象
     * @param hdc Windows设备上下文句柄
     * @param shape 待绘制的图形对象
     * @param color 绘制颜色（可选，会覆盖图形自身的颜色）
     * 
     * 根据图形类型自动选择合适的绘制算法进行渲染
     * 支持所有定义在ShapeType中的图形类型
     */
    static void DrawShape(HDC hdc, const Shape& shape, COLORREF color);
};
