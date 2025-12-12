#include "ShaderManager.h"
#include <windows.h>
#include <iostream>
#include <sstream>

// 需要包含OpenGL头文件 - 这里假设已经通过GLAD或其他方式加载
// #include <glad/glad.h> 或 #include <GL/gl.h>

// OpenGL常量定义（如果没有包含OpenGL头文件）
#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84
#endif

// OpenGL函数指针（需要在实际使用时通过GLAD等加载）
// 这里提供函数签名，实际实现需要动态加载
extern "C" {
    unsigned int (*glCreateShader)(unsigned int type);
    void (*glShaderSource)(unsigned int shader, int count, const char* const* string, const int* length);
    void (*glCompileShader)(unsigned int shader);
    void (*glGetShaderiv)(unsigned int shader, unsigned int pname, int* params);
    void (*glGetShaderInfoLog)(unsigned int shader, int bufSize, int* length, char* infoLog);
    unsigned int (*glCreateProgram)(void);
    void (*glAttachShader)(unsigned int program, unsigned int shader);
    void (*glLinkProgram)(unsigned int program);
    void (*glGetProgramiv)(unsigned int program, unsigned int pname, int* params);
    void (*glGetProgramInfoLog)(unsigned int program, int bufSize, int* length, char* infoLog);
    void (*glDeleteShader)(unsigned int shader);
}

unsigned int ShaderManager::CreateShaderProgram(const char* vertexSource, const char* fragmentSource) {
    // 编译顶点着色器
    unsigned int vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        return 0;
    }
    
    // 编译片段着色器
    unsigned int fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }
    
    // 创建着色器程序
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // 检查链接错误
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        
        std::string msg = "着色器程序链接失败:\n" + std::string(infoLog);
        MessageBoxA(NULL, msg.c_str(), "着色器链接错误", MB_OK | MB_ICONERROR);
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }
    
    // 删除着色器对象（已经链接到程序中）
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

unsigned int ShaderManager::CompileShader(const char* source, unsigned int type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    // 检查编译错误
    std::string typeStr = (type == GL_VERTEX_SHADER) ? "顶点着色器" : "片段着色器";
    if (!CheckCompileErrors(shader, typeStr)) {
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

bool ShaderManager::CheckCompileErrors(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[512];
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        
        std::string msg = "着色器编译失败 (" + type + "):\n" + std::string(infoLog);
        MessageBoxA(NULL, msg.c_str(), "着色器编译错误", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

const char* ShaderManager::GetDefaultVertexShader() {
    return R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoord = aTexCoord;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
}

const char* ShaderManager::GetDefaultFragmentShader() {
    return R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoord;
        
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 viewPos;
        
        uniform vec3 ambient;
        uniform vec3 diffuse;
        uniform vec3 specular;
        uniform float shininess;
        
        uniform bool useTexture;
        uniform sampler2D textureSampler;
        
        void main() {
            // Phong光照模型
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            
            // 环境光
            vec3 ambientColor = ambient * lightColor;
            
            // 漫反射
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuseColor = diffuse * diff * lightColor;
            
            // 镜面反射
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
            vec3 specularColor = specular * spec * lightColor;
            
            vec3 result = ambientColor + diffuseColor + specularColor;
            
            if (useTexture) {
                result *= texture(textureSampler, TexCoord).rgb;
            }
            
            FragColor = vec4(result, 1.0);
        }
    )";
}