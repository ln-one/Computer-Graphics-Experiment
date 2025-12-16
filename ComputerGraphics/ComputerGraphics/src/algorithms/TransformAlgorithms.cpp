/**
 * @file TransformAlgorithms.cpp
 * @brief 二维几何变换算法实现
 * @author 计算机图形学项目组
 * 
 * 本文件实现了三种基本的二维几何变换：
 * 1. 平移变换（Translation）
 * 2. 缩放变换（Scaling）
 * 3. 旋转变换（Rotation）
 * 
 * 【齐次坐标与变换矩阵】
 * 在计算机图形学中，通常使用齐次坐标和变换矩阵来表示几何变换。
 * 二维点(x, y)的齐次坐标表示为(x, y, 1)。
 * 
 * 平移矩阵：          缩放矩阵：          旋转矩阵：
 * | 1  0  dx |       | sx 0  0 |        | cosθ -sinθ 0 |
 * | 0  1  dy |       | 0  sy 0 |        | sinθ  cosθ 0 |
 * | 0  0  1  |       | 0  0  1 |        | 0     0    1 |
 * 
 * 本实现直接使用公式计算，未显式构建矩阵，但原理相同。
 */

#include "TransformAlgorithms.h"
#include <cmath>

/**
 * @brief 计算图形的几何中心
 * @param shape 待计算的图形对象
 * @return 图形的几何中心坐标
 * 
 * 【计算方法】
 * 几何中心（质心）是所有顶点坐标的算术平均值：
 * - center.x = (x1 + x2 + ... + xn) / n
 * - center.y = (y1 + y2 + ... + yn) / n
 * 
 * 【用途】
 * 几何中心常用作缩放和旋转的默认参考点，
 * 使图形相对于自身中心进行变换。
 */
Point2D TransformAlgorithms::CalculateShapeCenter(const Shape& shape) {
    // 空图形返回原点
    if (shape.points.empty()) return Point2D(0, 0);
    
    // 累加所有顶点的坐标
    int sumX = 0, sumY = 0;
    for (const Point2D& pt : shape.points) {
        sumX += pt.x;
        sumY += pt.y;
    }
    
    // 计算平均值作为几何中心
    return Point2D(sumX / (int)shape.points.size(), sumY / (int)shape.points.size());
}

/**
 * @brief 应用平移变换
 * @param shape 待变换的图形对象（引用，会被修改）
 * @param dx x方向的平移距离（正值向右，负值向左）
 * @param dy y方向的平移距离（正值向下，负值向上）
 * 
 * 【数学原理】
 * 平移是最简单的几何变换，将点(x, y)移动到(x+dx, y+dy)。
 * 
 * 变换公式：
 * - x' = x + dx
 * - y' = y + dy
 * 
 * 齐次坐标矩阵形式：
 * | x' |   | 1  0  dx |   | x |
 * | y' | = | 0  1  dy | × | y |
 * | 1  |   | 0  0  1  |   | 1 |
 * 
 * 【特点】
 * - 平移不改变图形的形状和大小
 * - 平移不改变图形的方向
 * - 平移是刚体变换的一种
 */
void TransformAlgorithms::ApplyTranslation(Shape& shape, int dx, int dy) {
    // 对图形的每个顶点应用平移
    for (auto& p : shape.points) {
        p.x += dx;  // x坐标加上水平偏移
        p.y += dy;  // y坐标加上垂直偏移
    }
}

/**
 * @brief 应用缩放变换
 * @param shape 待变换的图形对象（引用，会被修改）
 * @param scale 缩放因子（>1放大，<1缩小，=1不变）
 * @param center 缩放中心点
 * 
 * 【数学原理】
 * 缩放变换改变图形的大小，以指定点为中心进行缩放。
 * 
 * 相对于原点的缩放公式：
 * - x' = x × sx
 * - y' = y × sy
 * 
 * 相对于任意点(cx, cy)的缩放公式：
 * 1. 先平移使中心点移到原点：x1 = x - cx, y1 = y - cy
 * 2. 进行缩放：x2 = x1 × s, y2 = y1 × s
 * 3. 再平移回原位置：x' = x2 + cx, y' = y2 + cy
 * 
 * 合并后：
 * - x' = cx + (x - cx) × s
 * - y' = cy + (y - cy) × s
 * 
 * 【等比例缩放 vs 非等比例缩放】
 * - 等比例缩放：sx = sy，保持图形比例不变
 * - 非等比例缩放：sx ≠ sy，会导致图形变形
 * 本实现使用等比例缩放（sx = sy = scale）
 */
void TransformAlgorithms::ApplyScaling(Shape& shape, double scale, Point2D center) {
    // 对图形的每个顶点应用缩放
    for (auto& p : shape.points) {
        // 计算顶点相对于缩放中心的偏移
        int dx = p.x - center.x;
        int dy = p.y - center.y;
        
        // 应用缩放并加回中心点坐标
        p.x = center.x + (int)(dx * scale);
        p.y = center.y + (int)(dy * scale);
    }
    
    // 特殊处理：如果是圆形，还需要缩放半径
    if (shape.type == SHAPE_CIRCLE) {
        shape.radius = (int)(shape.radius * scale);
    }
}

/**
 * @brief 应用旋转变换
 * @param shape 待变换的图形对象（引用，会被修改）
 * @param angle 旋转角度（弧度制，逆时针为正方向）
 * @param center 旋转中心点
 * 
 * 【数学原理】
 * 旋转变换使图形绕指定点旋转一定角度。
 * 
 * 相对于原点的旋转公式（逆时针旋转θ角）：
 * - x' = x × cosθ - y × sinθ
 * - y' = x × sinθ + y × cosθ
 * 
 * 相对于任意点(cx, cy)的旋转公式：
 * 1. 先平移使旋转中心移到原点
 * 2. 进行旋转
 * 3. 再平移回原位置
 * 
 * 合并后：
 * - x' = cx + (x - cx) × cosθ - (y - cy) × sinθ
 * - y' = cy + (x - cx) × sinθ + (y - cy) × cosθ
 * 
 * 【旋转矩阵推导】
 * 设点P(x, y)到原点距离为r，与x轴夹角为α，则：
 * - x = r × cosα
 * - y = r × sinα
 * 
 * 旋转θ角后：
 * - x' = r × cos(α + θ) = r × (cosα×cosθ - sinα×sinθ) = x×cosθ - y×sinθ
 * - y' = r × sin(α + θ) = r × (sinα×cosθ + cosα×sinθ) = x×sinθ + y×cosθ
 * 
 * 【注意事项】
 * - 角度使用弧度制，如需度数转换：弧度 = 度数 × π / 180
 * - 逆时针为正方向（数学坐标系），屏幕坐标系y轴向下，视觉效果为顺时针
 */
void TransformAlgorithms::ApplyRotation(Shape& shape, double angle, Point2D center) {
    // 预计算三角函数值，避免在循环中重复计算
    double cosA = cos(angle);  // 余弦值
    double sinA = sin(angle);  // 正弦值
    
    // 对图形的每个顶点应用旋转
    for (auto& p : shape.points) {
        // 计算顶点相对于旋转中心的偏移
        int dx = p.x - center.x;
        int dy = p.y - center.y;
        
        // 应用旋转公式并加回中心点坐标
        // x' = cx + dx×cosθ - dy×sinθ
        // y' = cy + dx×sinθ + dy×cosθ
        p.x = center.x + (int)(dx * cosA - dy * sinA);
        p.y = center.y + (int)(dx * sinA + dy * cosA);
    }
}
