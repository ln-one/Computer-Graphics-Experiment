#pragma once
#include <string>

// 纹理加载类
// Texture loading class using stb_image
class TextureLoader {
public:
    // 从文件加载纹理
    // Load texture from file (supports BMP, JPG, PNG formats)
    // Returns texture ID on success, 0 on failure
    static unsigned int LoadTexture(const std::string& filepath);
    
    // 删除纹理
    // Delete texture and free GPU memory
    static void DeleteTexture(unsigned int textureID);
    
    // 检查文件格式是否支持
    // Check if file format is supported
    static bool IsSupportedFormat(const std::string& filepath);
};
