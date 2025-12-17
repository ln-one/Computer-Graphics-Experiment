#pragma once

/**
 * @file Point3D.h
 * @brief 三维点数据结构定义
 * @author ln1.opensource@gmail.com
 */

/**
 * @struct Point3D
 * @brief 三维点结构体
 * 
 * 用于表示三维坐标系中的一个点，包含x、y、z坐标
 * 主要用于3D图形绘制、变换和几何计算
 */
struct Point3D {
    float x, y, z;  ///< 三维坐标值，使用浮点数以支持精确的3D计算
    
    /**
     * @brief 默认构造函数
     * 将所有坐标初始化为0，创建原点
     */
    Point3D() : x(0), y(0), z(0) {}
    
    /**
     * @brief 参数化构造函数
     * @param x x坐标值
     * @param y y坐标值  
     * @param z z坐标值
     */
    Point3D(float x, float y, float z) : x(x), y(y), z(z) {}
};
