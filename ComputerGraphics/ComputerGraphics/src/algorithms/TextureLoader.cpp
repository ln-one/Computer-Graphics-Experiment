/**
 * @file TextureLoader.cpp
 * @brief 纹理加载器实现
 * @author ln1.opensource@gmail.com
 * 
 * 本文件实现了纹理加载功能，主要包括：
 * 1. 使用stb_image库加载各种格式的图片文件
 * 2. 创建OpenGL纹理对象并上传数据到GPU
 * 3. 自动生成Mipmap以提高渲染质量
 * 4. 纹理资源的释放和管理
 */

#include "TextureLoader.h"
#include "../engine/OpenGLFunctions.h"
#include <windows.h>
#include <gl/GL.h>
#include <algorithm>
#include <cctype>

// 在此文件中定义STB_IMAGE_IMPLEMENTATION以包含stb_image的实现代码
// 注意：整个项目中只能在一个.cpp文件中定义此宏
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ============================================================================
// OpenGL纹理操作相关常量定义
// 这些常量在某些OpenGL头文件中可能未定义，因此在此处手动定义
// ============================================================================

#ifndef GL_TEXTURE_WRAP_S
#define GL_TEXTURE_WRAP_S 0x2802      ///< 纹理S方向（水平）环绕模式
#endif
#ifndef GL_TEXTURE_WRAP_T
#define GL_TEXTURE_WRAP_T 0x2803      ///< 纹理T方向（垂直）环绕模式
#endif
#ifndef GL_TEXTURE_MIN_FILTER
#define GL_TEXTURE_MIN_FILTER 0x2801  ///< 纹理缩小过滤模式
#endif
#ifndef GL_TEXTURE_MAG_FILTER
#define GL_TEXTURE_MAG_FILTER 0x2800  ///< 纹理放大过滤模式
#endif
#ifndef GL_REPEAT
#define GL_REPEAT 0x2901              ///< 纹理重复环绕
#endif
#ifndef GL_LINEAR
#define GL_LINEAR 0x2601              ///< 线性过滤
#endif
#ifndef GL_LINEAR_MIPMAP_LINEAR
#define GL_LINEAR_MIPMAP_LINEAR 0x2703 ///< 三线性过滤（最高质量）
#endif
#ifndef GL_RGB
#define GL_RGB 0x1907                 ///< RGB格式（3通道）
#endif
#ifndef GL_RGBA
#define GL_RGBA 0x1908                ///< RGBA格式（4通道，含透明度）
#endif
#ifndef GL_UNSIGNED_BYTE
#define GL_UNSIGNED_BYTE 0x1401       ///< 无符号字节类型
#endif
#ifndef GL_RED
#define GL_RED 0x1903                 ///< 单通道红色格式（灰度图）
#endif

// ============================================================================
// OpenGL函数指针类型定义
// ============================================================================

typedef void (APIENTRY *PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);
typedef void (APIENTRY *PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint *textures);
typedef void (APIENTRY *PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, 
    GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRY *PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (APIENTRY *PFNGLGENERATEMIPMAPPROC)(GLenum target);

// ============================================================================
// 静态变量和辅助函数
// ============================================================================

/// glGenerateMipmap函数指针（OpenGL 3.0+扩展函数）
static PFNGLGENERATEMIPMAPPROC glGenerateMipmapPtr = nullptr;

/// 标记OpenGL函数是否已加载
static bool functionsLoaded = false;

/**
 * @brief 加载纹理相关的OpenGL扩展函数
 * @return 始终返回true
 * 
 * 使用wglGetProcAddress获取OpenGL扩展函数的地址。
 * glGenerateMipmap是OpenGL 3.0引入的函数，需要动态加载。
 */
static bool LoadTextureFunctions() {
    if (functionsLoaded) return true;
    
    // 加载Mipmap生成函数
    glGenerateMipmapPtr = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
    
    functionsLoaded = true;
    return true;
}

/**
 * @brief 获取文件扩展名（转换为小写）
 * @param filepath 文件路径
 * @return 小写的文件扩展名，如果没有扩展名则返回空字符串
 */
static std::string GetFileExtension(const std::string& filepath) {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    std::string ext = filepath.substr(dotPos + 1);
    // 转换为小写以便比较
    std::transform(ext.begin(), ext.end(), ext.begin(), 
        [](unsigned char c) { return std::tolower(c); });
    return ext;
}

/**
 * @brief 检查文件格式是否支持
 * @param filepath 文件路径
 * @return 如果格式支持返回true，否则返回false
 * 
 * stb_image支持的格式：
 * - BMP: Windows位图
 * - JPG/JPEG: JPEG压缩图像
 * - PNG: 便携式网络图形（支持透明度）
 * - TGA: Targa图像
 * - GIF: 图形交换格式（仅第一帧）
 * - PSD: Photoshop文档（仅合并图层）
 * - HDR: 高动态范围图像
 * - PIC: Softimage图像
 */
bool TextureLoader::IsSupportedFormat(const std::string& filepath) {
    std::string ext = GetFileExtension(filepath);
    return (ext == "bmp" || ext == "jpg" || ext == "jpeg" || ext == "png" ||
            ext == "tga" || ext == "gif" || ext == "psd" || ext == "hdr" || ext == "pic");
}

/**
 * @brief 从文件加载纹理
 * @param filepath 纹理文件路径
 * @return 成功返回OpenGL纹理ID，失败返回0
 * 
 * 详细加载流程：
 * 1. 加载OpenGL扩展函数（如果尚未加载）
 * 2. 检查文件格式是否支持
 * 3. 配置stb_image垂直翻转（OpenGL纹理坐标原点在左下角）
 * 4. 使用stb_image读取图片数据
 * 5. 根据通道数确定OpenGL格式
 * 6. 创建OpenGL纹理对象
 * 7. 设置纹理参数（环绕模式、过滤模式）
 * 8. 上传纹理数据到GPU
 * 9. 生成Mipmap（多级渐远纹理）
 * 10. 释放CPU端图片数据
 */
unsigned int TextureLoader::LoadTexture(const std::string& filepath) {
    // 加载OpenGL扩展函数（如果尚未加载）
    LoadTextureFunctions();
    
    // 检查文件格式是否支持
    if (!IsSupportedFormat(filepath)) {
        std::string msg = "不支持的纹理格式:\n" + filepath + 
                          "\n\n支持的格式: BMP, JPG, PNG, TGA, GIF, PSD, HDR, PIC";
        MessageBoxA(NULL, msg.c_str(), "纹理加载错误", MB_OK | MB_ICONERROR);
        return 0;
    }
    
    // 配置stb_image垂直翻转图片
    // OpenGL纹理坐标原点在左下角，而大多数图片格式原点在左上角
    stbi_set_flip_vertically_on_load(true);
    
    // 使用stb_image加载图片数据
    int width, height, channels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        std::string msg = "无法加载纹理文件:\n" + filepath + 
                          "\n\n错误: " + std::string(stbi_failure_reason());
        MessageBoxA(NULL, msg.c_str(), "纹理加载错误", MB_OK | MB_ICONERROR);
        return 0;
    }
    
    // 根据通道数确定OpenGL格式
    // 1通道: 灰度图 (GL_RED)
    // 3通道: RGB彩色图
    // 4通道: RGBA彩色图（含透明度）
    GLenum format;
    GLenum internalFormat;
    switch (channels) {
        case 1:
            format = GL_RED;
            internalFormat = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            internalFormat = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            internalFormat = GL_RGBA;
            break;
        default:
            stbi_image_free(data);
            std::string msg = "不支持的通道数: " + std::to_string(channels);
            MessageBoxA(NULL, msg.c_str(), "纹理加载错误", MB_OK | MB_ICONERROR);
            return 0;
    }
    
    // 生成OpenGL纹理对象
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    if (textureID == 0) {
        stbi_image_free(data);
        MessageBoxA(NULL, "无法生成OpenGL纹理ID", 
                    "纹理加载错误", MB_OK | MB_ICONERROR);
        return 0;
    }
    
    // 绑定纹理并设置参数
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // 设置纹理环绕模式（超出[0,1]范围时重复纹理）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // 设置纹理过滤模式
    // 缩小时使用三线性过滤（最高质量，需要Mipmap）
    // 放大时使用线性过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // 上传纹理数据到GPU
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, 
                 format, GL_UNSIGNED_BYTE, data);
    
    // 生成Mipmap（多级渐远纹理）
    // Mipmap可以提高远距离渲染质量并减少锯齿
    if (glGenerateMipmapPtr) {
        glGenerateMipmapPtr(GL_TEXTURE_2D);
    }
    
    // 释放CPU端图片数据（已上传到GPU，不再需要）
    stbi_image_free(data);
    
    // 解绑纹理
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // 输出调试信息
    char debugMsg[512];
    sprintf_s(debugMsg, "纹理加载成功: %s\n尺寸: %dx%d, 通道数: %d, ID: %u",
              filepath.c_str(), width, height, channels, textureID);
    OutputDebugStringA(debugMsg);
    
    return textureID;
}

/**
 * @brief 删除纹理
 * @param textureID 要删除的纹理ID
 * 
 * 释放GPU上的纹理内存。
 * 删除后该纹理ID将不再有效，不应再使用。
 */
void TextureLoader::DeleteTexture(unsigned int textureID) {
    if (textureID != 0) {
        GLuint id = textureID;
        glDeleteTextures(1, &id);
        
        // 输出调试信息
        char debugMsg[128];
        sprintf_s(debugMsg, "纹理已删除: ID %u", textureID);
        OutputDebugStringA(debugMsg);
    }
}
