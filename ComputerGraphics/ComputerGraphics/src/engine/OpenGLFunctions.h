/**
 * @file OpenGLFunctions.h
 * @brief OpenGL函数指针声明
 * @author ln1.opensource@gmail.com
 * 
 * 本文件定义了OpenGL扩展函数的类型和函数指针声明。
 * 由于Windows默认只支持OpenGL 1.1，更高版本的函数需要动态加载。
 * 
 * 包含内容：
 * 1. OpenGL常量定义（缓冲区类型、着色器类型等）
 * 2. OpenGL类型定义（GLuint、GLfloat等）
 * 3. OpenGL函数指针类型定义
 * 4. 外部函数指针声明（在GraphicsEngine3D_Core.cpp中定义）
 */

#pragma once
#include <windows.h>

// ============================================================================
// OpenGL常量和类型定义
// ============================================================================

#ifndef GL_ARRAY_BUFFER
// 缓冲区类型
#define GL_ARRAY_BUFFER 0x8892           ///< 顶点属性缓冲区
#define GL_ELEMENT_ARRAY_BUFFER 0x8893   ///< 索引缓冲区
#define GL_STATIC_DRAW 0x88E4            ///< 静态绑定数据（不常修改）

// 图元类型
#define GL_TRIANGLES 0x0004              ///< 三角形图元

// 数据类型
#define GL_UNSIGNED_INT 0x1405           ///< 无符号整数
#define GL_FLOAT 0x1406                  ///< 浮点数
#define GL_FALSE 0                       ///< 布尔假值

// 纹理相关
#define GL_TEXTURE0 0x84C0               ///< 纹理单元0
#define GL_TEXTURE_2D 0x0DE1             ///< 2D纹理

// 着色器类型
#define GL_VERTEX_SHADER 0x8B31          ///< 顶点着色器
#define GL_FRAGMENT_SHADER 0x8B30        ///< 片段着色器

// 着色器状态查询
#define GL_COMPILE_STATUS 0x8B81         ///< 编译状态
#define GL_LINK_STATUS 0x8B82            ///< 链接状态
#define GL_INFO_LOG_LENGTH 0x8B84        ///< 信息日志长度

// 数学常量
#ifndef M_PI
#define M_PI 3.14159265358979323846      ///< 圆周率
#endif

// OpenGL基本类型定义
typedef int GLsizei;                     ///< 大小类型
typedef unsigned int GLuint;             ///< 无符号整数类型
typedef int GLint;                       ///< 有符号整数类型
typedef unsigned char GLboolean;         ///< 布尔类型
typedef float GLfloat;                   ///< 浮点类型
typedef char GLchar;                     ///< 字符类型
typedef ptrdiff_t GLsizeiptr;            ///< 指针大小类型
typedef unsigned int GLenum;             ///< 枚举类型
#endif

// ============================================================================
// OpenGL函数指针类型定义
// ============================================================================

/// 生成顶点数组对象
typedef void (APIENTRY *PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
/// 绑定顶点数组对象
typedef void (APIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
/// 生成缓冲区对象
typedef void (APIENTRY *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
/// 绑定缓冲区对象
typedef void (APIENTRY *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
/// 上传缓冲区数据
typedef void (APIENTRY *PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
/// 设置顶点属性指针
typedef void (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
/// 启用顶点属性数组
typedef void (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
/// 删除顶点数组对象
typedef void (APIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
/// 删除缓冲区对象
typedef void (APIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);

// ============================================================================
// 外部函数指针声明（在GraphicsEngine3D_Core.cpp中定义）
// ============================================================================

extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;           ///< 生成VAO
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;           ///< 绑定VAO
extern PFNGLGENBUFFERSPROC glGenBuffers;                     ///< 生成VBO
extern PFNGLBINDBUFFERPROC glBindBuffer;                     ///< 绑定VBO
extern PFNGLBUFFERDATAPROC glBufferData;                     ///< 上传数据
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;   ///< 设置顶点属性
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray; ///< 启用顶点属性
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;     ///< 删除VAO
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;               ///< 删除VBO