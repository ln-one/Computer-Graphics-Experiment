#include "TextureLoader.h"
#include "../engine/OpenGLFunctions.h"
#include <windows.h>
#include <gl/GL.h>
#include <algorithm>
#include <cctype>

// Define STB_IMAGE_IMPLEMENTATION in this file to include the implementation
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Additional OpenGL constants for texture operations
#ifndef GL_TEXTURE_WRAP_S
#define GL_TEXTURE_WRAP_S 0x2802
#endif
#ifndef GL_TEXTURE_WRAP_T
#define GL_TEXTURE_WRAP_T 0x2803
#endif
#ifndef GL_TEXTURE_MIN_FILTER
#define GL_TEXTURE_MIN_FILTER 0x2801
#endif
#ifndef GL_TEXTURE_MAG_FILTER
#define GL_TEXTURE_MAG_FILTER 0x2800
#endif
#ifndef GL_REPEAT
#define GL_REPEAT 0x2901
#endif
#ifndef GL_LINEAR
#define GL_LINEAR 0x2601
#endif
#ifndef GL_LINEAR_MIPMAP_LINEAR
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#endif
#ifndef GL_RGB
#define GL_RGB 0x1907
#endif
#ifndef GL_RGBA
#define GL_RGBA 0x1908
#endif
#ifndef GL_UNSIGNED_BYTE
#define GL_UNSIGNED_BYTE 0x1401
#endif
#ifndef GL_RED
#define GL_RED 0x1903
#endif

// OpenGL function pointer types for texture operations
typedef void (APIENTRY *PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);
typedef void (APIENTRY *PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint *textures);
typedef void (APIENTRY *PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, 
    GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRY *PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (APIENTRY *PFNGLGENERATEMIPMAPPROC)(GLenum target);

// Static function pointers for texture operations
static PFNGLGENERATEMIPMAPPROC glGenerateMipmapPtr = nullptr;
static bool functionsLoaded = false;

// Helper function to load texture-related OpenGL functions
static bool LoadTextureFunctions() {
    if (functionsLoaded) return true;
    
    glGenerateMipmapPtr = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
    
    functionsLoaded = true;
    return true;
}

// Helper function to get file extension (lowercase)
static std::string GetFileExtension(const std::string& filepath) {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    std::string ext = filepath.substr(dotPos + 1);
    // Convert to lowercase
    std::transform(ext.begin(), ext.end(), ext.begin(), 
        [](unsigned char c) { return std::tolower(c); });
    return ext;
}

bool TextureLoader::IsSupportedFormat(const std::string& filepath) {
    std::string ext = GetFileExtension(filepath);
    return (ext == "bmp" || ext == "jpg" || ext == "jpeg" || ext == "png" ||
            ext == "tga" || ext == "gif" || ext == "psd" || ext == "hdr" || ext == "pic");
}

unsigned int TextureLoader::LoadTexture(const std::string& filepath) {
    // Load texture functions if not already loaded
    LoadTextureFunctions();
    
    // Check if file format is supported
    if (!IsSupportedFormat(filepath)) {
        std::string msg = "Unsupported texture format:\n" + filepath + 
                          "\n\nSupported formats: BMP, JPG, PNG, TGA, GIF, PSD, HDR, PIC";
        MessageBoxA(NULL, msg.c_str(), "Texture Load Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    
    // Configure stb_image to flip images vertically (OpenGL expects bottom-left origin)
    stbi_set_flip_vertically_on_load(true);
    
    // Load image data using stb_image
    int width, height, channels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        std::string msg = "Failed to load texture file:\n" + filepath + 
                          "\n\nError: " + std::string(stbi_failure_reason());
        MessageBoxA(NULL, msg.c_str(), "Texture Load Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    
    // Determine OpenGL format based on number of channels
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
            std::string msg = "Unsupported number of channels: " + std::to_string(channels);
            MessageBoxA(NULL, msg.c_str(), "Texture Load Error", MB_OK | MB_ICONERROR);
            return 0;
    }
    
    // Generate OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    if (textureID == 0) {
        stbi_image_free(data);
        MessageBoxA(NULL, "Failed to generate OpenGL texture ID", 
                    "Texture Load Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    
    // Bind texture and set parameters
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Upload texture data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, 
                 format, GL_UNSIGNED_BYTE, data);
    
    // Generate mipmaps for better quality at different distances
    if (glGenerateMipmapPtr) {
        glGenerateMipmapPtr(GL_TEXTURE_2D);
    }
    
    // Free image data (no longer needed after upload to GPU)
    stbi_image_free(data);
    
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Debug output
    char debugMsg[512];
    sprintf_s(debugMsg, "Texture loaded successfully: %s\nSize: %dx%d, Channels: %d, ID: %u",
              filepath.c_str(), width, height, channels, textureID);
    OutputDebugStringA(debugMsg);
    
    return textureID;
}

void TextureLoader::DeleteTexture(unsigned int textureID) {
    if (textureID != 0) {
        GLuint id = textureID;
        glDeleteTextures(1, &id);
        
        // Debug output
        char debugMsg[128];
        sprintf_s(debugMsg, "Texture deleted: ID %u", textureID);
        OutputDebugStringA(debugMsg);
    }
}
