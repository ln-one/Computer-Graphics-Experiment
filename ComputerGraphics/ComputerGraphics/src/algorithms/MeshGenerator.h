#pragma once
#include "../core/Shape3D.h"
#include <vector>

/**
 * @file MeshGenerator.h
 * @brief 三维网格生成算法类
 * @author ln1.opensource@gmail.com
 */

/**
 * @class MeshGenerator
 * @brief 三维网格生成算法类
 * 
 * 提供各种基本三维图形的网格生成功能，包括球体、柱体、平面和立方体。
 * 生成的网格数据包含顶点坐标、法线向量和纹理坐标，适用于OpenGL渲染。
 * 参照2D算法类（LineDrawer, CircleDrawer）的设计模式。
 */
class MeshGenerator {
public:
    /**
     * @brief 生成立方体网格
     * @param shape 要填充网格数据的Shape3D对象引用
     * @param size 立方体的边长
     * 
     * 生成一个以原点为中心的立方体网格，边长为size。
     * 立方体的顶点坐标范围为[-size/2, size/2]。
     * 生成的顶点数据格式：[x, y, z, nx, ny, nz, u, v]
     * - 位置坐标(x,y,z): 3个float
     * - 法线向量(nx,ny,nz): 3个float  
     * - 纹理坐标(u,v): 2个float
     */
    static void GenerateCube(Shape3D& shape, float size);
    
    /**
     * @brief 生成球体网格
     * @param shape 要填充网格数据的Shape3D对象引用
     * @param radius 球体半径
     * @param segments 水平分段数（经线数量）
     * @param rings 垂直分段数（纬线数量）
     */
    static void GenerateSphere(Shape3D& shape, float radius, 
                               int segments, int rings);
    
    /**
     * @brief 生成柱体网格
     * @param shape 要填充网格数据的Shape3D对象引用
     * @param radius 柱体底面半径
     * @param height 柱体高度
     * @param segments 圆周分段数
     */
    static void GenerateCylinder(Shape3D& shape, float radius, 
                                 float height, int segments);
    
    /**
     * @brief 生成平面网格
     * @param shape 要填充网格数据的Shape3D对象引用
     * @param width 平面宽度
     * @param height 平面高度
     */
    static void GeneratePlane(Shape3D& shape, float width, float height);
    
private:
    /**
     * @brief 创建OpenGL缓冲对象
     * @param shape 包含顶点和索引数据的Shape3D对象引用
     * 
     * 根据shape中的vertices和indices数据创建VAO、VBO和EBO，
     * 并设置顶点属性指针。顶点数据格式：
     * - location 0: 位置坐标 (3 floats)
     * - location 1: 法线向量 (3 floats)  
     * - location 2: 纹理坐标 (2 floats)
     */
    static void CreateBuffers(Shape3D& shape);
};