#pragma once
#include "Point3D.h"
#include <vector>

/**
 * @file Shape3D.h
 * @brief 三维图形数据结构定义
 * @author 计算机图形学项目组
 */

/**
 * @enum Shape3DType
 * @brief 三维图形类型枚举
 * 
 * 定义了系统支持的所有三维图形类型
 */
enum Shape3DType {
    SHAPE3D_SPHERE,   ///< 球体
    SHAPE3D_CYLINDER, ///< 圆柱体
    SHAPE3D_PLANE,    ///< 平面
    SHAPE3D_CUBE      ///< 立方体
};

/**
 * @struct Shape3D
 * @brief 三维图形结构体
 * 
 * 包含了绘制和管理三维图形所需的所有信息，包括几何变换、
 * 材质属性、纹理信息和OpenGL渲染数据
 */
struct Shape3D {
    Shape3DType type;  ///< 三维图形类型
    
    // 变换参数
    float positionX, positionY, positionZ;  ///< 位置坐标（世界坐标系）
    float rotationX, rotationY, rotationZ;  ///< 绕X、Y、Z轴的旋转角度（弧度）
    float scaleX, scaleY, scaleZ;           ///< X、Y、Z方向的缩放因子
    
    // 材质参数（Phong光照模型）
    float ambient[3];   ///< 环境光反射系数（RGB）
    float diffuse[3];   ///< 漫反射系数（RGB）
    float specular[3];  ///< 镜面反射系数（RGB）
    float shininess;    ///< 镜面反射指数（控制高光大小）
    
    // 纹理相关
    unsigned int textureID;  ///< OpenGL纹理ID
    bool hasTexture;         ///< 是否应用纹理标志
    
    // 网格数据（用于OpenGL渲染）
    std::vector<float> vertices;         ///< 顶点数据数组
    std::vector<unsigned int> indices;   ///< 索引数据数组
    unsigned int VAO, VBO, EBO;          ///< OpenGL缓冲对象（顶点数组对象、顶点缓冲对象、元素缓冲对象）
    
    bool selected;  ///< 是否被选中状态标志
    
    /**
     * @brief 默认构造函数
     * 
     * 初始化为球体，位于原点，无旋转，单位缩放
     * 设置默认材质属性和OpenGL缓冲对象
     */
    Shape3D() : type(SHAPE3D_SPHERE), 
                positionX(0), positionY(0), positionZ(0),
                rotationX(0), rotationY(0), rotationZ(0),
                scaleX(1), scaleY(1), scaleZ(1),
                shininess(32.0f), textureID(0), hasTexture(false),
                VAO(0), VBO(0), EBO(0), selected(false) {
        // 设置默认材质属性
        ambient[0] = ambient[1] = ambient[2] = 0.2f;   // 低环境光
        diffuse[0] = diffuse[1] = diffuse[2] = 0.8f;   // 高漫反射
        specular[0] = specular[1] = specular[2] = 1.0f; // 全镜面反射
    }
};
