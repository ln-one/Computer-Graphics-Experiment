#include "MeshGenerator.h"
#include "../engine/OpenGLFunctions.h"
#include <cmath>

/**
 * @file MeshGenerator.cpp
 * @brief 三维网格生成算法实现
 * @author 计算机图形学项目组
 */



void MeshGenerator::GenerateCube(Shape3D& shape, float size) {
    // 清空现有数据
    shape.vertices.clear();
    shape.indices.clear();
    
    float halfSize = size * 0.5f;
    
    // 立方体的8个顶点坐标
    float positions[8][3] = {
        // 前面4个顶点
        {-halfSize, -halfSize,  halfSize}, // 0: 左下前
        { halfSize, -halfSize,  halfSize}, // 1: 右下前
        { halfSize,  halfSize,  halfSize}, // 2: 右上前
        {-halfSize,  halfSize,  halfSize}, // 3: 左上前
        // 后面4个顶点
        {-halfSize, -halfSize, -halfSize}, // 4: 左下后
        { halfSize, -halfSize, -halfSize}, // 5: 右下后
        { halfSize,  halfSize, -halfSize}, // 6: 右上后
        {-halfSize,  halfSize, -halfSize}  // 7: 左上后
    };
    
    // 立方体的6个面，每个面有4个顶点
    // 每个面的法线向量和纹理坐标
    struct Face {
        int vertices[4];    // 顶点索引
        float normal[3];    // 法线向量
        float texCoords[4][2]; // 纹理坐标
    };
    
    Face faces[6] = {
        // 前面 (Z+)
        {{0, 1, 2, 3}, {0, 0, 1}, {{0, 0}, {1, 0}, {1, 1}, {0, 1}}},
        // 后面 (Z-)
        {{5, 4, 7, 6}, {0, 0, -1}, {{0, 0}, {1, 0}, {1, 1}, {0, 1}}},
        // 右面 (X+)
        {{1, 5, 6, 2}, {1, 0, 0}, {{0, 0}, {1, 0}, {1, 1}, {0, 1}}},
        // 左面 (X-)
        {{4, 0, 3, 7}, {-1, 0, 0}, {{0, 0}, {1, 0}, {1, 1}, {0, 1}}},
        // 上面 (Y+)
        {{3, 2, 6, 7}, {0, 1, 0}, {{0, 0}, {1, 0}, {1, 1}, {0, 1}}},
        // 下面 (Y-)
        {{4, 5, 1, 0}, {0, -1, 0}, {{0, 0}, {1, 0}, {1, 1}, {0, 1}}}
    };
    
    // 为每个面生成顶点数据
    for (int faceIdx = 0; faceIdx < 6; faceIdx++) {
        const Face& face = faces[faceIdx];
        
        // 为当前面的4个顶点添加数据
        for (int vertIdx = 0; vertIdx < 4; vertIdx++) {
            int posIdx = face.vertices[vertIdx];
            
            // 添加位置坐标 (3 floats)
            shape.vertices.push_back(positions[posIdx][0]);
            shape.vertices.push_back(positions[posIdx][1]);
            shape.vertices.push_back(positions[posIdx][2]);
            
            // 添加法线向量 (3 floats)
            shape.vertices.push_back(face.normal[0]);
            shape.vertices.push_back(face.normal[1]);
            shape.vertices.push_back(face.normal[2]);
            
            // 添加纹理坐标 (2 floats)
            shape.vertices.push_back(face.texCoords[vertIdx][0]);
            shape.vertices.push_back(face.texCoords[vertIdx][1]);
        }
    }
    
    // 生成索引数据（每个面2个三角形，共12个三角形）
    for (int faceIdx = 0; faceIdx < 6; faceIdx++) {
        int baseIdx = faceIdx * 4; // 当前面的起始顶点索引
        
        // 第一个三角形 (0, 1, 2)
        shape.indices.push_back(baseIdx + 0);
        shape.indices.push_back(baseIdx + 1);
        shape.indices.push_back(baseIdx + 2);
        
        // 第二个三角形 (0, 2, 3)
        shape.indices.push_back(baseIdx + 0);
        shape.indices.push_back(baseIdx + 2);
        shape.indices.push_back(baseIdx + 3);
    }
    
    // 设置图形类型
    shape.type = SHAPE3D_CUBE;
    
    // 创建OpenGL缓冲对象
    CreateBuffers(shape);
}

void MeshGenerator::GenerateSphere(Shape3D& shape, float radius, 
                                   int segments, int rings) {
    // TODO: 实现球体网格生成
    // 这将在后续任务中实现
}

void MeshGenerator::GenerateCylinder(Shape3D& shape, float radius, 
                                     float height, int segments) {
    // TODO: 实现柱体网格生成
    // 这将在后续任务中实现
}

void MeshGenerator::GeneratePlane(Shape3D& shape, float width, float height) {
    // TODO: 实现平面网格生成
    // 这将在后续任务中实现
}

void MeshGenerator::CreateBuffers(Shape3D& shape) {
    // 如果已经有缓冲对象，先删除它们
    if (shape.VAO != 0) {
        glDeleteVertexArrays(1, &shape.VAO);
        glDeleteBuffers(1, &shape.VBO);
        glDeleteBuffers(1, &shape.EBO);
    }
    
    // 生成缓冲对象
    glGenVertexArrays(1, &shape.VAO);
    glGenBuffers(1, &shape.VBO);
    glGenBuffers(1, &shape.EBO);
    
    // 绑定VAO
    glBindVertexArray(shape.VAO);
    
    // 绑定并填充VBO
    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 shape.vertices.size() * sizeof(float), 
                 shape.vertices.data(), 
                 GL_STATIC_DRAW);
    
    // 绑定并填充EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 shape.indices.size() * sizeof(unsigned int), 
                 shape.indices.data(), 
                 GL_STATIC_DRAW);
    
    // 设置顶点属性指针
    // 顶点数据格式：[x, y, z, nx, ny, nz, u, v] = 8 floats per vertex
    int stride = 8 * sizeof(float);
    
    // 位置属性 (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    
    // 法线属性 (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // 纹理坐标属性 (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // 解绑VAO
    glBindVertexArray(0);
}