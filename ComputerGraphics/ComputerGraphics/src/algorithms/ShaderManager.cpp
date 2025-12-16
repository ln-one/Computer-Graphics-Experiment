/**
 * @file ShaderManager.cpp
 * @brief 着色器管理器实现
 * @author 计算机图形学项目组
 * 
 * 本文件实现了OpenGL着色器的编译、链接和管理功能。
 * 
 * 【着色器概述】
 * 着色器（Shader）是运行在GPU上的小程序，用于控制图形渲染管线的各个阶段。
 * 现代OpenGL使用可编程管线，主要包含两种着色器：
 * 
 * 1. 顶点着色器（Vertex Shader）：
 *    - 处理每个顶点的数据
 *    - 主要任务：坐标变换（模型→世界→视图→裁剪空间）
 *    - 输入：顶点属性（位置、法线、纹理坐标等）
 *    - 输出：变换后的顶点位置和传递给片段着色器的数据
 * 
 * 2. 片段着色器（Fragment Shader）：
 *    - 处理每个像素（片段）的颜色
 *    - 主要任务：光照计算、纹理采样、颜色混合
 *    - 输入：从顶点着色器插值得到的数据
 *    - 输出：最终的像素颜色
 * 
 * 【着色器程序创建流程】
 * 1. 创建着色器对象 (glCreateShader)
 * 2. 设置着色器源代码 (glShaderSource)
 * 3. 编译着色器 (glCompileShader)
 * 4. 检查编译错误 (glGetShaderiv, glGetShaderInfoLog)
 * 5. 创建程序对象 (glCreateProgram)
 * 6. 附加着色器到程序 (glAttachShader)
 * 7. 链接程序 (glLinkProgram)
 * 8. 检查链接错误 (glGetProgramiv, glGetProgramInfoLog)
 * 9. 删除着色器对象（已链接到程序中）
 */

#include "ShaderManager.h"
#include <windows.h>
#include <iostream>
#include <sstream>

// ============================================================
// OpenGL常量和类型定义
// ============================================================
// 这些常量在OpenGL头文件中定义，这里为了独立性重新定义

#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31      // 顶点着色器类型标识
#define GL_FRAGMENT_SHADER 0x8B30    // 片段着色器类型标识
#define GL_COMPILE_STATUS 0x8B81     // 编译状态查询标识
#define GL_LINK_STATUS 0x8B82        // 链接状态查询标识
#define GL_INFO_LOG_LENGTH 0x8B84    // 信息日志长度查询标识

// OpenGL基本类型定义
typedef int GLsizei;
typedef unsigned int GLuint;
typedef int GLint;
typedef unsigned char GLboolean;
typedef float GLfloat;
typedef char GLchar;
typedef unsigned int GLenum;
#endif

// ============================================================
// OpenGL函数指针类型定义
// ============================================================
// 现代OpenGL函数需要在运行时动态加载

typedef GLuint (APIENTRY *PFNGLCREATESHADERPROC)(GLenum type);
typedef void (APIENTRY *PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (APIENTRY *PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (APIENTRY *PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef GLuint (APIENTRY *PFNGLCREATEPROGRAMPROC)(void);
typedef void (APIENTRY *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (APIENTRY *PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (APIENTRY *PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRY *PFNGLDELETESHADERPROC)(GLuint shader);

// ============================================================
// 全局函数指针
// ============================================================
// 这些指针在LoadShaderFunctions()中初始化

static PFNGLCREATESHADERPROC glCreateShader = nullptr;       // 创建着色器对象
static PFNGLSHADERSOURCEPROC glShaderSource = nullptr;       // 设置着色器源代码
static PFNGLCOMPILESHADERPROC glCompileShader = nullptr;     // 编译着色器
static PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;         // 获取着色器参数
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr; // 获取着色器日志
static PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;     // 创建程序对象
static PFNGLATTACHSHADERPROC glAttachShader = nullptr;       // 附加着色器到程序
static PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;         // 链接程序
static PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;       // 获取程序参数
static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr; // 获取程序日志
static PFNGLDELETESHADERPROC glDeleteShader = nullptr;       // 删除着色器对象

/**
 * @brief 加载OpenGL着色器相关函数
 * @return 是否成功加载所有函数
 * 
 * 【动态加载原理】
 * OpenGL 1.1之后的函数不在opengl32.dll中直接导出，
 * 需要通过wglGetProcAddress在运行时获取函数指针。
 * 这是因为不同的显卡驱动实现了不同版本的OpenGL。
 */
static bool LoadShaderFunctions() {
    // 使用静态变量确保只加载一次
    static bool loaded = false;
    if (loaded) return true;
    
    // 通过wglGetProcAddress获取各个函数的地址
    glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
    glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
    
    // 检查所有函数是否都成功加载
    loaded = (glCreateShader && glShaderSource && glCompileShader && 
              glGetShaderiv && glGetShaderInfoLog && glCreateProgram && 
              glAttachShader && glLinkProgram && glGetProgramiv && 
              glGetProgramInfoLog && glDeleteShader);
    
    return loaded;
}

/**
 * @brief 创建着色器程序
 * @param vertexSource 顶点着色器源代码（GLSL）
 * @param fragmentSource 片段着色器源代码（GLSL）
 * @return 着色器程序ID，失败返回0
 * 
 * 【创建流程】
 * 1. 加载OpenGL函数
 * 2. 编译顶点着色器
 * 3. 编译片段着色器
 * 4. 创建程序对象
 * 5. 附加着色器并链接
 * 6. 清理着色器对象
 */
unsigned int ShaderManager::CreateShaderProgram(const char* vertexSource, const char* fragmentSource) {
    // 首先加载OpenGL函数
    if (!LoadShaderFunctions()) {
        MessageBoxA(NULL, "Failed to load OpenGL shader functions", "Shader Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    
    // ========== 编译顶点着色器 ==========
    unsigned int vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        return 0;  // 编译失败
    }
    
    // ========== 编译片段着色器 ==========
    unsigned int fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);  // 清理已创建的顶点着色器
        return 0;
    }
    
    // ========== 创建并链接着色器程序 ==========
    unsigned int shaderProgram = glCreateProgram();
    
    // 将编译好的着色器附加到程序
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    
    // 链接程序：将所有着色器组合成一个完整的渲染管线
    glLinkProgram(shaderProgram);
    
    // ========== 检查链接错误 ==========
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        
        std::string msg = "着色器程序链接失败:\n" + std::string(infoLog);
        MessageBoxA(NULL, msg.c_str(), "着色器链接错误", MB_OK | MB_ICONERROR);
        
        // 清理资源
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }
    
    // ========== 清理着色器对象 ==========
    // 着色器已经链接到程序中，不再需要单独的着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

/**
 * @brief 编译单个着色器
 * @param source 着色器源代码
 * @param type 着色器类型（GL_VERTEX_SHADER或GL_FRAGMENT_SHADER）
 * @return 着色器ID，失败返回0
 * 
 * 【编译流程】
 * 1. 创建着色器对象
 * 2. 设置源代码
 * 3. 编译
 * 4. 检查编译错误
 */
unsigned int ShaderManager::CompileShader(const char* source, unsigned int type) {
    // 创建着色器对象
    unsigned int shader = glCreateShader(type);
    
    // 设置着色器源代码
    // 参数：着色器ID，字符串数量，字符串数组，长度数组（NULL表示以\0结尾）
    glShaderSource(shader, 1, &source, NULL);
    
    // 编译着色器
    glCompileShader(shader);
    
    // 检查编译错误
    std::string typeStr = (type == GL_VERTEX_SHADER) ? "顶点着色器" : "片段着色器";
    if (!CheckCompileErrors(shader, typeStr)) {
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

/**
 * @brief 检查着色器编译错误
 * @param shader 着色器ID
 * @param type 着色器类型描述（用于错误信息）
 * @return 是否编译成功
 */
bool ShaderManager::CheckCompileErrors(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[512];
    
    // 查询编译状态
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        // 获取错误日志
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        
        std::string msg = "着色器编译失败 (" + type + "):\n" + std::string(infoLog);
        MessageBoxA(NULL, msg.c_str(), "着色器编译错误", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

/**
 * @brief 获取默认顶点着色器源代码
 * @return GLSL顶点着色器代码
 * 
 * 【顶点着色器功能】
 * 1. 接收顶点属性（位置、法线、纹理坐标）
 * 2. 将顶点位置从模型空间变换到裁剪空间
 * 3. 将法线变换到世界空间（用于光照计算）
 * 4. 传递数据给片段着色器
 * 
 * 【坐标变换流程】
 * 模型空间 → (Model矩阵) → 世界空间 → (View矩阵) → 视图空间 → (Projection矩阵) → 裁剪空间
 * 
 * 【GLSL变量类型】
 * - attribute：顶点属性输入（位置、法线等）
 * - varying：传递给片段着色器的插值变量
 * - uniform：全局统一变量（矩阵、光照参数等）
 */
const char* ShaderManager::GetDefaultVertexShader() {
    return R"(
        #version 120
        
        // ========== 顶点属性输入 ==========
        attribute vec3 aPos;       // 顶点位置（模型空间）
        attribute vec3 aNormal;    // 顶点法线（模型空间）
        attribute vec2 aTexCoord;  // 纹理坐标
        
        // ========== 传递给片段着色器的变量 ==========
        varying vec3 FragPos;      // 片段位置（世界空间）
        varying vec3 Normal;       // 法线向量（世界空间）
        varying vec2 TexCoord;     // 纹理坐标
        
        // ========== 变换矩阵（统一变量） ==========
        uniform mat4 model;        // 模型矩阵：模型空间 → 世界空间
        uniform mat4 view;         // 视图矩阵：世界空间 → 视图空间
        uniform mat4 projection;   // 投影矩阵：视图空间 → 裁剪空间
        
        void main() {
            // 将顶点位置变换到世界空间
            // FragPos用于片段着色器中的光照计算
            FragPos = vec3(model * vec4(aPos, 1.0));
            
            // 将法线变换到世界空间
            // 注意：对于非均匀缩放，应该使用模型矩阵的逆转置矩阵
            // 这里简化处理，使用mat3(model)，适用于均匀缩放
            Normal = mat3(model) * aNormal;
            
            // 直接传递纹理坐标（不需要变换）
            TexCoord = aTexCoord;
            
            // 计算最终的裁剪空间位置
            // MVP变换：Model → View → Projection
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
}

/**
 * @brief 获取默认片段着色器源代码
 * @return GLSL片段着色器代码
 * 
 * 【片段着色器功能】
 * 实现Phong光照模型，计算每个像素的最终颜色。
 * 
 * 【Phong光照模型】
 * 最终颜色 = 环境光 + 漫反射 + 镜面反射
 * 
 * 1. 环境光（Ambient）：
 *    - 模拟间接光照（光线在场景中多次反射）
 *    - 公式：ambient = Ka × Ia
 *    - Ka：材质环境光系数，Ia：环境光强度
 * 
 * 2. 漫反射（Diffuse）：
 *    - 模拟粗糙表面的光照（光线均匀散射）
 *    - 依赖于光线方向和表面法线的夹角
 *    - 公式：diffuse = Kd × Id × max(N·L, 0)
 *    - Kd：材质漫反射系数，Id：漫反射光强度
 *    - N：表面法线，L：光线方向
 * 
 * 3. 镜面反射（Specular）：
 *    - 模拟光滑表面的高光
 *    - 依赖于视线方向和反射方向的夹角
 *    - 公式：specular = Ks × Is × max(R·V, 0)^n
 *    - Ks：材质镜面反射系数，Is：镜面反射光强度
 *    - R：反射方向，V：视线方向，n：光泽度
 */
const char* ShaderManager::GetDefaultFragmentShader() {
    return R"(
        #version 120
        
        // ========== 从顶点着色器接收的插值变量 ==========
        varying vec3 FragPos;      // 片段位置（世界空间）
        varying vec3 Normal;       // 法线向量（世界空间）
        varying vec2 TexCoord;     // 纹理坐标
        
        // ========== 光源属性 ==========
        uniform vec3 lightPos;              // 光源位置（世界空间）
        uniform vec3 lightColor;            // 光源颜色
        uniform float lightAmbientIntensity;  // 环境光强度
        uniform float lightDiffuseIntensity;  // 漫反射强度
        uniform float lightSpecularIntensity; // 镜面反射强度
        
        // ========== 相机位置（用于镜面反射计算） ==========
        uniform vec3 viewPos;               // 相机位置（世界空间）
        
        // ========== 材质属性 ==========
        uniform vec3 ambient;               // 环境光反射系数
        uniform vec3 diffuse;               // 漫反射系数
        uniform vec3 specular;              // 镜面反射系数
        uniform float shininess;            // 光泽度（高光锐度）
        
        // ========== 纹理 ==========
        uniform bool useTexture;            // 是否使用纹理
        uniform sampler2D textureSampler;   // 纹理采样器
        
        void main() {
            // ========================================
            // Phong光照模型（冯氏光照模型）
            // ========================================
            
            // 归一化法线向量（插值后可能不是单位向量）
            vec3 norm = normalize(Normal);
            
            // 计算光线方向（从片段指向光源）
            vec3 lightDir = normalize(lightPos - FragPos);
            
            // 计算视线方向（从片段指向相机）
            vec3 viewDir = normalize(viewPos - FragPos);
            
            // 计算反射方向（用于镜面反射）
            // reflect函数：计算入射光线关于法线的反射方向
            vec3 reflectDir = reflect(-lightDir, norm);
            
            // ----------------------------------------
            // 1. 环境光分量（Ambient Component）
            // 模拟场景中的间接光照
            // ----------------------------------------
            vec3 ambientColor = ambient * lightColor * lightAmbientIntensity;
            
            // ----------------------------------------
            // 2. 漫反射分量（Diffuse Component）
            // 光线在粗糙表面均匀散射
            // 强度取决于光线与法线的夹角
            // ----------------------------------------
            // dot(norm, lightDir)：计算法线和光线方向的点积
            // max(..., 0.0)：确保背面不会有负的光照
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuseColor = diffuse * diff * lightColor * lightDiffuseIntensity;
            
            // ----------------------------------------
            // 3. 镜面反射分量（Specular Component）
            // 光滑表面的高光效果
            // 强度取决于视线与反射方向的夹角
            // ----------------------------------------
            // dot(viewDir, reflectDir)：计算视线和反射方向的点积
            // pow(..., shininess)：shininess越大，高光越集中
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
            vec3 specularColor = specular * spec * lightColor * lightSpecularIntensity;
            
            // ----------------------------------------
            // 合并所有光照分量
            // ----------------------------------------
            vec3 result = ambientColor + diffuseColor + specularColor;
            
            // 如果启用纹理，将纹理颜色与光照结果相乘
            if (useTexture) {
                result *= texture2D(textureSampler, TexCoord).rgb;
            }
            
            // 输出最终颜色（alpha = 1.0，完全不透明）
            gl_FragColor = vec4(result, 1.0);
        }
    )";
}
