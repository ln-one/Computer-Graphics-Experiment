#include "GraphicsEngine3D.h"
#include "../algorithms/ShaderManager.h"
#include "../algorithms/MeshGenerator.h"
#include <gl/GL.h>
#include <cmath>

// OpenGL constants and types
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW 0x88E4
#define GL_TRIANGLES 0x0004
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_FALSE 0
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE_2D 0x0DE1
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84

typedef int GLsizei;
typedef unsigned int GLuint;
typedef int GLint;
typedef unsigned char GLboolean;
typedef float GLfloat;
typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef unsigned int GLenum;
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Simple matrix math functions for MVP calculation
struct Matrix4 {
    float m[16];
    
    Matrix4() {
        // Identity matrix
        for (int i = 0; i < 16; i++) m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }
    
    static Matrix4 perspective(float fov, float aspect, float near, float far) {
        Matrix4 result;
        float f = 1.0f / tan(fov * 0.5f);
        result.m[0] = f / aspect;
        result.m[5] = f;
        result.m[10] = (far + near) / (near - far);
        result.m[11] = -1.0f;
        result.m[14] = (2.0f * far * near) / (near - far);
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
        float flen = sqrt(fx*fx + fy*fy + fz*fz);
        fx /= flen; fy /= flen; fz /= flen;
        
        // Calculate right vector
        float rx = fy * upZ - fz * upY;
        float ry = fz * upX - fx * upZ;
        float rz = fx * upY - fy * upX;
        float rlen = sqrt(rx*rx + ry*ry + rz*rz);
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
        float c = cos(angle);
        float s = sin(angle);
        result.m[5] = c; result.m[6] = s;
        result.m[9] = -s; result.m[10] = c;
        return result;
    }
    
    static Matrix4 rotateY(float angle) {
        Matrix4 result;
        float c = cos(angle);
        float s = sin(angle);
        result.m[0] = c; result.m[2] = -s;
        result.m[8] = s; result.m[10] = c;
        return result;
    }
    
    static Matrix4 rotateZ(float angle) {
        Matrix4 result;
        float c = cos(angle);
        float s = sin(angle);
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

// OpenGL function pointers (basic implementation for demonstration)
typedef void (APIENTRY *PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void (APIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (APIENTRY *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void (APIENTRY *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRY *PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (APIENTRY *PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
typedef void (APIENTRY *PFNGLUSEPROGRAMPROC)(GLuint program);
typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY *PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRY *PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void (APIENTRY *PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (APIENTRY *PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (APIENTRY *PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (APIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
typedef void (APIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);

// Global function pointers (exported for use by other modules)
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYSPROC glBindVertexArray = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLDRAWELEMENTSPROC glDrawElements = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;
PFNGLUNIFORM3FPROC glUniform3f = nullptr;
PFNGLUNIFORM1FPROC glUniform1f = nullptr;
PFNGLUNIFORM1IPROC glUniform1i = nullptr;
PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr;
PFNGLBINDTEXTUREPROC glBindTexture = nullptr;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;

bool GraphicsEngine3D::LoadOpenGLFunctions() {
    // Load OpenGL extension functions
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
    glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    glDrawElements = (PFNGLDRAWELEMENTSPROC)wglGetProcAddress("glDrawElements");
    glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
    glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
    glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
    glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
    glBindTexture = (PFNGLBINDTEXTUREPROC)wglGetProcAddress("glBindTexture");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
    
    // Check if all functions were loaded successfully
    return (glGenVertexArrays && glBindVertexArray && glGenBuffers && 
            glBindBuffer && glBufferData && glVertexAttribPointer && 
            glEnableVertexAttribArray && glDrawElements && glUseProgram &&
            glGetUniformLocation && glUniformMatrix4fv && glUniform3f &&
            glUniform1f && glUniform1i && glActiveTexture && glBindTexture &&
            glDeleteVertexArrays && glDeleteBuffers);
}

void GraphicsEngine3D::Render() {
    if (!isInitialized || shaderProgram == 0) {
        return;
    }
    
    // Make sure our OpenGL context is current
    wglMakeCurrent(hdc, hglrc);
    
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
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
    glUseProgram(shaderProgram);
    
    // Calculate camera position
    float cameraX = camera.targetX + camera.distance * cos(camera.angleY * M_PI / 180.0f) * cos(camera.angleX * M_PI / 180.0f);
    float cameraY = camera.targetY + camera.distance * sin(camera.angleY * M_PI / 180.0f);
    float cameraZ = camera.targetZ + camera.distance * cos(camera.angleY * M_PI / 180.0f) * sin(camera.angleX * M_PI / 180.0f);
    
    // Create matrices
    Matrix4 projection = Matrix4::perspective(45.0f * M_PI / 180.0f, aspectRatio, 0.1f, 100.0f);
    Matrix4 view = Matrix4::lookAt(cameraX, cameraY, cameraZ,
                                   camera.targetX, camera.targetY, camera.targetZ,
                                   0.0f, 1.0f, 0.0f);
    
    // Set projection and view matrices
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    
    if (projLoc >= 0) glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.m);
    if (viewLoc >= 0) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.m);
    
    // Set light parameters
    int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    
    if (lightPosLoc >= 0) glUniform3f(lightPosLoc, light.positionX, light.positionY, light.positionZ);
    if (lightColorLoc >= 0) glUniform3f(lightColorLoc, light.color[0], light.color[1], light.color[2]);
    if (viewPosLoc >= 0) glUniform3f(viewPosLoc, cameraX, cameraY, cameraZ);
    
    // Render all shapes
    for (size_t i = 0; i < shapes.size(); i++) {
        const Shape3D& shape = shapes[i];
        
        // Calculate model matrix
        Matrix4 model = Matrix4::translate(shape.positionX, shape.positionY, shape.positionZ);
        model = model.multiply(Matrix4::rotateZ(shape.rotationZ * M_PI / 180.0f));
        model = model.multiply(Matrix4::rotateY(shape.rotationY * M_PI / 180.0f));
        model = model.multiply(Matrix4::rotateX(shape.rotationX * M_PI / 180.0f));
        model = model.multiply(Matrix4::scale(shape.scaleX, shape.scaleY, shape.scaleZ));
        
        if (modelLoc >= 0) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.m);
        
        // Set material properties
        int ambientLoc = glGetUniformLocation(shaderProgram, "ambient");
        int diffuseLoc = glGetUniformLocation(shaderProgram, "diffuse");
        int specularLoc = glGetUniformLocation(shaderProgram, "specular");
        int shininessLoc = glGetUniformLocation(shaderProgram, "shininess");
        int useTextureLoc = glGetUniformLocation(shaderProgram, "useTexture");
        
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
        
        if (ambientLoc >= 0) glUniform3f(ambientLoc, ambient[0], ambient[1], ambient[2]);
        if (diffuseLoc >= 0) glUniform3f(diffuseLoc, diffuse[0], diffuse[1], diffuse[2]);
        if (specularLoc >= 0) glUniform3f(specularLoc, specular[0], specular[1], specular[2]);
        if (shininessLoc >= 0) glUniform1f(shininessLoc, shape.shininess);
        if (useTextureLoc >= 0) glUniform1i(useTextureLoc, shape.hasTexture ? 1 : 0);
        
        // Bind texture if available
        if (shape.hasTexture && shape.textureID != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, shape.textureID);
        }
        
        // Render the shape
        if (shape.VAO != 0) {
            glBindVertexArray(shape.VAO);
            glDrawElements(GL_TRIANGLES, (GLsizei)shape.indices.size(), GL_UNSIGNED_INT, 0);
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
            // Record mouse position in view control mode
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
    
    if (currentMode == MODE_3D_VIEW_CONTROL) {
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
}

void GraphicsEngine3D::HandleSelection(int x, int y) {
    // Selection logic (to be implemented)
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
