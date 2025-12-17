/**
 * @file MeshGenerator.cpp
 * @brief 三维网格生成算法实现
 * @author ln1.opensource@gmail.com
 * 
 * 本文件实现了四种基本三维几何体的网格生成：
 * 1. 立方体（Cube）- 6个面，24个顶点
 * 2. 球体（Sphere）- 基于经纬度参数化
 * 3. 圆柱体（Cylinder）- 侧面 + 顶面 + 底面
 * 4. 平面（Plane）- 简单的四边形
 * 
 * 【顶点数据格式】
 * 每个顶点包含8个float值，共32字节：
 * - 位置坐标 (x, y, z)：3个float，定义顶点在3D空间中的位置
 * - 法线向量 (nx, ny, nz)：3个float，用于光照计算
 * - 纹理坐标 (u, v)：2个float，用于纹理映射
 * 
 * 【索引数据】
 * 使用索引数组定义三角形面片，每3个索引构成一个三角形。
 * 索引的顺序决定了面的朝向（逆时针为正面）。
 * 
 * 【OpenGL缓冲对象】
 * - VAO（顶点数组对象）：存储顶点属性配置
 * - VBO（顶点缓冲对象）：存储顶点数据
 * - EBO（元素缓冲对象）：存储索引数据
 */

#include "MeshGenerator.h"
#include "../engine/OpenGLFunctions.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief 生成立方体网格
 * @param shape 要填充网格数据的Shape3D对象引用
 * @param size 立方体的边长
 * 
 * 【生成原理】
 * 立方体由6个正方形面组成，每个面需要4个顶点。
 * 虽然立方体只有8个角点，但由于每个面的法线方向不同，
 * 需要为每个面单独定义顶点（共24个顶点）。
 * 
 * 【面的定义】
 * - 前面：z = +halfSize，法线 (0, 0, 1)
 * - 后面：z = -halfSize，法线 (0, 0, -1)
 * - 上面：y = +halfSize，法线 (0, 1, 0)
 * - 下面：y = -halfSize，法线 (0, -1, 0)
 * - 右面：x = +halfSize，法线 (1, 0, 0)
 * - 左面：x = -halfSize，法线 (-1, 0, 0)
 * 
 * 【三角形划分】
 * 每个正方形面由2个三角形组成，共12个三角形。
 */
void MeshGenerator::GenerateCube(Shape3D& shape, float size) {
    shape.type = SHAPE3D_CUBE;
    shape.vertices.clear();
    shape.indices.clear();
    
    // 计算半边长，使立方体以原点为中心
    float halfSize = size / 2.0f;
    
    // 立方体有6个面，每个面4个顶点
    // 顶点格式: x, y, z, nx, ny, nz, u, v（共8个float）
    
    // ========== 前面 (z = +halfSize) ==========
    // 法线指向+Z方向，面向观察者
    shape.vertices.insert(shape.vertices.end(), {
        // 左下角
        -halfSize, -halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        // 右下角
         halfSize, -halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
        // 右上角
         halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        // 左上角
        -halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f
    });
    
    // ========== 后面 (z = -halfSize) ==========
    // 法线指向-Z方向，背向观察者
    shape.vertices.insert(shape.vertices.end(), {
         halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        -halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f
    });
    
    // ========== 上面 (y = +halfSize) ==========
    // 法线指向+Y方向（向上）
    shape.vertices.insert(shape.vertices.end(), {
        -halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    });
    
    // ========== 下面 (y = -halfSize) ==========
    // 法线指向-Y方向（向下）
    shape.vertices.insert(shape.vertices.end(), {
        -halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f
    });
    
    // ========== 右面 (x = +halfSize) ==========
    // 法线指向+X方向（向右）
    shape.vertices.insert(shape.vertices.end(), {
         halfSize, -halfSize,  halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         halfSize, -halfSize, -halfSize,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         halfSize,  halfSize, -halfSize,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f
    });
    
    // ========== 左面 (x = -halfSize) ==========
    // 法线指向-X方向（向左）
    shape.vertices.insert(shape.vertices.end(), {
        -halfSize, -halfSize, -halfSize, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -halfSize, -halfSize,  halfSize, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -halfSize,  halfSize,  halfSize, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -halfSize,  halfSize, -halfSize, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f
    });
    
    // ========== 生成索引 ==========
    // 每个面由2个三角形组成，顶点顺序为逆时针（正面朝外）
    // 三角形1: 0-1-2，三角形2: 0-2-3
    for (unsigned int face = 0; face < 6; face++) {
        unsigned int base = face * 4;  // 每个面4个顶点
        shape.indices.insert(shape.indices.end(), {
            base + 0, base + 1, base + 2,  // 第一个三角形
            base + 0, base + 2, base + 3   // 第二个三角形
        });
    }
    
    // 创建OpenGL缓冲对象
    CreateBuffers(shape);
}

/**
 * @brief 生成球体网格
 * @param shape 要填充网格数据的Shape3D对象引用
 * @param radius 球体半径
 * @param segments 水平分段数（经线数量，控制水平方向的精细度）
 * @param rings 垂直分段数（纬线数量，控制垂直方向的精细度）
 * 
 * 【生成原理】
 * 使用球面参数方程生成顶点：
 * - x = r × sin(φ) × cos(θ)
 * - y = r × cos(φ)
 * - z = r × sin(φ) × sin(θ)
 * 
 * 其中：
 * - φ (phi)：从北极到南极的角度，范围 [0, π]
 * - θ (theta)：绕Y轴的角度，范围 [0, 2π]
 * - r：球体半径
 * 
 * 【法线计算】
 * 球面上任意点的法线就是从球心指向该点的单位向量：
 * n = (x/r, y/r, z/r)
 * 
 * 【纹理映射】
 * 使用经纬度映射：
 * - u = θ / 2π（水平方向）
 * - v = φ / π（垂直方向）
 */
void MeshGenerator::GenerateSphere(Shape3D& shape, float radius, int segments, int rings) {
    shape.type = SHAPE3D_SPHERE;
    shape.vertices.clear();
    shape.indices.clear();
    
    // ========== 生成顶点 ==========
    // 从北极（ring=0）到南极（ring=rings）逐圈生成
    for (int ring = 0; ring <= rings; ring++) {
        // φ角：从0（北极）到π（南极）
        float phi = (float)M_PI * ring / rings;
        
        // 计算当前纬度圈的y坐标和半径
        float y = radius * cosf(phi);           // y = r × cos(φ)
        float ringRadius = radius * sinf(phi);  // 当前圈的半径 = r × sin(φ)
        
        // 沿当前纬度圈生成顶点
        for (int seg = 0; seg <= segments; seg++) {
            // θ角：从0到2π绕Y轴一圈
            float theta = 2.0f * (float)M_PI * seg / segments;
            
            // 计算顶点的x和z坐标
            float x = ringRadius * cosf(theta);  // x = ringRadius × cos(θ)
            float z = ringRadius * sinf(theta);  // z = ringRadius × sin(θ)
            
            // 法线向量：单位球面上的点就是法线方向
            // 对于半径为r的球，法线 = 位置 / r
            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;
            
            // 纹理坐标：经纬度映射
            float u = (float)seg / segments;   // 水平方向 [0, 1]
            float v = (float)ring / rings;     // 垂直方向 [0, 1]
            
            // 添加顶点数据
            shape.vertices.insert(shape.vertices.end(), {
                x, y, z, nx, ny, nz, u, v
            });
        }
    }
    
    // ========== 生成索引 ==========
    // 连接相邻两圈的顶点形成四边形，每个四边形分成2个三角形
    for (int ring = 0; ring < rings; ring++) {
        for (int seg = 0; seg < segments; seg++) {
            // 计算当前四边形的4个顶点索引
            unsigned int current = ring * (segments + 1) + seg;      // 当前圈当前点
            unsigned int next = current + segments + 1;               // 下一圈对应点
            
            // 两个三角形组成一个四边形
            // 三角形1: current -> next -> current+1
            // 三角形2: current+1 -> next -> next+1
            shape.indices.insert(shape.indices.end(), {
                current, next, current + 1,
                current + 1, next, next + 1
            });
        }
    }
    
    CreateBuffers(shape);
}

/**
 * @brief 生成圆柱体网格
 * @param shape 要填充网格数据的Shape3D对象引用
 * @param radius 圆柱体底面半径
 * @param height 圆柱体高度
 * @param segments 圆周分段数（控制圆的精细度）
 * 
 * 【生成原理】
 * 圆柱体由三部分组成：
 * 1. 侧面：一个卷曲的矩形，展开后宽度为2πr，高度为h
 * 2. 顶面：一个圆形盖子
 * 3. 底面：一个圆形盖子
 * 
 * 【侧面顶点】
 * 使用参数方程：
 * - x = r × cos(θ)
 * - z = r × sin(θ)
 * - y = ±h/2（顶部或底部）
 * 
 * 侧面法线水平向外：n = (cos(θ), 0, sin(θ))
 * 
 * 【顶面和底面】
 * 使用扇形三角形，中心点连接边缘点。
 * 顶面法线向上 (0, 1, 0)，底面法线向下 (0, -1, 0)
 */
void MeshGenerator::GenerateCylinder(Shape3D& shape, float radius, float height, int segments) {
    shape.type = SHAPE3D_CYLINDER;
    shape.vertices.clear();
    shape.indices.clear();
    
    float halfHeight = height / 2.0f;
    
    // ========== 侧面顶点 ==========
    // 每个分段生成上下两个顶点
    for (int i = 0; i <= segments; i++) {
        // θ角：绕Y轴一圈
        float theta = 2.0f * (float)M_PI * i / segments;
        
        // 计算圆周上的x和z坐标
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        
        // 侧面法线：水平向外，与位置方向相同（但归一化）
        float nx = cosf(theta);
        float nz = sinf(theta);
        
        // 纹理坐标：u沿圆周，v沿高度
        float u = (float)i / segments;
        
        // 底部顶点 (y = -halfHeight)
        shape.vertices.insert(shape.vertices.end(), {
            x, -halfHeight, z, nx, 0.0f, nz, u, 0.0f
        });
        
        // 顶部顶点 (y = +halfHeight)
        shape.vertices.insert(shape.vertices.end(), {
            x, halfHeight, z, nx, 0.0f, nz, u, 1.0f
        });
    }
    
    // ========== 侧面索引 ==========
    // 连接相邻的上下顶点对形成四边形
    for (int i = 0; i < segments; i++) {
        unsigned int base = i * 2;  // 每个分段2个顶点（上下）
        // 四边形分成2个三角形
        shape.indices.insert(shape.indices.end(), {
            base, base + 2, base + 1,      // 三角形1
            base + 1, base + 2, base + 3   // 三角形2
        });
    }
    
    // ========== 顶面 ==========
    // 顶面中心点
    unsigned int topCenterIndex = (unsigned int)shape.vertices.size() / 8;
    shape.vertices.insert(shape.vertices.end(), {
        0.0f, halfHeight, 0.0f,   // 位置：顶面中心
        0.0f, 1.0f, 0.0f,         // 法线：向上
        0.5f, 0.5f                // 纹理：中心
    });
    
    // 顶面边缘顶点（需要单独的顶点，因为法线不同）
    unsigned int topEdgeStart = (unsigned int)shape.vertices.size() / 8;
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * (float)M_PI * i / segments;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        
        // 纹理坐标：圆形映射到[0,1]×[0,1]
        float u = 0.5f + 0.5f * cosf(theta);
        float v = 0.5f + 0.5f * sinf(theta);
        
        shape.vertices.insert(shape.vertices.end(), {
            x, halfHeight, z,     // 位置
            0.0f, 1.0f, 0.0f,     // 法线：向上
            u, v                  // 纹理
        });
    }
    
    // 顶面索引：扇形三角形
    for (int i = 0; i < segments; i++) {
        shape.indices.insert(shape.indices.end(), {
            topCenterIndex, topEdgeStart + i, topEdgeStart + i + 1
        });
    }
    
    // ========== 底面 ==========
    // 底面中心点
    unsigned int bottomCenterIndex = (unsigned int)shape.vertices.size() / 8;
    shape.vertices.insert(shape.vertices.end(), {
        0.0f, -halfHeight, 0.0f,  // 位置：底面中心
        0.0f, -1.0f, 0.0f,        // 法线：向下
        0.5f, 0.5f                // 纹理：中心
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
            x, -halfHeight, z,    // 位置
            0.0f, -1.0f, 0.0f,    // 法线：向下
            u, v                  // 纹理
        });
    }
    
    // 底面索引：扇形三角形（注意缠绕顺序相反，使法线朝下）
    for (int i = 0; i < segments; i++) {
        shape.indices.insert(shape.indices.end(), {
            bottomCenterIndex, bottomEdgeStart + i + 1, bottomEdgeStart + i
        });
    }
    
    CreateBuffers(shape);
}

/**
 * @brief 生成平面网格
 * @param shape 要填充网格数据的Shape3D对象引用
 * @param width 平面宽度（X方向）
 * @param height 平面高度（Z方向）
 * 
 * 【生成原理】
 * 平面是最简单的3D图形，由4个顶点和2个三角形组成。
 * 平面位于XZ平面上（y=0），法线指向Y轴正方向。
 * 
 * 【顶点布局】
 *   3 -------- 2
 *   |          |
 *   |    +     |  （+表示原点）
 *   |          |
 *   0 -------- 1
 * 
 * 【用途】
 * - 地面
 * - 墙壁
 * - 任何需要平面的场景
 */
void MeshGenerator::GeneratePlane(Shape3D& shape, float width, float height) {
    shape.type = SHAPE3D_PLANE;
    shape.vertices.clear();
    shape.indices.clear();
    
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    
    // 顶点格式: x, y, z, nx, ny, nz, u, v
    // 平面位于XZ平面（y=0），法线指向Y轴正方向
    shape.vertices = {
        // 顶点0：左下角
        -halfWidth, 0.0f, -halfHeight,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        // 顶点1：右下角
         halfWidth, 0.0f, -halfHeight,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        // 顶点2：右上角
         halfWidth, 0.0f,  halfHeight,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        // 顶点3：左上角
        -halfWidth, 0.0f,  halfHeight,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
    };
    
    // 索引：2个三角形组成矩形
    // 三角形1: 0-1-2，三角形2: 0-2-3
    shape.indices = {
        0, 1, 2,
        0, 2, 3
    };
    
    CreateBuffers(shape);
}

/**
 * @brief 创建OpenGL缓冲对象
 * @param shape 包含顶点和索引数据的Shape3D对象引用
 * 
 * 【OpenGL缓冲对象说明】
 * 
 * VAO（Vertex Array Object，顶点数组对象）：
 * - 存储顶点属性的配置状态
 * - 记录VBO和EBO的绑定关系
 * - 记录顶点属性指针的设置
 * - 渲染时只需绑定VAO即可恢复所有配置
 * 
 * VBO（Vertex Buffer Object，顶点缓冲对象）：
 * - 存储顶点数据（位置、法线、纹理坐标等）
 * - 数据存储在GPU显存中，提高渲染效率
 * 
 * EBO（Element Buffer Object，元素缓冲对象）：
 * - 存储索引数据
 * - 允许顶点复用，减少数据量
 * 
 * 【顶点属性布局】
 * 每个顶点32字节（8个float）：
 * - location 0：位置 (x, y, z) - 偏移0，3个float
 * - location 1：法线 (nx, ny, nz) - 偏移12字节，3个float
 * - location 2：纹理坐标 (u, v) - 偏移24字节，2个float
 */
void MeshGenerator::CreateBuffers(Shape3D& shape) {
    // 检查OpenGL函数是否可用（动态加载的函数指针）
    if (!glGenVertexArrays || !glBindVertexArray || !glGenBuffers || 
        !glBindBuffer || !glBufferData || !glVertexAttribPointer || 
        !glEnableVertexAttribArray) {
        return;
    }
    
    // ========== 清理旧的缓冲对象 ==========
    // 如果之前已经创建过缓冲对象，需要先删除
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
    
    // ========== 创建VAO ==========
    // VAO记录后续的VBO绑定和顶点属性配置
    glGenVertexArrays(1, &shape.VAO);
    glBindVertexArray(shape.VAO);
    
    // ========== 创建VBO并上传顶点数据 ==========
    glGenBuffers(1, &shape.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    // GL_STATIC_DRAW：数据不会或很少改变，适合静态网格
    glBufferData(GL_ARRAY_BUFFER, shape.vertices.size() * sizeof(float), 
                 shape.vertices.data(), GL_STATIC_DRAW);
    
    // ========== 创建EBO并上传索引数据 ==========
    glGenBuffers(1, &shape.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indices.size() * sizeof(unsigned int), 
                 shape.indices.data(), GL_STATIC_DRAW);
    
    // ========== 设置顶点属性指针 ==========
    // 顶点格式: [x, y, z, nx, ny, nz, u, v]
    // 步长（stride）= 8 * sizeof(float) = 32字节
    
    // 位置属性 (location = 0)
    // 3个float，从偏移0开始
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 法线属性 (location = 1)
    // 3个float，从偏移12字节（3*sizeof(float)）开始
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // 纹理坐标属性 (location = 2)
    // 2个float，从偏移24字节（6*sizeof(float)）开始
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // ========== 解绑VAO ==========
    // 解绑后，后续的VBO/EBO操作不会影响这个VAO
    glBindVertexArray(0);
}
