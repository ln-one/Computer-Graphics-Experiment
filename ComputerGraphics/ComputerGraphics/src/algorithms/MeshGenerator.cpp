#include "MeshGenerator.h"
#include "../engine/OpenGLFunctions.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @file MeshGenerator.cpp
 * @brief 三维网格生成算法实现
 * @author 计算机图形学项目组
 */



void MeshGenerator::GenerateCube(Shape3D& shape, float size) {
    shape.type = SHAPE3D_CUBE;
    shape.vertices.clear();
    shape.indices.clear();
    
    float halfSize = size / 2.0f;
    
    // 立方体有6个面，每个面4个顶点
    // 顶点格式: x, y, z, nx, ny, nz, u, v
    
    // 前面 (z = +halfSize)
    shape.vertices.insert(shape.vertices.end(), {
        -halfSize, -halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         halfSize, -halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f
    });
    
    // 后面 (z = -halfSize)
    shape.vertices.insert(shape.vertices.end(), {
         halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        -halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f
    });
    
    // 上面 (y = +halfSize)
    shape.vertices.insert(shape.vertices.end(), {
        -halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    });
    
    // 下面 (y = -halfSize)
    shape.vertices.insert(shape.vertices.end(), {
        -halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f
    });
    
    // 右面 (x = +halfSize)
    shape.vertices.insert(shape.vertices.end(), {
         halfSize, -halfSize,  halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         halfSize, -halfSize, -halfSize,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         halfSize,  halfSize, -halfSize,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f
    });
    
    // 左面 (x = -halfSize)
    shape.vertices.insert(shape.vertices.end(), {
        -halfSize, -halfSize, -halfSize, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -halfSize, -halfSize,  halfSize, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -halfSize,  halfSize,  halfSize, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -halfSize,  halfSize, -halfSize, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f
    });
    
    // 索引 - 每个面2个三角形
    for (unsigned int face = 0; face < 6; face++) {
        unsigned int base = face * 4;
        shape.indices.insert(shape.indices.end(), {
            base + 0, base + 1, base + 2,
            base + 0, base + 2, base + 3
        });
    }
    
    CreateBuffers(shape);
}

void MeshGenerator::GenerateSphere(Shape3D& shape, float radius, int segments, int rings) {
    shape.type = SHAPE3D_SPHERE;
    shape.vertices.clear();
    shape.indices.clear();
    
    // 生成顶点
    for (int ring = 0; ring <= rings; ring++) {
        float phi = (float)M_PI * ring / rings;  // 0 到 PI
        float y = radius * cosf(phi);
        float ringRadius = radius * sinf(phi);
        
        for (int seg = 0; seg <= segments; seg++) {
            float theta = 2.0f * (float)M_PI * seg / segments;  // 0 到 2PI
            float x = ringRadius * cosf(theta);
            float z = ringRadius * sinf(theta);
            
            // 法线向量（单位球面上的点就是法线方向）
            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;
            
            // 纹理坐标
            float u = (float)seg / segments;
            float v = (float)ring / rings;
            
            shape.vertices.insert(shape.vertices.end(), {
                x, y, z, nx, ny, nz, u, v
            });
        }
    }
    
    // 生成索引
    for (int ring = 0; ring < rings; ring++) {
        for (int seg = 0; seg < segments; seg++) {
            unsigned int current = ring * (segments + 1) + seg;
            unsigned int next = current + segments + 1;
            
            shape.indices.insert(shape.indices.end(), {
                current, next, current + 1,
                current + 1, next, next + 1
            });
        }
    }
    
    CreateBuffers(shape);
}

void MeshGenerator::GenerateCylinder(Shape3D& shape, float radius, float height, int segments) {
    shape.type = SHAPE3D_CYLINDER;
    shape.vertices.clear();
    shape.indices.clear();
    
    float halfHeight = height / 2.0f;
    
    // 侧面顶点
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * (float)M_PI * i / segments;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        float nx = cosf(theta);
        float nz = sinf(theta);
        float u = (float)i / segments;
        
        // 底部顶点
        shape.vertices.insert(shape.vertices.end(), {
            x, -halfHeight, z, nx, 0.0f, nz, u, 0.0f
        });
        
        // 顶部顶点
        shape.vertices.insert(shape.vertices.end(), {
            x, halfHeight, z, nx, 0.0f, nz, u, 1.0f
        });
    }
    
    // 侧面索引
    for (int i = 0; i < segments; i++) {
        unsigned int base = i * 2;
        shape.indices.insert(shape.indices.end(), {
            base, base + 2, base + 1,
            base + 1, base + 2, base + 3
        });
    }
    
    // 顶面中心点
    unsigned int topCenterIndex = (unsigned int)shape.vertices.size() / 8;
    shape.vertices.insert(shape.vertices.end(), {
        0.0f, halfHeight, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f
    });
    
    // 顶面边缘顶点
    unsigned int topEdgeStart = (unsigned int)shape.vertices.size() / 8;
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * (float)M_PI * i / segments;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        float u = 0.5f + 0.5f * cosf(theta);
        float v = 0.5f + 0.5f * sinf(theta);
        
        shape.vertices.insert(shape.vertices.end(), {
            x, halfHeight, z, 0.0f, 1.0f, 0.0f, u, v
        });
    }
    
    // 顶面索引
    for (int i = 0; i < segments; i++) {
        shape.indices.insert(shape.indices.end(), {
            topCenterIndex, topEdgeStart + i, topEdgeStart + i + 1
        });
    }
    
    // 底面中心点
    unsigned int bottomCenterIndex = (unsigned int)shape.vertices.size() / 8;
    shape.vertices.insert(shape.vertices.end(), {
        0.0f, -halfHeight, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f
    });
    
    // 底面边缘顶点
    unsigned int bottomEdgeStart = (unsigned int)shape.vertices.size() / 8;
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * (float)M_PI * i / segments;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        float u = 0.5f + 0.5f * cosf(theta);
        float v = 0.5f + 0.5f * sinf(theta);
        
        shape.vertices.insert(shape.vertices.end(), {
            x, -halfHeight, z, 0.0f, -1.0f, 0.0f, u, v
        });
    }
    
    // 底面索引（注意缠绕顺序相反）
    for (int i = 0; i < segments; i++) {
        shape.indices.insert(shape.indices.end(), {
            bottomCenterIndex, bottomEdgeStart + i + 1, bottomEdgeStart + i
        });
    }
    
    CreateBuffers(shape);
}

void MeshGenerator::GeneratePlane(Shape3D& shape, float width, float height) {
    shape.type = SHAPE3D_PLANE;
    shape.vertices.clear();
    shape.indices.clear();
    
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    
    // 顶点格式: x, y, z, nx, ny, nz, u, v
    // 平面位于XZ平面，法线指向Y轴正方向
    shape.vertices = {
        -halfWidth, 0.0f, -halfHeight,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         halfWidth, 0.0f, -halfHeight,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         halfWidth, 0.0f,  halfHeight,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -halfWidth, 0.0f,  halfHeight,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
    };
    
    shape.indices = {
        0, 1, 2,
        0, 2, 3
    };
    
    CreateBuffers(shape);
}

void MeshGenerator::CreateBuffers(Shape3D& shape) {
    // 检查OpenGL函数是否可用
    if (!glGenVertexArrays || !glBindVertexArray || !glGenBuffers || 
        !glBindBuffer || !glBufferData || !glVertexAttribPointer || 
        !glEnableVertexAttribArray) {
        return;
    }
    
    // 删除旧的缓冲对象（如果存在）
    if (shape.VAO != 0 && glDeleteVertexArrays) {
        glDeleteVertexArrays(1, &shape.VAO);
        shape.VAO = 0;
    }
    if (shape.VBO != 0 && glDeleteBuffers) {
        glDeleteBuffers(1, &shape.VBO);
        shape.VBO = 0;
    }
    if (shape.EBO != 0 && glDeleteBuffers) {
        glDeleteBuffers(1, &shape.EBO);
        shape.EBO = 0;
    }
    
    // 生成VAO
    glGenVertexArrays(1, &shape.VAO);
    glBindVertexArray(shape.VAO);
    
    // 生成VBO并上传顶点数据
    glGenBuffers(1, &shape.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    glBufferData(GL_ARRAY_BUFFER, shape.vertices.size() * sizeof(float), 
                 shape.vertices.data(), GL_STATIC_DRAW);
    
    // 生成EBO并上传索引数据
    glGenBuffers(1, &shape.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indices.size() * sizeof(unsigned int), 
                 shape.indices.data(), GL_STATIC_DRAW);
    
    // 设置顶点属性指针
    // 顶点格式: [x, y, z, nx, ny, nz, u, v] = 8 floats = 32 bytes stride
    
    // 位置属性 (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 法线属性 (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // 纹理坐标属性 (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // 解绑VAO
    glBindVertexArray(0);
}