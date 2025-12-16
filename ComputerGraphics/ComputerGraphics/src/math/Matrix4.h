/**
 * @dir math
 * @brief 数学工具库目录
 * 
 * 本目录包含图形学相关的数学运算工具，为3D图形变换提供数学支持。
 * 
 * 目录内容：
 * - Matrix4.h - 4x4矩阵运算类，支持透视投影、视图变换、模型变换
 * 
 * 使用说明：
 * Matrix4 类是3D图形渲染的核心数学工具，用于实现MVP（Model-View-Projection）
 * 变换流水线。所有3D图形的位置、旋转、缩放以及摄像机视角都通过矩阵运算实现。
 */

/**
 * @file Matrix4.h
 * @brief 4x4矩阵运算类 - 用于3D图形变换
 * 
 * 本模块提供3D图形学中常用的矩阵运算功能，包括：
 * - 透视投影矩阵（Perspective Projection）
 * - 视图矩阵（View Matrix / LookAt）
 * - 模型变换矩阵（平移、旋转、缩放）
 * - 矩阵乘法
 * 
 * 矩阵存储格式：
 * 采用列主序（Column-Major Order）存储，与OpenGL兼容。
 * 矩阵元素排列如下：
 * 
 *     | m[0]  m[4]  m[8]   m[12] |
 * M = | m[1]  m[5]  m[9]   m[13] |
 *     | m[2]  m[6]  m[10]  m[14] |
 *     | m[3]  m[7]  m[11]  m[15] |
 * 
 * 其中 m[12], m[13], m[14] 是平移分量。
 */

#ifndef MATRIX4_H
#define MATRIX4_H

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @struct Matrix4
 * @brief 4x4矩阵结构体
 * 
 * 用于表示3D空间中的变换矩阵，支持MVP（Model-View-Projection）变换。
 * 
 * MVP变换流程：
 * 1. 模型矩阵（Model）：将物体从局部坐标系变换到世界坐标系
 * 2. 视图矩阵（View）：将世界坐标系变换到摄像机坐标系
 * 3. 投影矩阵（Projection）：将3D坐标投影到2D屏幕
 * 
 * 最终变换：gl_Position = Projection * View * Model * vertex
 */
struct Matrix4 {
    float m[16];  ///< 列主序存储的4x4矩阵数据
    
    /**
     * @brief 默认构造函数 - 创建单位矩阵
     * 
     * 单位矩阵（Identity Matrix）：
     *     | 1  0  0  0 |
     * I = | 0  1  0  0 |
     *     | 0  0  1  0 |
     *     | 0  0  0  1 |
     * 
     * 任何矩阵与单位矩阵相乘，结果不变：M * I = M
     */
    Matrix4() {
        // 初始化所有元素为0
        for (int i = 0; i < 16; i++) m[i] = 0.0f;
        // 设置对角线元素为1，形成单位矩阵
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    /**
     * @brief 创建透视投影矩阵
     * 
     * 透视投影模拟人眼观察效果，远处物体看起来更小。
     * 
     * 透视投影矩阵公式：
     *     | f/aspect  0     0                              0  |
     * P = | 0         f     0                              0  |
     *     | 0         0     (far+near)/(near-far)         -1  |
     *     | 0         0     (2*far*near)/(near-far)        0  |
     * 
     * 其中 f = 1/tan(fov/2)，fov是垂直视场角
     * 
     * @param fov 垂直视场角（Field of View），单位：弧度
     *            常用值：45度 = π/4 ≈ 0.785弧度
     * @param aspect 宽高比（Aspect Ratio）= 屏幕宽度/屏幕高度
     * @param nearPlane 近裁剪面距离，物体比这更近则不渲染
     * @param farPlane 远裁剪面距离，物体比这更远则不渲染
     * @return 透视投影矩阵
     * 
     * 注意：near和far必须为正数，且near < far
     */
    static Matrix4 perspective(float fov, float aspect, float nearPlane, float farPlane) {
        Matrix4 result;
        
        // f = cot(fov/2) = 1/tan(fov/2)
        // 控制视场角大小，f越大视场角越小
        float f = 1.0f / tanf(fov * 0.5f);
        
        // 第一列：X轴缩放，考虑宽高比
        result.m[0] = f / aspect;
        
        // 第二列：Y轴缩放
        result.m[5] = f;
        
        // 第三列：Z轴变换，用于深度计算
        result.m[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
        result.m[11] = -1.0f;  // 透视除法标志
        
        // 第四列：Z轴平移分量
        result.m[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
        result.m[15] = 0.0f;
        
        return result;
    }
    
    /**
     * @brief 创建视图矩阵（LookAt矩阵）
     * 
     * 视图矩阵将世界坐标系变换到摄像机坐标系。
     * 摄像机坐标系定义：
     * - 摄像机位于原点
     * - 摄像机看向-Z方向
     * - Y轴向上
     * 
     * 算法步骤：
     * 1. 计算前向向量（Forward）：从摄像机指向目标点
     * 2. 计算右向量（Right）：前向向量与上向量的叉积
     * 3. 计算真正的上向量（Up）：右向量与前向向量的叉积
     * 4. 构建旋转矩阵和平移矩阵的组合
     * 
     * @param eyeX, eyeY, eyeZ 摄像机位置（世界坐标）
     * @param centerX, centerY, centerZ 目标点位置（摄像机看向的点）
     * @param upX, upY, upZ 上方向向量（通常为(0,1,0)）
     * @return 视图矩阵
     */
    static Matrix4 lookAt(float eyeX, float eyeY, float eyeZ,
                         float centerX, float centerY, float centerZ,
                         float upX, float upY, float upZ) {
        Matrix4 result;
        
        // ========== 步骤1：计算前向向量（Forward Vector）==========
        // 从摄像机位置指向目标点的方向
        float fx = centerX - eyeX;
        float fy = centerY - eyeY;
        float fz = centerZ - eyeZ;
        
        // 归一化前向向量
        float flen = sqrtf(fx*fx + fy*fy + fz*fz);
        fx /= flen; fy /= flen; fz /= flen;
        
        // ========== 步骤2：计算右向量（Right Vector）==========
        // 右向量 = 前向向量 × 上向量（叉积）
        // 叉积公式：a × b = (ay*bz - az*by, az*bx - ax*bz, ax*by - ay*bx)
        float rx = fy * upZ - fz * upY;
        float ry = fz * upX - fx * upZ;
        float rz = fx * upY - fy * upX;
        
        // 归一化右向量
        float rlen = sqrtf(rx*rx + ry*ry + rz*rz);
        rx /= rlen; ry /= rlen; rz /= rlen;
        
        // ========== 步骤3：计算真正的上向量（Up Vector）==========
        // 上向量 = 右向量 × 前向向量
        // 这确保三个向量相互垂直
        float ux = ry * fz - rz * fy;
        float uy = rz * fx - rx * fz;
        float uz = rx * fy - ry * fx;
        
        // ========== 步骤4：构建视图矩阵 ==========
        // 视图矩阵 = 旋转矩阵 * 平移矩阵
        // 旋转部分：将世界坐标轴对齐到摄像机坐标轴
        // 平移部分：将摄像机移动到原点
        
        // 第一行：右向量（X轴）
        result.m[0] = rx; result.m[4] = ux; result.m[8] = -fx;
        // 第二行：上向量（Y轴）
        result.m[1] = ry; result.m[5] = uy; result.m[9] = -fy;
        // 第三行：负前向向量（Z轴，OpenGL看向-Z）
        result.m[2] = rz; result.m[6] = uz; result.m[10] = -fz;
        // 第四行：齐次坐标
        result.m[3] = 0; result.m[7] = 0; result.m[11] = 0;
        
        // 平移分量：将摄像机位置变换到原点
        // 使用点积计算在各轴上的投影
        result.m[12] = -(rx*eyeX + ux*eyeY - fx*eyeZ);
        result.m[13] = -(ry*eyeX + uy*eyeY - fy*eyeZ);
        result.m[14] = -(rz*eyeX + uz*eyeY - fz*eyeZ);
        result.m[15] = 1;
        
        return result;
    }

    /**
     * @brief 创建平移矩阵
     * 
     * 平移矩阵将点沿指定方向移动。
     * 
     * 平移矩阵公式：
     *     | 1  0  0  tx |
     * T = | 0  1  0  ty |
     *     | 0  0  1  tz |
     *     | 0  0  0  1  |
     * 
     * 变换效果：
     * | x' |   | 1  0  0  tx |   | x |   | x + tx |
     * | y' | = | 0  1  0  ty | * | y | = | y + ty |
     * | z' |   | 0  0  1  tz |   | z |   | z + tz |
     * | 1  |   | 0  0  0  1  |   | 1 |   | 1      |
     * 
     * @param x X轴平移量
     * @param y Y轴平移量
     * @param z Z轴平移量
     * @return 平移矩阵
     */
    static Matrix4 translate(float x, float y, float z) {
        Matrix4 result;  // 从单位矩阵开始
        // 设置平移分量（第四列的前三个元素）
        result.m[12] = x;
        result.m[13] = y;
        result.m[14] = z;
        return result;
    }
    
    /**
     * @brief 创建绕X轴旋转矩阵
     * 
     * 绕X轴旋转时，X坐标不变，Y和Z坐标按角度旋转。
     * 
     * 旋转矩阵公式（右手坐标系，逆时针为正）：
     *      | 1    0       0    0 |
     * Rx = | 0   cos(θ)  sin(θ) 0 |
     *      | 0  -sin(θ)  cos(θ) 0 |
     *      | 0    0       0    1 |
     * 
     * @param angle 旋转角度，单位：弧度
     *              正值表示从Y轴向Z轴方向旋转（右手定则）
     * @return 绕X轴旋转矩阵
     */
    static Matrix4 rotateX(float angle) {
        Matrix4 result;
        float c = cosf(angle);  // cos(θ)
        float s = sinf(angle);  // sin(θ)
        
        // Y-Z平面内的旋转
        result.m[5] = c;   // cos(θ)
        result.m[6] = s;   // sin(θ)
        result.m[9] = -s;  // -sin(θ)
        result.m[10] = c;  // cos(θ)
        
        return result;
    }
    
    /**
     * @brief 创建绕Y轴旋转矩阵
     * 
     * 绕Y轴旋转时，Y坐标不变，X和Z坐标按角度旋转。
     * 
     * 旋转矩阵公式（右手坐标系，逆时针为正）：
     *      | cos(θ)  0  -sin(θ)  0 |
     * Ry = | 0       1    0      0 |
     *      | sin(θ)  0   cos(θ)  0 |
     *      | 0       0    0      1 |
     * 
     * @param angle 旋转角度，单位：弧度
     *              正值表示从Z轴向X轴方向旋转（右手定则）
     * @return 绕Y轴旋转矩阵
     */
    static Matrix4 rotateY(float angle) {
        Matrix4 result;
        float c = cosf(angle);  // cos(θ)
        float s = sinf(angle);  // sin(θ)
        
        // X-Z平面内的旋转
        result.m[0] = c;   // cos(θ)
        result.m[2] = -s;  // -sin(θ)
        result.m[8] = s;   // sin(θ)
        result.m[10] = c;  // cos(θ)
        
        return result;
    }
    
    /**
     * @brief 创建绕Z轴旋转矩阵
     * 
     * 绕Z轴旋转时，Z坐标不变，X和Y坐标按角度旋转。
     * 
     * 旋转矩阵公式（右手坐标系，逆时针为正）：
     *      | cos(θ)  sin(θ)  0  0 |
     * Rz = |-sin(θ)  cos(θ)  0  0 |
     *      | 0        0      1  0 |
     *      | 0        0      0  1 |
     * 
     * @param angle 旋转角度，单位：弧度
     *              正值表示从X轴向Y轴方向旋转（右手定则）
     * @return 绕Z轴旋转矩阵
     */
    static Matrix4 rotateZ(float angle) {
        Matrix4 result;
        float c = cosf(angle);  // cos(θ)
        float s = sinf(angle);  // sin(θ)
        
        // X-Y平面内的旋转
        result.m[0] = c;   // cos(θ)
        result.m[1] = s;   // sin(θ)
        result.m[4] = -s;  // -sin(θ)
        result.m[5] = c;   // cos(θ)
        
        return result;
    }
    
    /**
     * @brief 创建缩放矩阵
     * 
     * 缩放矩阵沿各轴方向缩放物体。
     * 
     * 缩放矩阵公式：
     *     | sx  0   0   0 |
     * S = | 0   sy  0   0 |
     *     | 0   0   sz  0 |
     *     | 0   0   0   1 |
     * 
     * 变换效果：
     * | x' |   | sx  0   0   0 |   | x |   | x * sx |
     * | y' | = | 0   sy  0   0 | * | y | = | y * sy |
     * | z' |   | 0   0   sz  0 |   | z |   | z * sz |
     * | 1  |   | 0   0   0   1 |   | 1 |   | 1      |
     * 
     * @param x X轴缩放因子（1.0表示不缩放）
     * @param y Y轴缩放因子
     * @param z Z轴缩放因子
     * @return 缩放矩阵
     * 
     * 注意：
     * - 缩放因子 > 1：放大
     * - 缩放因子 = 1：不变
     * - 0 < 缩放因子 < 1：缩小
     * - 缩放因子 < 0：镜像翻转
     */
    static Matrix4 scale(float x, float y, float z) {
        Matrix4 result;
        // 设置对角线元素为缩放因子
        result.m[0] = x;   // X轴缩放
        result.m[5] = y;   // Y轴缩放
        result.m[10] = z;  // Z轴缩放
        return result;
    }

    /**
     * @brief 矩阵乘法
     * 
     * 计算 this * other，用于组合多个变换。
     * 
     * 矩阵乘法规则：
     * C[i][j] = Σ(k=0 to 3) A[i][k] * B[k][j]
     * 
     * 变换组合顺序（重要！）：
     * 由于矩阵乘法不满足交换律，变换的应用顺序很重要。
     * 在OpenGL中，变换从右向左应用：
     * 
     * 例如：Model = Translate * Rotate * Scale
     * 实际应用顺序：先缩放 → 再旋转 → 最后平移
     * 
     * 完整MVP变换：
     * gl_Position = Projection * View * Model * vertex
     * 应用顺序：Model → View → Projection
     * 
     * @param other 右操作数矩阵
     * @return 乘法结果矩阵
     * 
     * 时间复杂度：O(64) = O(1)，4x4矩阵乘法需要64次乘法和48次加法
     */
    Matrix4 multiply(const Matrix4& other) const {
        Matrix4 result;
        
        // 遍历结果矩阵的每个元素
        for (int i = 0; i < 4; i++) {          // 行
            for (int j = 0; j < 4; j++) {      // 列
                result.m[i*4 + j] = 0;
                // 计算第i行与第j列的点积
                for (int k = 0; k < 4; k++) {
                    result.m[i*4 + j] += m[i*4 + k] * other.m[k*4 + j];
                }
            }
        }
        
        return result;
    }
    
    /**
     * @brief 运算符重载：矩阵乘法
     * 
     * 提供更直观的矩阵乘法语法：
     * Matrix4 result = A * B;  // 等价于 A.multiply(B)
     * 
     * @param other 右操作数矩阵
     * @return 乘法结果矩阵
     */
    Matrix4 operator*(const Matrix4& other) const {
        return multiply(other);
    }
};

#endif // MATRIX4_H
