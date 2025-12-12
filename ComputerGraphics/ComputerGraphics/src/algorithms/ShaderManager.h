#pragma once
#include <string>

// 着色器管理类
class ShaderManager {
public:
    // 创建着色器程序
    static unsigned int CreateShaderProgram(
        const char* vertexSource, 
        const char* fragmentSource);
    
    // 获取默认着色器
    static const char* GetDefaultVertexShader();
    static const char* GetDefaultFragmentShader();
    
private:
    static unsigned int CompileShader(const char* source, unsigned int type);
    static bool CheckCompileErrors(unsigned int shader, const std::string& type);
};