#pragma once
#include <string>

/**
 * @file TextureLoader.h
 * @brief 纹理加载器类定义
 * @author ln1.opensource@gmail.com
 * 
 * 使用stb_image库实现纹理加载功能，支持多种常见图片格式。
 */

/**
 * @class TextureLoader
 * @brief 纹理加载器类
 * 
 * 提供纹理文件的加载、删除和格式检查功能。
 * 使用stb_image库解析图片文件，支持以下格式：
 * - BMP (位图)
 * - JPG/JPEG (JPEG压缩图像)
 * - PNG (便携式网络图形)
 * - TGA (Targa图像)
 * - GIF (图形交换格式)
 * - PSD (Photoshop文档)
 * - HDR (高动态范围图像)
 * - PIC (Softimage图像)
 */
class TextureLoader {
public:
    /**
     * @brief 从文件加载纹理
     * @param filepath 纹理文件路径
     * @return 成功返回OpenGL纹理ID，失败返回0
     * 
     * 加载流程：
     * 1. 检查文件格式是否支持
     * 2. 使用stb_image读取图片数据
     * 3. 创建OpenGL纹理对象
     * 4. 上传纹理数据到GPU
     * 5. 生成Mipmap
     */
    static unsigned int LoadTexture(const std::string& filepath);
    
    /**
     * @brief 删除纹理
     * @param textureID 要删除的纹理ID
     * 
     * 释放GPU上的纹理内存。
     */
    static void DeleteTexture(unsigned int textureID);
    
    /**
     * @brief 检查文件格式是否支持
     * @param filepath 文件路径
     * @return 如果格式支持返回true，否则返回false
     */
    static bool IsSupportedFormat(const std::string& filepath);
};
