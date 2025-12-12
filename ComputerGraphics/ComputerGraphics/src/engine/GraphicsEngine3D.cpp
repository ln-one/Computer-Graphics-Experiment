#include "GraphicsEngine3D.h"
#include "OpenGLFunctions.h"
#include "../algorithms/ShaderManager.h"
#include "../algorithms/MeshGenerator.h"
#include <gl/GL.h>
#include <cmath>
#include <cfloat>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Undefine Windows macros that conflict with parameter names
// Must be after all includes since windows.h defines these
#undef near
#undef far

// Simple matrix math functions for MVP calculation
struct Matrix4 {
    float m[16];
    
    Matrix4() {
        // Identity matrix
        for (int i = 0; i < 16; i++) m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }
    
    static Matrix4 perspective(float fov, float aspect, float nearPlane, float farPlane) {
        Matrix4 result;
        float f = 1.0f / tanf(fov * 0.5f);
        result.m[0] = f / aspect;
        result.m[5] = f;
        result.m[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
        result.m[11] = -1.0f;
        result.m[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
        result.m[15] = 0.0f;
        return result;
    }
    
    static Matrix4 lookAt(float eyeX, float eyeY, float eyeZ,
                         float centerX, float centerY, float centerZ,
                         float upX, float upY, float upZ) {
        Matrix4 result;
        
        // Calculate forward vector
        float fx = centerX - eyeX;
        float fy = centerY - eyeY;
        float fz = centerZ - eyeZ;
        float flen = sqrtf(fx*fx + fy*fy + fz*fz);
        fx /= flen; fy /= flen; fz /= flen;
        
        // Calculate right vector
        float rx = fy * upZ - fz * upY;
        float ry = fz * upX - fx * upZ;
        float rz = fx * upY - fy * upX;
        float rlen = sqrtf(rx*rx + ry*ry + rz*rz);
        rx /= rlen; ry /= rlen; rz /= rlen;
        
        // Calculate up vector
        float ux = ry * fz - rz * fy;
        float uy = rz * fx - rx * fz;
        float uz = rx * fy - ry * fx;
        
        result.m[0] = rx; result.m[4] = ux; result.m[8] = -fx; result.m[12] = -(rx*eyeX + ux*eyeY - fx*eyeZ);
        result.m[1] = ry; result.m[5] = uy; result.m[9] = -fy; result.m[13] = -(ry*eyeX + uy*eyeY - fy*eyeZ);
        result.m[2] = rz; result.m[6] = uz; result.m[10] = -fz; result.m[14] = -(rz*eyeX + uz*eyeY - fz*eyeZ);
        result.m[3] = 0; result.m[7] = 0; result.m[11] = 0; result.m[15] = 1;
        
        return result;
    }
    
    static Matrix4 translate(float x, float y, float z) {
        Matrix4 result;
        result.m[12] = x;
        result.m[13] = y;
        result.m[14] = z;
        return result;
    }
    
    static Matrix4 rotateX(float angle) {
        Matrix4 result;
        float c = cosf(angle);
        float s = sinf(angle);
        result.m[5] = c; result.m[6] = s;
        result.m[9] = -s; result.m[10] = c;
        return result;
    }
    
    static Matrix4 rotateY(float angle) {
        Matrix4 result;
        float c = cosf(angle);
        float s = sinf(angle);
        result.m[0] = c; result.m[2] = -s;
        result.m[8] = s; result.m[10] = c;
        return result;
    }
    
    static Matrix4 rotateZ(float angle) {
        Matrix4 result;
        float c = cosf(angle);
        float s = sinf(angle);
        result.m[0] = c; result.m[1] = s;
        result.m[4] = -s; result.m[5] = c;
        return result;
    }
    
    static Matrix4 scale(float x, float y, float z) {
        Matrix4 result;
        result.m[0] = x;
        result.m[5] = y;
        result.m[10] = z;
        return result;
    }
    
    Matrix4 multiply(const Matrix4& other) const {
        Matrix4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i*4 + j] = 0;
                for (int k = 0; k < 4; k++) {
                    result.m[i*4 + j] += m[i*4 + k] * other.m[k*4 + j];
                }
            }
        }
        return result;
    }
};

GraphicsEngine3D::GraphicsEngine3D() 
    : hwnd(NULL), hdc(NULL), hglrc(NULL), 
      currentMode(MODE_NONE), selectedShapeIndex(-1), hasSelection(false),
      lastMouseX(0), lastMouseY(0), isDragging(false),
      shaderProgram(0), isInitialized(false) {
    
    // Initialize default camera parameters
    camera.distance = 5.0f;
    camera.angleX = 0.0f;
    camera.angleY = 0.0f;
    camera.targetX = 0.0f;
    camera.targetY = 0.0f;
    camera.targetZ = 0.0f;
    
    // Initialize default light parameters
    light.positionX = 5.0f;
    light.positionY = 5.0f;
    light.positionZ = 5.0f;
    light.color[0] = light.color[1] = light.color[2] = 1.0f;
    light.ambientIntensity = 0.2f;
    light.diffuseIntensity = 0.8f;
    light.specularIntensity = 1.0f;
}

GraphicsEngine3D::~GraphicsEngine3D() {
    Shutdown();
}

bool GraphicsEngine3D::Initialize(HWND hwnd) {
    if (isInitialized) {
        return true;
    }
    
    this->hwnd = hwnd;
    this->hdc = GetDC(hwnd);
    
    if (!CreateOpenGLContext()) {
        MessageBoxW(hwnd, L"Failed to create OpenGL context. Please ensure your graphics card supports OpenGL.", L"Initialization Failed", MB_OK | MB_ICONERROR);
        return false;
    }
    
    if (!LoadOpenGLFunctions()) {
        MessageBoxW(hwnd, L"Failed to load OpenGL functions.", L"Initialization Failed", MB_OK | MB_ICONERROR);
        return false;
    }
    
    // Setup OpenGL basic state
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.4f, 0.8f, 1.0f);  // Blue background
    
    // Initialize shader program
    const char* vertexShader = ShaderManager::GetDefaultVertexShader();
    const char* fragmentShader = ShaderManager::GetDefaultFragmentShader();
    shaderProgram = ShaderManager::CreateShaderProgram(vertexShader, fragmentShader);
    
    if (shaderProgram == 0) {
        MessageBoxW(hwnd, L"Failed to create shader program.", L"Initialization Failed", MB_OK | MB_ICONERROR);
        return false;
    }
    
    isInitialized = true;
    return true;
}

void GraphicsEngine3D::Shutdown() {
    if (hglrc) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hglrc);
        hglrc = NULL;
    }
    
    if (hdc && hwnd) {
        ReleaseDC(hwnd, hdc);
        hdc = NULL;
    }
    
    isInitialized = false;
}

bool GraphicsEngine3D::CreateOpenGLContext() {
    // Setup pixel format
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,  // Color depth
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,  // Depth buffer
        8,   // Stencil buffer
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    if (pixelFormat == 0) {
        return false;
    }
    
    if (!SetPixelFormat(hdc, pixelFormat, &pfd)) {
        return false;
    }
    
    // Create OpenGL rendering context
    hglrc = wglCreateContext(hdc);
    if (!hglrc) {
        return false;
    }
    
    if (!wglMakeCurrent(hdc, hglrc)) {
        wglDeleteContext(hglrc);
        hglrc = NULL;
        return false;
    }
    
    return true;
}

// Additional OpenGL function pointer types not defined in OpenGLFunctions.h
typedef void (APIENTRY *PFNGLDRAWELEMENTSPROC_EXT)(GLenum mode, GLsizei count, GLenum type, const void *indices);
typedef void (APIENTRY *PFNGLUSEPROGRAMPROC_EXT)(GLuint program);
typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC_EXT)(GLuint program, const GLchar *name);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC_EXT)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY *PFNGLUNIFORM3FPROC_EXT)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRY *PFNGLUNIFORM1FPROC_EXT)(GLint location, GLfloat v0);
typedef void (APIENTRY *PFNGLUNIFORM1IPROC_EXT)(GLint location, GLint v0);
typedef void (APIENTRY *PFNGLACTIVETEXTUREPROC_EXT)(GLenum texture);
typedef void (APIENTRY *PFNGLBINDTEXTUREPROC_EXT)(GLenum target, GLuint texture);

// Global function pointers - definitions for the ones declared in OpenGLFunctions.h
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;

// Additional function pointers not in OpenGLFunctions.h
PFNGLDRAWELEMENTSPROC_EXT glDrawElementsExt = nullptr;
PFNGLUSEPROGRAMPROC_EXT glUseProgramExt = nullptr;
PFNGLGETUNIFORMLOCATIONPROC_EXT glGetUniformLocationExt = nullptr;
PFNGLUNIFORMMATRIX4FVPROC_EXT glUniformMatrix4fvExt = nullptr;
PFNGLUNIFORM3FPROC_EXT glUniform3fExt = nullptr;
PFNGLUNIFORM1FPROC_EXT glUniform1fExt = nullptr;
PFNGLUNIFORM1IPROC_EXT glUniform1iExt = nullptr;
PFNGLACTIVETEXTUREPROC_EXT glActiveTextureExt = nullptr;
PFNGLBINDTEXTUREPROC_EXT glBindTextureExt = nullptr;

bool GraphicsEngine3D::LoadOpenGLFunctions() {
    // Load OpenGL extension functions
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
    glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
    glDrawElementsExt = (PFNGLDRAWELEMENTSPROC_EXT)wglGetProcAddress("glDrawElements");
    glUseProgramExt = (PFNGLUSEPROGRAMPROC_EXT)wglGetProcAddress("glUseProgram");
    glGetUniformLocationExt = (PFNGLGETUNIFORMLOCATIONPROC_EXT)wglGetProcAddress("glGetUniformLocation");
    glUniformMatrix4fvExt = (PFNGLUNIFORMMATRIX4FVPROC_EXT)wglGetProcAddress("glUniformMatrix4fv");
    glUniform3fExt = (PFNGLUNIFORM3FPROC_EXT)wglGetProcAddress("glUniform3f");
    glUniform1fExt = (PFNGLUNIFORM1FPROC_EXT)wglGetProcAddress("glUniform1f");
    glUniform1iExt = (PFNGLUNIFORM1IPROC_EXT)wglGetProcAddress("glUniform1i");
    glActiveTextureExt = (PFNGLACTIVETEXTUREPROC_EXT)wglGetProcAddress("glActiveTexture");
    glBindTextureExt = (PFNGLBINDTEXTUREPROC_EXT)wglGetProcAddress("glBindTexture");
    
    // Check if all functions were loaded successfully
    return (glGenVertexArrays && glBindVertexArray && glGenBuffers && 
            glBindBuffer && glBufferData && glVertexAttribPointer && 
            glEnableVertexAttribArray && glDrawElementsExt && glUseProgramExt &&
            glGetUniformLocationExt && glUniformMatrix4fvExt && glUniform3fExt &&
            glUniform1fExt && glUniform1iExt && glActiveTextureExt && glBindTextureExt &&
            glDeleteVertexArrays && glDeleteBuffers);
}

void GraphicsEngine3D::Render() {
    if (!isInitialized) {
        return;
    }
    
    // Make sure our OpenGL context is current
    wglMakeCurrent(hdc, hglrc);
    
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Debug: Always use fixed pipeline for now to ensure something renders
    RenderWithFixedPipeline();
    SwapBuffers(hdc);
    return;
    
    // If shader program failed to load, use fixed function pipeline
    if (shaderProgram == 0) {
        RenderWithFixedPipeline();
        SwapBuffers(hdc);
        return;
    }
    
    // Get window dimensions for aspect ratio
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width == 0 || height == 0) return;
    
    float aspectRatio = (float)width / (float)height;
    
    // Set viewport
    glViewport(0, 0, width, height);
    
    // Use shader program
    glUseProgramExt(shaderProgram);
    
    // Calculate camera position
    float cameraX = camera.targetX + camera.distance * cosf(camera.angleY * (float)M_PI / 180.0f) * cosf(camera.angleX * (float)M_PI / 180.0f);
    float cameraY = camera.targetY + camera.distance * sinf(camera.angleY * (float)M_PI / 180.0f);
    float cameraZ = camera.targetZ + camera.distance * cosf(camera.angleY * (float)M_PI / 180.0f) * sinf(camera.angleX * (float)M_PI / 180.0f);
    
    // Create matrices
    Matrix4 projection = Matrix4::perspective(45.0f * (float)M_PI / 180.0f, aspectRatio, 0.1f, 100.0f);
    Matrix4 view = Matrix4::lookAt(cameraX, cameraY, cameraZ,
                                   camera.targetX, camera.targetY, camera.targetZ,
                                   0.0f, 1.0f, 0.0f);
    
    // Set projection and view matrices
    int projLoc = glGetUniformLocationExt(shaderProgram, "projection");
    int viewLoc = glGetUniformLocationExt(shaderProgram, "view");
    int modelLoc = glGetUniformLocationExt(shaderProgram, "model");
    
    if (projLoc >= 0) glUniformMatrix4fvExt(projLoc, 1, GL_FALSE, projection.m);
    if (viewLoc >= 0) glUniformMatrix4fvExt(viewLoc, 1, GL_FALSE, view.m);
    
    // Set light parameters
    int lightPosLoc = glGetUniformLocationExt(shaderProgram, "lightPos");
    int lightColorLoc = glGetUniformLocationExt(shaderProgram, "lightColor");
    int viewPosLoc = glGetUniformLocationExt(shaderProgram, "viewPos");
    
    if (lightPosLoc >= 0) glUniform3fExt(lightPosLoc, light.positionX, light.positionY, light.positionZ);
    if (lightColorLoc >= 0) glUniform3fExt(lightColorLoc, light.color[0], light.color[1], light.color[2]);
    if (viewPosLoc >= 0) glUniform3fExt(viewPosLoc, cameraX, cameraY, cameraZ);
    
    // Render all shapes
    for (size_t i = 0; i < shapes.size(); i++) {
        const Shape3D& shape = shapes[i];
        
        // Calculate model matrix
        Matrix4 model = Matrix4::translate(shape.positionX, shape.positionY, shape.positionZ);
        model = model.multiply(Matrix4::rotateZ(shape.rotationZ * (float)M_PI / 180.0f));
        model = model.multiply(Matrix4::rotateY(shape.rotationY * (float)M_PI / 180.0f));
        model = model.multiply(Matrix4::rotateX(shape.rotationX * (float)M_PI / 180.0f));
        model = model.multiply(Matrix4::scale(shape.scaleX, shape.scaleY, shape.scaleZ));
        
        if (modelLoc >= 0) glUniformMatrix4fvExt(modelLoc, 1, GL_FALSE, model.m);
        
        // Set material properties
        int ambientLoc = glGetUniformLocationExt(shaderProgram, "ambient");
        int diffuseLoc = glGetUniformLocationExt(shaderProgram, "diffuse");
        int specularLoc = glGetUniformLocationExt(shaderProgram, "specular");
        int shininessLoc = glGetUniformLocationExt(shaderProgram, "shininess");
        int useTextureLoc = glGetUniformLocationExt(shaderProgram, "useTexture");
        
        // Use different color for selected shapes
        float ambient[3], diffuse[3], specular[3];
        if (shape.selected) {
            // Highlight selected shapes with yellow tint
            ambient[0] = 0.3f; ambient[1] = 0.3f; ambient[2] = 0.1f;
            diffuse[0] = 1.0f; diffuse[1] = 1.0f; diffuse[2] = 0.3f;
            specular[0] = 1.0f; specular[1] = 1.0f; specular[2] = 0.5f;
        } else {
            ambient[0] = shape.ambient[0]; ambient[1] = shape.ambient[1]; ambient[2] = shape.ambient[2];
            diffuse[0] = shape.diffuse[0]; diffuse[1] = shape.diffuse[1]; diffuse[2] = shape.diffuse[2];
            specular[0] = shape.specular[0]; specular[1] = shape.specular[1]; specular[2] = shape.specular[2];
        }
        
        if (ambientLoc >= 0) glUniform3fExt(ambientLoc, ambient[0], ambient[1], ambient[2]);
        if (diffuseLoc >= 0) glUniform3fExt(diffuseLoc, diffuse[0], diffuse[1], diffuse[2]);
        if (specularLoc >= 0) glUniform3fExt(specularLoc, specular[0], specular[1], specular[2]);
        if (shininessLoc >= 0) glUniform1fExt(shininessLoc, shape.shininess);
        if (useTextureLoc >= 0) glUniform1iExt(useTextureLoc, shape.hasTexture ? 1 : 0);
        
        // Bind texture if available
        if (shape.hasTexture && shape.textureID != 0) {
            glActiveTextureExt(GL_TEXTURE0);
            glBindTextureExt(GL_TEXTURE_2D, shape.textureID);
        }
        
        // Render the shapes
        if (shape.VAO != 0) {
            glBindVertexArray(shape.VAO);
            glDrawElementsExt(GL_TRIANGLES, (GLsizei)shape.indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
    
    // Swap buffers
    SwapBuffers(hdc);
}

void GraphicsEngine3D::ClearScene() {
    shapes.clear();
    selectedShapeIndex = -1;
    hasSelection = false;
}

void GraphicsEngine3D::SetMode(DrawMode mode) {
    currentMode = mode;
}

void GraphicsEngine3D::OnLButtonDown(int x, int y) {
    lastMouseX = x;
    lastMouseY = y;
    isDragging = true;
    
    // 检查是否按住了Ctrl键（用于视角控制）
    bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    
    if (ctrlPressed) {
        // Ctrl + 左键 = 旋转视角
        // 不创建形状，只记录鼠标位置用于旋转
        return;
    }
    
    switch (currentMode) {
        case MODE_3D_SPHERE:
        case MODE_3D_CYLINDER:
        case MODE_3D_PLANE:
        case MODE_3D_CUBE:
            HandleShapeCreation(x, y);
            break;
        case MODE_3D_SELECT:
            HandleSelection(x, y);
            break;
        case MODE_3D_VIEW_CONTROL:
            // 视角控制模式：只旋转，不创建形状
            break;
        default:
            break;
    }
}

void GraphicsEngine3D::OnLButtonUp(int x, int y) {
    isDragging = false;
}

void GraphicsEngine3D::OnLButtonDoubleClick(int x, int y) {
    // Double-click to open transform dialog (to be implemented)
}

void GraphicsEngine3D::OnMouseMove(int x, int y) {
    if (!isDragging) {
        return;
    }
    
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;
    
    // 检查是否按住了Ctrl键（用于视角控制）
    bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    
    if (currentMode == MODE_3D_VIEW_CONTROL || ctrlPressed) {
        HandleViewControl(deltaX, deltaY);
    }
    
    lastMouseX = x;
    lastMouseY = y;
}

void GraphicsEngine3D::OnMouseWheel(int delta) {
    // Adjust camera distance based on wheel delta
    float zoomSpeed = 0.1f;
    camera.distance -= (delta / 120.0f) * zoomSpeed;  // 120 is standard wheel delta
    
    // Clamp distance to reasonable values
    if (camera.distance < 1.0f) camera.distance = 1.0f;
    if (camera.distance > 20.0f) camera.distance = 20.0f;
}

void GraphicsEngine3D::HandleShapeCreation(int x, int y) {
    // Convert screen coordinates to world coordinates (simplified)
    // For now, place shapes at fixed positions in 3D space
    
    // Debug: Show message when shape creation is called
    char debugMsg[256];
    sprintf_s(debugMsg, "Creating shape at (%d, %d), current mode: %d", x, y, (int)currentMode);
    OutputDebugStringA(debugMsg);
    
    Shape3D newShape;
    
    // Set position based on screen coordinates (simplified mapping)
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    if (width > 0 && height > 0) {
        // Map screen coordinates to world coordinates
        float worldX = ((float)x / width - 0.5f) * 4.0f;  // Range: -2 to 2
        float worldY = -((float)y / height - 0.5f) * 4.0f; // Range: -2 to 2 (inverted Y)
        float worldZ = 0.0f; // Place at origin Z
        
        newShape.positionX = worldX;
        newShape.positionY = worldY;
        newShape.positionZ = worldZ;
    }
    
    // Set default rotation and scale
    newShape.rotationX = 0.0f;
    newShape.rotationY = 0.0f;
    newShape.rotationZ = 0.0f;
    newShape.scaleX = 1.0f;
    newShape.scaleY = 1.0f;
    newShape.scaleZ = 1.0f;
    
    // Generate mesh based on current mode
    switch (currentMode) {
        case MODE_3D_CUBE:
            MeshGenerator::GenerateCube(newShape, 1.0f);
            break;
        case MODE_3D_SPHERE:
            MeshGenerator::GenerateSphere(newShape, 0.5f, 16, 16);
            break;
        case MODE_3D_CYLINDER:
            MeshGenerator::GenerateCylinder(newShape, 0.5f, 1.0f, 16);
            break;
        case MODE_3D_PLANE:
            MeshGenerator::GeneratePlane(newShape, 1.0f, 1.0f);
            break;
        default:
            return; // Unknown shape type
    }
    
    // Add to shapes collection
    shapes.push_back(newShape);
    
    // Debug: Show message when shape is added
    char debugMsg2[256];
    sprintf_s(debugMsg2, "Shape added! Total shapes: %zu, VAO: %u", shapes.size(), newShape.VAO);
    OutputDebugStringA(debugMsg2);
}

void GraphicsEngine3D::HandleSelection(int x, int y) {
    // Convert screen coordinates to world coordinates for selection
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    if (width <= 0 || height <= 0) return;
    
    // Map screen coordinates to normalized device coordinates
    float ndcX = ((float)x / width) * 2.0f - 1.0f;   // Range: -1 to 1
    float ndcY = -(((float)y / height) * 2.0f - 1.0f); // Range: -1 to 1 (inverted Y)
    
    // Simple selection: find the closest shape to the click point
    // For now, we'll use a simple 2D distance check in screen space
    int closestShapeIndex = -1;
    float minDistance = FLT_MAX;
    
    for (size_t i = 0; i < shapes.size(); i++) {
        const Shape3D& shape = shapes[i];
        
        // Project shape position to screen space (simplified)
        // Map world coordinates to screen coordinates
        float screenX = ((shape.positionX + 2.0f) / 4.0f) * width;   // Assuming world range -2 to 2
        float screenY = ((-shape.positionY + 2.0f) / 4.0f) * height; // Inverted Y
        
        // Calculate distance from click point to shape center
        float dx = (float)x - screenX;
        float dy = (float)y - screenY;
        float distance = sqrtf(dx * dx + dy * dy);
        
        // Consider shape size for selection tolerance
        float selectionRadius = 50.0f; // Base selection radius in pixels
        
        if (distance < selectionRadius && distance < minDistance) {
            minDistance = distance;
            closestShapeIndex = (int)i;
        }
    }
    
    // Clear previous selection
    for (size_t i = 0; i < shapes.size(); i++) {
        shapes[i].selected = false;
    }
    
    // Select the closest shape if found
    if (closestShapeIndex >= 0) {
        shapes[closestShapeIndex].selected = true;
        selectedShapeIndex = closestShapeIndex;
        hasSelection = true;
        
        // Debug message
        char debugMsg[256];
        sprintf_s(debugMsg, "Selected shape %d at position (%.2f, %.2f, %.2f)", 
                  closestShapeIndex, 
                  shapes[closestShapeIndex].positionX,
                  shapes[closestShapeIndex].positionY,
                  shapes[closestShapeIndex].positionZ);
        OutputDebugStringA(debugMsg);
    } else {
        selectedShapeIndex = -1;
        hasSelection = false;
        OutputDebugStringA("No shape selected");
    }
}

void GraphicsEngine3D::HandleViewControl(int deltaX, int deltaY) {
    // Update camera angles based on mouse movement
    camera.angleX += deltaX * 0.5f;  // Horizontal rotation
    camera.angleY += deltaY * 0.5f;  // Vertical rotation
    
    // Clamp vertical angle to prevent flipping
    if (camera.angleY > 89.0f) camera.angleY = 89.0f;
    if (camera.angleY < -89.0f) camera.angleY = -89.0f;
}

void GraphicsEngine3D::UpdateLight() {
    // Light update logic (to be implemented)
}

void GraphicsEngine3D::ReleaseContext() {
    // Release OpenGL context to allow 2D rendering
    wglMakeCurrent(NULL, NULL);
}

void GraphicsEngine3D::RenderWithFixedPipeline() {
    // Get window dimensions for aspect ratio
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width == 0 || height == 0) return;
    
    float aspectRatio = (float)width / (float)height;
    
    // Set viewport
    glViewport(0, 0, width, height);
    
    // Setup projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Create perspective projection
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float top = nearPlane * tanf(fov * (float)M_PI / 360.0f);
    float bottom = -top;
    float right = top * aspectRatio;
    float left = -right;
    glFrustum(left, right, bottom, top, nearPlane, farPlane);
    
    // Setup modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Calculate camera position
    float cameraX = camera.targetX + camera.distance * cosf(camera.angleY * (float)M_PI / 180.0f) * cosf(camera.angleX * (float)M_PI / 180.0f);
    float cameraY = camera.targetY + camera.distance * sinf(camera.angleY * (float)M_PI / 180.0f);
    float cameraZ = camera.targetZ + camera.distance * cosf(camera.angleY * (float)M_PI / 180.0f) * sinf(camera.angleX * (float)M_PI / 180.0f);
    
    // Set camera (manual implementation since gluLookAt might not be available)
    // Calculate view matrix manually
    float fx = camera.targetX - cameraX;
    float fy = camera.targetY - cameraY;
    float fz = camera.targetZ - cameraZ;
    float flen = sqrtf(fx*fx + fy*fy + fz*fz);
    fx /= flen; fy /= flen; fz /= flen;
    
    float upX = 0.0f, upY = 1.0f, upZ = 0.0f;
    float rx = fy * upZ - fz * upY;
    float ry = fz * upX - fx * upZ;
    float rz = fx * upY - fy * upX;
    float rlen = sqrtf(rx*rx + ry*ry + rz*rz);
    rx /= rlen; ry /= rlen; rz /= rlen;
    
    float ux = ry * fz - rz * fy;
    float uy = rz * fx - rx * fz;
    float uz = rx * fy - ry * fx;
    
    float viewMatrix[16] = {
        rx, ux, -fx, 0,
        ry, uy, -fy, 0,
        rz, uz, -fz, 0,
        -(rx*cameraX + ry*cameraY + rz*cameraZ),
        -(ux*cameraX + uy*cameraY + uz*cameraZ),
        -(-fx*cameraX + -fy*cameraY + -fz*cameraZ),
        1
    };
    
    glMultMatrixf(viewMatrix);
    
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Set light position
    float lightPos[] = {light.positionX, light.positionY, light.positionZ, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    // Set light colors
    float lightColor[] = {light.color[0], light.color[1], light.color[2], 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
    
    float ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    
    // Render all shapes using immediate mode
    for (size_t i = 0; i < shapes.size(); i++) {
        const Shape3D& shape = shapes[i];
        
        glPushMatrix();
        
        // Apply transformations
        glTranslatef(shape.positionX, shape.positionY, shape.positionZ);
        glRotatef(shape.rotationZ, 0.0f, 0.0f, 1.0f);
        glRotatef(shape.rotationY, 0.0f, 1.0f, 0.0f);
        glRotatef(shape.rotationX, 1.0f, 0.0f, 0.0f);
        glScalef(shape.scaleX, shape.scaleY, shape.scaleZ);
        
        // Set material properties with selection highlighting
        float ambient[4], diffuse[4], specular[4];
        
        if (shape.selected) {
            // Highlight selected shapes with yellow tint
            ambient[0] = 0.3f; ambient[1] = 0.3f; ambient[2] = 0.1f; ambient[3] = 1.0f;
            diffuse[0] = 1.0f; diffuse[1] = 1.0f; diffuse[2] = 0.3f; diffuse[3] = 1.0f;
            specular[0] = 1.0f; specular[1] = 1.0f; specular[2] = 0.5f; specular[3] = 1.0f;
        } else {
            ambient[0] = shape.ambient[0]; ambient[1] = shape.ambient[1]; ambient[2] = shape.ambient[2]; ambient[3] = 1.0f;
            diffuse[0] = shape.diffuse[0]; diffuse[1] = shape.diffuse[1]; diffuse[2] = shape.diffuse[2]; diffuse[3] = 1.0f;
            specular[0] = shape.specular[0]; specular[1] = shape.specular[1]; specular[2] = shape.specular[2]; specular[3] = 1.0f;
        }
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shape.shininess);
        
        // Render shape based on type
        switch (shape.type) {
            case SHAPE3D_CUBE:
                RenderCubeImmediate(1.0f);
                break;
            case SHAPE3D_SPHERE:
                RenderSphereImmediate(0.5f, 16, 16);
                break;
            case SHAPE3D_CYLINDER:
                RenderCylinderImmediate(0.5f, 1.0f, 16);
                break;
            case SHAPE3D_PLANE:
                RenderPlaneImmediate(1.0f, 1.0f);
                break;
        }
        
        glPopMatrix();
    }
    
    glDisable(GL_LIGHTING);
}

void GraphicsEngine3D::RenderCubeImmediate(float size) {
    float halfSize = size * 0.5f;
    
    glBegin(GL_QUADS);
    
    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-halfSize, -halfSize,  halfSize);
    glVertex3f( halfSize, -halfSize,  halfSize);
    glVertex3f( halfSize,  halfSize,  halfSize);
    glVertex3f(-halfSize,  halfSize,  halfSize);
    
    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(-halfSize,  halfSize, -halfSize);
    glVertex3f( halfSize,  halfSize, -halfSize);
    glVertex3f( halfSize, -halfSize, -halfSize);
    
    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-halfSize,  halfSize, -halfSize);
    glVertex3f(-halfSize,  halfSize,  halfSize);
    glVertex3f( halfSize,  halfSize,  halfSize);
    glVertex3f( halfSize,  halfSize, -halfSize);
    
    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f( halfSize, -halfSize, -halfSize);
    glVertex3f( halfSize, -halfSize,  halfSize);
    glVertex3f(-halfSize, -halfSize,  halfSize);
    
    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f( halfSize, -halfSize, -halfSize);
    glVertex3f( halfSize,  halfSize, -halfSize);
    glVertex3f( halfSize,  halfSize,  halfSize);
    glVertex3f( halfSize, -halfSize,  halfSize);
    
    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(-halfSize, -halfSize,  halfSize);
    glVertex3f(-halfSize,  halfSize,  halfSize);
    glVertex3f(-halfSize,  halfSize, -halfSize);
    
    glEnd();
}

void GraphicsEngine3D::RenderSphereImmediate(float radius, int segments, int rings) {
    glBegin(GL_TRIANGLES);
    
    for (int ring = 0; ring < rings; ring++) {
        float phi1 = (float)M_PI * ring / rings;        // Current ring angle
        float phi2 = (float)M_PI * (ring + 1) / rings;  // Next ring angle
        
        for (int seg = 0; seg < segments; seg++) {
            float theta1 = 2.0f * (float)M_PI * seg / segments;        // Current segment angle
            float theta2 = 2.0f * (float)M_PI * (seg + 1) / segments;  // Next segment angle
            
            // Calculate vertices for the quad (2 triangles)
            // Vertex 1: current ring, current segment
            float x1 = radius * sinf(phi1) * cosf(theta1);
            float y1 = radius * cosf(phi1);
            float z1 = radius * sinf(phi1) * sinf(theta1);
            float nx1 = x1 / radius, ny1 = y1 / radius, nz1 = z1 / radius;
            
            // Vertex 2: next ring, current segment
            float x2 = radius * sinf(phi2) * cosf(theta1);
            float y2 = radius * cosf(phi2);
            float z2 = radius * sinf(phi2) * sinf(theta1);
            float nx2 = x2 / radius, ny2 = y2 / radius, nz2 = z2 / radius;
            
            // Vertex 3: current ring, next segment
            float x3 = radius * sinf(phi1) * cosf(theta2);
            float y3 = radius * cosf(phi1);
            float z3 = radius * sinf(phi1) * sinf(theta2);
            float nx3 = x3 / radius, ny3 = y3 / radius, nz3 = z3 / radius;
            
            // Vertex 4: next ring, next segment
            float x4 = radius * sinf(phi2) * cosf(theta2);
            float y4 = radius * cosf(phi2);
            float z4 = radius * sinf(phi2) * sinf(theta2);
            float nx4 = x4 / radius, ny4 = y4 / radius, nz4 = z4 / radius;
            
            // First triangle: 1-2-3
            glNormal3f(nx1, ny1, nz1);
            glVertex3f(x1, y1, z1);
            glNormal3f(nx2, ny2, nz2);
            glVertex3f(x2, y2, z2);
            glNormal3f(nx3, ny3, nz3);
            glVertex3f(x3, y3, z3);
            
            // Second triangle: 2-4-3
            glNormal3f(nx2, ny2, nz2);
            glVertex3f(x2, y2, z2);
            glNormal3f(nx4, ny4, nz4);
            glVertex3f(x4, y4, z4);
            glNormal3f(nx3, ny3, nz3);
            glVertex3f(x3, y3, z3);
        }
    }
    
    glEnd();
}

void GraphicsEngine3D::RenderCylinderImmediate(float radius, float height, int segments) {
    float halfHeight = height * 0.5f;
    
    // Render cylinder sides
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < segments; i++) {
        float theta1 = 2.0f * (float)M_PI * i / segments;
        float theta2 = 2.0f * (float)M_PI * (i + 1) / segments;
        
        float x1 = radius * cosf(theta1);
        float z1 = radius * sinf(theta1);
        float x2 = radius * cosf(theta2);
        float z2 = radius * sinf(theta2);
        
        float nx1 = cosf(theta1), nz1 = sinf(theta1);
        float nx2 = cosf(theta2), nz2 = sinf(theta2);
        
        // Side quad (2 triangles)
        // Triangle 1
        glNormal3f(nx1, 0.0f, nz1);
        glVertex3f(x1, -halfHeight, z1);
        glNormal3f(nx2, 0.0f, nz2);
        glVertex3f(x2, -halfHeight, z2);
        glNormal3f(nx1, 0.0f, nz1);
        glVertex3f(x1, halfHeight, z1);
        
        // Triangle 2
        glNormal3f(nx2, 0.0f, nz2);
        glVertex3f(x2, -halfHeight, z2);
        glNormal3f(nx2, 0.0f, nz2);
        glVertex3f(x2, halfHeight, z2);
        glNormal3f(nx1, 0.0f, nz1);
        glVertex3f(x1, halfHeight, z1);
    }
    glEnd();
    
    // Render top and bottom caps
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < segments; i++) {
        float theta1 = 2.0f * (float)M_PI * i / segments;
        float theta2 = 2.0f * (float)M_PI * (i + 1) / segments;
        
        float x1 = radius * cosf(theta1);
        float z1 = radius * sinf(theta1);
        float x2 = radius * cosf(theta2);
        float z2 = radius * sinf(theta2);
        
        // Top cap
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, halfHeight, 0.0f);
        glVertex3f(x1, halfHeight, z1);
        glVertex3f(x2, halfHeight, z2);
        
        // Bottom cap
        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(0.0f, -halfHeight, 0.0f);
        glVertex3f(x2, -halfHeight, z2);
        glVertex3f(x1, -halfHeight, z1);
    }
    glEnd();
}

void GraphicsEngine3D::RenderPlaneImmediate(float width, float height) {
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    glBegin(GL_TRIANGLES);
    
    // First triangle
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-halfWidth, 0.0f, -halfHeight);
    glVertex3f(halfWidth, 0.0f, -halfHeight);
    glVertex3f(halfWidth, 0.0f, halfHeight);
    
    // Second triangle
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-halfWidth, 0.0f, -halfHeight);
    glVertex3f(halfWidth, 0.0f, halfHeight);
    glVertex3f(-halfWidth, 0.0f, halfHeight);
    
    glEnd();
}
