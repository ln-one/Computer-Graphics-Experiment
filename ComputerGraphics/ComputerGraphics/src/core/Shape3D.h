#pragma once
#include "Point3D.h"
#include <vector>

// 3D图形类型
enum Shape3DType {
    SHAPE3D_SPHERE,
    SHAPE3D_CYLINDER,
    SHAPE3D_PLANE,
    SHAPE3D_CUBE
};

// 3D图形结构（参照2D的Shape结构）
struct Shape3D {
    Shape3DType type;
    
    // 变换参数
    float positionX, positionY, positionZ;
    float rotationX, rotationY, rotationZ;  // 绕X,Y,Z轴旋转角度
    float scaleX, scaleY, scaleZ;
    
    // 材质参数
    float ambient[3];   // 环境光
    float diffuse[3];   // 漫反射
    float specular[3];  // 镜面反射
    float shininess;    // 光泽度
    
    // 纹理
    unsigned int textureID;
    bool hasTexture;
    
    // 网格数据
    std::vector<float> vertices;  // 顶点数据
    std::vector<unsigned int> indices;  // 索引数据
    unsigned int VAO, VBO, EBO;  // OpenGL缓冲对象
    
    bool selected;
    
    Shape3D() : type(SHAPE3D_SPHERE), 
                positionX(0), positionY(0), positionZ(0),
                rotationX(0), rotationY(0), rotationZ(0),
                scaleX(1), scaleY(1), scaleZ(1),
                shininess(32.0f), textureID(0), hasTexture(false),
                VAO(0), VBO(0), EBO(0), selected(false) {
        // 默认材质
        ambient[0] = ambient[1] = ambient[2] = 0.2f;
        diffuse[0] = diffuse[1] = diffuse[2] = 0.8f;
        specular[0] = specular[1] = specular[2] = 1.0f;
    }
};
