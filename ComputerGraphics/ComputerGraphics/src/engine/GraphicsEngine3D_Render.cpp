/**
 * @file GraphicsEngine3D_Render.cpp
 * @brief 3D图形引擎渲染模块
 * 
 * 本文件包含3D图形引擎的渲染功能：
 * - 主渲染函数(Render)
 * - 固定管线渲染(RenderWithFixedPipeline)
 * - 立即模式图元渲染(RenderCubeImmediate等)
 * 
 * 渲染流程说明：
 * 1. 清除颜色缓冲和深度缓冲
 * 2. 设置投影矩阵（透视投影）
 * 3. 设置视图矩阵（摄像机位置和朝向）
 * 4. 设置光照参数
 * 5. 遍历所有图形，设置模型矩阵和材质，执行绘制
 * 6. 交换前后缓冲
 * 
 * Phong光照模型说明：
 * Phong光照模型由三个分量组成：
 * - 环境光(Ambient): 模拟间接光照，使物体在阴影中也可见
 * - 漫反射(Diffuse): 光线照射到粗糙表面后向各方向均匀散射
 * - 镜面反射(Specular): 光线照射到光滑表面产生的高光效果
 * 
 * 最终颜色 = 环境光 + 漫反射 + 镜面反射
 * 
 * @author 计算机图形学项目组
 */

#include "GraphicsEngine3D.h"
#include "OpenGLFunctions.h"
#include "../math/Matrix4.h"
#include <gl/GL.h>
#include <cmath>

// 取消Windows宏定义，避免与参数名冲突
#undef near
#undef far

// 外部声明OpenGL函数指针（在GraphicsEngine3D_Core.cpp中定义）
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

// 额外的OpenGL函数指针类型
typedef void (APIENTRY *PFNGLDRAWELEMENTSPROC_EXT)(GLenum mode, GLsizei count, GLenum type, const void *indices);
typedef void (APIENTRY *PFNGLUSEPROGRAMPROC_EXT)(GLuint program);
typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC_EXT)(GLuint program, const GLchar *name);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC_EXT)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY *PFNGLUNIFORM3FPROC_EXT)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRY *PFNGLUNIFORM1FPROC_EXT)(GLint location, GLfloat v0);
typedef void (APIENTRY *PFNGLUNIFORM1IPROC_EXT)(GLint location, GLint v0);
typedef void (APIENTRY *PFNGLACTIVETEXTUREPROC_EXT)(GLenum texture);
typedef void (APIENTRY *PFNGLBINDTEXTUREPROC_EXT)(GLenum target, GLuint texture);

// 外部声明额外的OpenGL函数指针
extern PFNGLDRAWELEMENTSPROC_EXT glDrawElementsExt;
extern PFNGLUSEPROGRAMPROC_EXT glUseProgramExt;
extern PFNGLGETUNIFORMLOCATIONPROC_EXT glGetUniformLocationExt;
extern PFNGLUNIFORMMATRIX4FVPROC_EXT glUniformMatrix4fvExt;
extern PFNGLUNIFORM3FPROC_EXT glUniform3fExt;
extern PFNGLUNIFORM1FPROC_EXT glUniform1fExt;
extern PFNGLUNIFORM1IPROC_EXT glUniform1iExt;
extern PFNGLACTIVETEXTUREPROC_EXT glActiveTextureExt;
extern PFNGLBINDTEXTUREPROC_EXT glBindTextureExt;

// ============================================================================
// 主渲染函数
// ============================================================================

/**
 * @brief 渲染3D场景
 * 
 * 这是3D图形引擎的主渲染入口，负责绘制整个3D场景。
 * 当前实现使用固定管线渲染，以确保兼容性。
 * 
 * 渲染流程：
 * 1. 检查引擎是否已初始化
 * 2. 激活OpenGL上下文
 * 3. 清除颜色和深度缓冲
 * 4. 调用固定管线渲染函数
 * 5. 交换前后缓冲（双缓冲）
 */
void GraphicsEngine3D::Render() {
    if (!isInitialized) {
        return;
    }
    
    // 确保OpenGL上下文是当前的
    wglMakeCurrent(hdc, hglrc);
    
    // 清除颜色缓冲和深度缓冲
    // GL_COLOR_BUFFER_BIT: 清除颜色缓冲（背景色）
    // GL_DEPTH_BUFFER_BIT: 清除深度缓冲（Z-buffer）
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 使用固定管线渲染（兼容性更好）
    // 注：可编程管线渲染代码保留在下方，但当前未启用
    RenderWithFixedPipeline();
    
    // 交换前后缓冲，显示渲染结果
    SwapBuffers(hdc);
    return;
    
    // ========================================================================
    // 以下是可编程管线渲染代码（当前未启用）
    // 使用着色器程序进行渲染，支持更高级的光照效果
    // ========================================================================
    
    if (shaderProgram == 0) {
        RenderWithFixedPipeline();
        SwapBuffers(hdc);
        return;
    }
    
    // 获取窗口尺寸，计算宽高比
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width == 0 || height == 0) return;
    
    float aspectRatio = (float)width / (float)height;
    
    // 设置视口
    glViewport(0, 0, width, height);
    
    // 激活着色器程序
    glUseProgramExt(shaderProgram);
    
    // 计算摄像机位置（球坐标转笛卡尔坐标）
    float cameraX = camera.targetX + camera.distance * cosf(camera.angleY * (float)M_PI / 180.0f) * cosf(camera.angleX * (float)M_PI / 180.0f);
    float cameraY = camera.targetY + camera.distance * sinf(camera.angleY * (float)M_PI / 180.0f);
    float cameraZ = camera.targetZ + camera.distance * cosf(camera.angleY * (float)M_PI / 180.0f) * sinf(camera.angleX * (float)M_PI / 180.0f);
    
    // 创建投影矩阵和视图矩阵
    Matrix4 projection = Matrix4::perspective(45.0f * (float)M_PI / 180.0f, aspectRatio, 0.1f, 100.0f);
    Matrix4 view = Matrix4::lookAt(cameraX, cameraY, cameraZ,
                                   camera.targetX, camera.targetY, camera.targetZ,
                                   0.0f, 1.0f, 0.0f);
    
    // 设置投影和视图矩阵uniform
    int projLoc = glGetUniformLocationExt(shaderProgram, "projection");
    int viewLoc = glGetUniformLocationExt(shaderProgram, "view");
    int modelLoc = glGetUniformLocationExt(shaderProgram, "model");
    
    if (projLoc >= 0) glUniformMatrix4fvExt(projLoc, 1, GL_FALSE, projection.m);
    if (viewLoc >= 0) glUniformMatrix4fvExt(viewLoc, 1, GL_FALSE, view.m);
    
    // 设置Phong光照模型参数
    int lightPosLoc = glGetUniformLocationExt(shaderProgram, "lightPos");
    int lightColorLoc = glGetUniformLocationExt(shaderProgram, "lightColor");
    int lightAmbientIntensityLoc = glGetUniformLocationExt(shaderProgram, "lightAmbientIntensity");
    int lightDiffuseIntensityLoc = glGetUniformLocationExt(shaderProgram, "lightDiffuseIntensity");
    int lightSpecularIntensityLoc = glGetUniformLocationExt(shaderProgram, "lightSpecularIntensity");
    int viewPosLoc = glGetUniformLocationExt(shaderProgram, "viewPos");
    
    if (lightPosLoc >= 0) glUniform3fExt(lightPosLoc, light.positionX, light.positionY, light.positionZ);
    if (lightColorLoc >= 0) glUniform3fExt(lightColorLoc, light.color[0], light.color[1], light.color[2]);
    if (lightAmbientIntensityLoc >= 0) glUniform1fExt(lightAmbientIntensityLoc, light.ambientIntensity);
    if (lightDiffuseIntensityLoc >= 0) glUniform1fExt(lightDiffuseIntensityLoc, light.diffuseIntensity);
    if (lightSpecularIntensityLoc >= 0) glUniform1fExt(lightSpecularIntensityLoc, light.specularIntensity);
    if (viewPosLoc >= 0) glUniform3fExt(viewPosLoc, cameraX, cameraY, cameraZ);
    
    // 渲染所有图形
    for (size_t i = 0; i < shapes.size(); i++) {
        const Shape3D& shape = shapes[i];
        
        // 计算模型矩阵（平移 * 旋转 * 缩放）
        Matrix4 model = Matrix4::translate(shape.positionX, shape.positionY, shape.positionZ);
        model = model.multiply(Matrix4::rotateZ(shape.rotationZ * (float)M_PI / 180.0f));
        model = model.multiply(Matrix4::rotateY(shape.rotationY * (float)M_PI / 180.0f));
        model = model.multiply(Matrix4::rotateX(shape.rotationX * (float)M_PI / 180.0f));
        model = model.multiply(Matrix4::scale(shape.scaleX, shape.scaleY, shape.scaleZ));
        
        if (modelLoc >= 0) glUniformMatrix4fvExt(modelLoc, 1, GL_FALSE, model.m);
        
        // 设置材质属性
        int ambientLoc = glGetUniformLocationExt(shaderProgram, "ambient");
        int diffuseLoc = glGetUniformLocationExt(shaderProgram, "diffuse");
        int specularLoc = glGetUniformLocationExt(shaderProgram, "specular");
        int shininessLoc = glGetUniformLocationExt(shaderProgram, "shininess");
        int useTextureLoc = glGetUniformLocationExt(shaderProgram, "useTexture");
        
        // 选中的图形使用黄色高亮显示
        float ambient[3], diffuse[3], specular[3];
        if (shape.selected) {
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
        
        // 绑定纹理
        if (shape.hasTexture && shape.textureID != 0) {
            glActiveTextureExt(GL_TEXTURE0);
            glBindTextureExt(GL_TEXTURE_2D, shape.textureID);
        }
        
        // 绘制图形
        if (shape.VAO != 0) {
            glBindVertexArray(shape.VAO);
            glDrawElementsExt(GL_TRIANGLES, (GLsizei)shape.indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
    
    SwapBuffers(hdc);
}

// ============================================================================
// 固定管线渲染
// ============================================================================

/**
 * @brief 使用OpenGL固定管线渲染3D场景
 * 
 * 固定管线是OpenGL 1.x/2.x的传统渲染方式，
 * 虽然功能有限，但兼容性好，适合教学演示。
 * 
 * 渲染流程：
 * 1. 设置视口和投影矩阵（透视投影）
 * 2. 设置视图矩阵（摄像机变换）
 * 3. 启用光照并配置光源参数
 * 4. 遍历所有图形，应用变换和材质，绘制图元
 * 5. 禁用光照
 * 
 * Phong光照模型在固定管线中的实现：
 * - GL_AMBIENT: 环境光分量
 * - GL_DIFFUSE: 漫反射分量
 * - GL_SPECULAR: 镜面反射分量
 * - GL_SHININESS: 高光指数（控制高光大小）
 */
void GraphicsEngine3D::RenderWithFixedPipeline() {
    // 获取窗口尺寸
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width == 0 || height == 0) return;
    
    float aspectRatio = (float)width / (float)height;
    
    // 设置视口
    glViewport(0, 0, width, height);
    
    // ========================================================================
    // 设置投影矩阵（透视投影）
    // ========================================================================
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // 透视投影参数
    float fov = 45.0f;           // 视场角（Field of View）
    float nearPlane = 0.1f;      // 近裁剪面
    float farPlane = 100.0f;     // 远裁剪面
    
    // 计算视锥体边界
    float top = nearPlane * tanf(fov * (float)M_PI / 360.0f);
    float bottom = -top;
    float right = top * aspectRatio;
    float left = -right;
    
    // 设置透视投影矩阵
    glFrustum(left, right, bottom, top, nearPlane, farPlane);
    
    // ========================================================================
    // 设置视图矩阵（摄像机变换）
    // ========================================================================
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // 计算摄像机位置（球坐标转笛卡尔坐标）
    // 摄像机绕目标点旋转，距离由camera.distance控制
    float cameraX = camera.targetX + camera.distance * cosf(camera.angleY * (float)M_PI / 180.0f) * cosf(camera.angleX * (float)M_PI / 180.0f);
    float cameraY = camera.targetY + camera.distance * sinf(camera.angleY * (float)M_PI / 180.0f);
    float cameraZ = camera.targetZ + camera.distance * cosf(camera.angleY * (float)M_PI / 180.0f) * sinf(camera.angleX * (float)M_PI / 180.0f);
    
    // 手动计算视图矩阵（lookAt矩阵）
    // 因为gluLookAt可能不可用，所以手动实现
    
    // 计算前向向量（从摄像机指向目标）
    float fx = camera.targetX - cameraX;
    float fy = camera.targetY - cameraY;
    float fz = camera.targetZ - cameraZ;
    float flen = sqrtf(fx*fx + fy*fy + fz*fz);
    fx /= flen; fy /= flen; fz /= flen;
    
    // 计算右向量（前向 × 上向）
    float upX = 0.0f, upY = 1.0f, upZ = 0.0f;
    float rx = fy * upZ - fz * upY;
    float ry = fz * upX - fx * upZ;
    float rz = fx * upY - fy * upX;
    float rlen = sqrtf(rx*rx + ry*ry + rz*rz);
    rx /= rlen; ry /= rlen; rz /= rlen;
    
    // 计算真正的上向量（右向 × 前向）
    float ux = ry * fz - rz * fy;
    float uy = rz * fx - rx * fz;
    float uz = rx * fy - ry * fx;
    
    // 构建视图矩阵（列主序）
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
    
    // ========================================================================
    // 启用OpenGL固定管线光照
    // 实现Phong光照模型
    // ========================================================================
    glEnable(GL_LIGHTING);    // 启用光照计算
    glEnable(GL_LIGHT0);      // 启用光源0
    glEnable(GL_NORMALIZE);   // 自动归一化法线（缩放后法线可能不是单位向量）
    
    // 启用双面光照，确保背面也能正确显示
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    
    // 设置全局环境光（场景基础亮度）
    float globalAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    
    // 设置光源位置（w=1.0表示点光源，w=0.0表示方向光）
    float lightPos[] = {light.positionX, light.positionY, light.positionZ, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    // 设置环境光分量（Ambient）
    // 环境光模拟间接光照，使物体在阴影中也可见
    float ambientLight[] = {
        light.color[0] * light.ambientIntensity, 
        light.color[1] * light.ambientIntensity, 
        light.color[2] * light.ambientIntensity, 
        1.0f
    };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    
    // 设置漫反射分量（Diffuse）
    // 漫反射是光照的主要贡献，与表面法线和光线方向的夹角有关
    float diffuseLight[] = {
        light.color[0] * light.diffuseIntensity, 
        light.color[1] * light.diffuseIntensity, 
        light.color[2] * light.diffuseIntensity, 
        1.0f
    };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    
    // 设置镜面反射分量（Specular）
    // 镜面反射产生高光效果，与视线方向和反射方向有关
    float specularLight[] = {
        light.color[0] * light.specularIntensity, 
        light.color[1] * light.specularIntensity, 
        light.color[2] * light.specularIntensity, 
        1.0f
    };
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    
    // ========================================================================
    // 渲染所有3D图形
    // ========================================================================
    for (size_t i = 0; i < shapes.size(); i++) {
        const Shape3D& shape = shapes[i];
        
        glPushMatrix();  // 保存当前矩阵
        
        // 应用模型变换（平移 → 旋转 → 缩放）
        glTranslatef(shape.positionX, shape.positionY, shape.positionZ);
        glRotatef(shape.rotationZ, 0.0f, 0.0f, 1.0f);  // 绕Z轴旋转
        glRotatef(shape.rotationY, 0.0f, 1.0f, 0.0f);  // 绕Y轴旋转
        glRotatef(shape.rotationX, 1.0f, 0.0f, 0.0f);  // 绕X轴旋转
        glScalef(shape.scaleX, shape.scaleY, shape.scaleZ);
        
        // 设置材质属性
        float ambient[4], diffuse[4], specular[4];
        
        if (shape.selected) {
            // 选中的图形使用黄色高亮
            ambient[0] = 0.3f; ambient[1] = 0.3f; ambient[2] = 0.1f; ambient[3] = 1.0f;
            diffuse[0] = 1.0f; diffuse[1] = 1.0f; diffuse[2] = 0.3f; diffuse[3] = 1.0f;
            specular[0] = 1.0f; specular[1] = 1.0f; specular[2] = 0.5f; specular[3] = 1.0f;
        } else {
            // 使用图形自身的材质颜色
            ambient[0] = shape.ambient[0]; ambient[1] = shape.ambient[1]; ambient[2] = shape.ambient[2]; ambient[3] = 1.0f;
            diffuse[0] = shape.diffuse[0]; diffuse[1] = shape.diffuse[1]; diffuse[2] = shape.diffuse[2]; diffuse[3] = 1.0f;
            specular[0] = shape.specular[0]; specular[1] = shape.specular[1]; specular[2] = shape.specular[2]; specular[3] = 1.0f;
        }
        
        // 设置材质属性（正面和背面）
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shape.shininess);
        
        // 启用纹理（如果有）
        bool textureEnabled = false;
        if (shape.hasTexture && shape.textureID != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, shape.textureID);
            textureEnabled = true;
        }
        
        // 根据图形类型调用相应的渲染函数
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
        
        // 禁用纹理
        if (textureEnabled) {
            glDisable(GL_TEXTURE_2D);
        }
        
        glPopMatrix();  // 恢复矩阵
    }
    
    // 禁用光照
    glDisable(GL_LIGHTING);
}


// ============================================================================
// 立即模式图元渲染函数
// 这些函数使用OpenGL立即模式(glBegin/glEnd)绘制基本3D图元
// ============================================================================

/**
 * @brief 使用立即模式渲染立方体
 * @param size 立方体边长
 * 
 * 立方体由6个面组成，每个面是一个四边形。
 * 每个顶点需要指定：
 * - 法线向量（用于光照计算）
 * - 纹理坐标（用于纹理映射）
 * - 顶点位置
 */
void GraphicsEngine3D::RenderCubeImmediate(float size) {
    float halfSize = size * 0.5f;
    
    glBegin(GL_QUADS);
    
    // 前面 (Z+)
    glNormal3f(0.0f, 0.0f, 1.0f);  // 法线指向Z正方向
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize,  halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( halfSize, -halfSize,  halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( halfSize,  halfSize,  halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize,  halfSize,  halfSize);
    
    // 后面 (Z-)
    glNormal3f(0.0f, 0.0f, -1.0f);  // 法线指向Z负方向
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize,  halfSize, -halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( halfSize,  halfSize, -halfSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( halfSize, -halfSize, -halfSize);
    
    // 顶面 (Y+)
    glNormal3f(0.0f, 1.0f, 0.0f);  // 法线指向Y正方向
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize,  halfSize, -halfSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize,  halfSize,  halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( halfSize,  halfSize,  halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( halfSize,  halfSize, -halfSize);
    
    // 底面 (Y-)
    glNormal3f(0.0f, -1.0f, 0.0f);  // 法线指向Y负方向
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( halfSize, -halfSize, -halfSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( halfSize, -halfSize,  halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize,  halfSize);
    
    // 右面 (X+)
    glNormal3f(1.0f, 0.0f, 0.0f);  // 法线指向X正方向
    glTexCoord2f(1.0f, 0.0f); glVertex3f( halfSize, -halfSize, -halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( halfSize,  halfSize, -halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( halfSize,  halfSize,  halfSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( halfSize, -halfSize,  halfSize);
    
    // 左面 (X-)
    glNormal3f(-1.0f, 0.0f, 0.0f);  // 法线指向X负方向
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize,  halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize,  halfSize,  halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize,  halfSize, -halfSize);
    
    glEnd();
}

/**
 * @brief 使用立即模式渲染球体
 * @param radius 球体半径
 * @param segments 经度方向的分段数（水平方向）
 * @param rings 纬度方向的分段数（垂直方向）
 * 
 * 球体使用球坐标系生成顶点：
 * - phi: 纬度角，从0到π（北极到南极）
 * - theta: 经度角，从0到2π（绕Y轴一圈）
 * 
 * 顶点位置公式：
 * x = r * sin(phi) * cos(theta)
 * y = r * cos(phi)
 * z = r * sin(phi) * sin(theta)
 * 
 * 法线向量等于归一化的顶点位置（球心在原点）
 */
void GraphicsEngine3D::RenderSphereImmediate(float radius, int segments, int rings) {
    glBegin(GL_TRIANGLES);
    
    for (int ring = 0; ring < rings; ring++) {
        // 当前环和下一环的纬度角
        float phi1 = (float)M_PI * ring / rings;
        float phi2 = (float)M_PI * (ring + 1) / rings;
        
        for (int seg = 0; seg < segments; seg++) {
            // 当前段和下一段的经度角
            float theta1 = 2.0f * (float)M_PI * seg / segments;
            float theta2 = 2.0f * (float)M_PI * (seg + 1) / segments;
            
            // 计算纹理坐标（球面映射）
            float u1 = (float)seg / segments;
            float u2 = (float)(seg + 1) / segments;
            float v1 = (float)ring / rings;
            float v2 = (float)(ring + 1) / rings;
            
            // 计算四个顶点位置和法线
            // 顶点1: 当前环，当前段
            float x1 = radius * sinf(phi1) * cosf(theta1);
            float y1 = radius * cosf(phi1);
            float z1 = radius * sinf(phi1) * sinf(theta1);
            float nx1 = x1 / radius, ny1 = y1 / radius, nz1 = z1 / radius;
            
            // 顶点2: 下一环，当前段
            float x2 = radius * sinf(phi2) * cosf(theta1);
            float y2 = radius * cosf(phi2);
            float z2 = radius * sinf(phi2) * sinf(theta1);
            float nx2 = x2 / radius, ny2 = y2 / radius, nz2 = z2 / radius;
            
            // 顶点3: 当前环，下一段
            float x3 = radius * sinf(phi1) * cosf(theta2);
            float y3 = radius * cosf(phi1);
            float z3 = radius * sinf(phi1) * sinf(theta2);
            float nx3 = x3 / radius, ny3 = y3 / radius, nz3 = z3 / radius;
            
            // 顶点4: 下一环，下一段
            float x4 = radius * sinf(phi2) * cosf(theta2);
            float y4 = radius * cosf(phi2);
            float z4 = radius * sinf(phi2) * sinf(theta2);
            float nx4 = x4 / radius, ny4 = y4 / radius, nz4 = z4 / radius;
            
            // 第一个三角形: 1-2-3
            glTexCoord2f(u1, v1); glNormal3f(nx1, ny1, nz1); glVertex3f(x1, y1, z1);
            glTexCoord2f(u1, v2); glNormal3f(nx2, ny2, nz2); glVertex3f(x2, y2, z2);
            glTexCoord2f(u2, v1); glNormal3f(nx3, ny3, nz3); glVertex3f(x3, y3, z3);
            
            // 第二个三角形: 2-4-3
            glTexCoord2f(u1, v2); glNormal3f(nx2, ny2, nz2); glVertex3f(x2, y2, z2);
            glTexCoord2f(u2, v2); glNormal3f(nx4, ny4, nz4); glVertex3f(x4, y4, z4);
            glTexCoord2f(u2, v1); glNormal3f(nx3, ny3, nz3); glVertex3f(x3, y3, z3);
        }
    }
    
    glEnd();
}

/**
 * @brief 使用立即模式渲染圆柱体
 * @param radius 圆柱体底面半径
 * @param height 圆柱体高度
 * @param segments 圆周方向的分段数
 * 
 * 圆柱体由三部分组成：
 * 1. 侧面：由多个四边形组成的曲面
 * 2. 顶面：圆形盖子
 * 3. 底面：圆形盖子
 * 
 * 侧面法线指向圆心外侧（水平方向）
 * 顶面法线指向Y+，底面法线指向Y-
 */
void GraphicsEngine3D::RenderCylinderImmediate(float radius, float height, int segments) {
    float halfHeight = height * 0.5f;
    
    // ========================================
    // 渲染圆柱体侧面
    // ========================================
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < segments; i++) {
        // 当前段和下一段的角度
        float theta1 = 2.0f * (float)M_PI * i / segments;
        float theta2 = 2.0f * (float)M_PI * (i + 1) / segments;
        
        // 计算顶点位置
        float x1 = radius * cosf(theta1);
        float z1 = radius * sinf(theta1);
        float x2 = radius * cosf(theta2);
        float z2 = radius * sinf(theta2);
        
        // 法线向量（指向圆心外侧）
        float nx1 = cosf(theta1), nz1 = sinf(theta1);
        float nx2 = cosf(theta2), nz2 = sinf(theta2);
        
        // 纹理坐标（圆柱面展开）
        float u1 = (float)i / segments;
        float u2 = (float)(i + 1) / segments;
        
        // 侧面四边形（两个三角形）
        // 三角形1
        glTexCoord2f(u1, 0.0f); glNormal3f(nx1, 0.0f, nz1); glVertex3f(x1, -halfHeight, z1);
        glTexCoord2f(u2, 0.0f); glNormal3f(nx2, 0.0f, nz2); glVertex3f(x2, -halfHeight, z2);
        glTexCoord2f(u1, 1.0f); glNormal3f(nx1, 0.0f, nz1); glVertex3f(x1, halfHeight, z1);
        
        // 三角形2
        glTexCoord2f(u2, 0.0f); glNormal3f(nx2, 0.0f, nz2); glVertex3f(x2, -halfHeight, z2);
        glTexCoord2f(u2, 1.0f); glNormal3f(nx2, 0.0f, nz2); glVertex3f(x2, halfHeight, z2);
        glTexCoord2f(u1, 1.0f); glNormal3f(nx1, 0.0f, nz1); glVertex3f(x1, halfHeight, z1);
    }
    glEnd();
    
    // ========================================
    // 渲染顶面和底面（圆形盖子）
    // ========================================
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < segments; i++) {
        float theta1 = 2.0f * (float)M_PI * i / segments;
        float theta2 = 2.0f * (float)M_PI * (i + 1) / segments;
        
        float x1 = radius * cosf(theta1);
        float z1 = radius * sinf(theta1);
        float x2 = radius * cosf(theta2);
        float z2 = radius * sinf(theta2);
        
        // 纹理坐标（径向映射）
        float tu1 = (cosf(theta1) + 1.0f) * 0.5f;
        float tv1 = (sinf(theta1) + 1.0f) * 0.5f;
        float tu2 = (cosf(theta2) + 1.0f) * 0.5f;
        float tv2 = (sinf(theta2) + 1.0f) * 0.5f;
        
        // 顶面（法线指向Y+）
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.5f, 0.5f); glVertex3f(0.0f, halfHeight, 0.0f);  // 圆心
        glTexCoord2f(tu1, tv1); glVertex3f(x1, halfHeight, z1);
        glTexCoord2f(tu2, tv2); glVertex3f(x2, halfHeight, z2);
        
        // 底面（法线指向Y-）
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.5f, 0.5f); glVertex3f(0.0f, -halfHeight, 0.0f);  // 圆心
        glTexCoord2f(tu2, tv2); glVertex3f(x2, -halfHeight, z2);
        glTexCoord2f(tu1, tv1); glVertex3f(x1, -halfHeight, z1);
    }
    glEnd();
}

/**
 * @brief 使用立即模式渲染平面
 * @param width 平面宽度（X方向）
 * @param height 平面高度（Z方向）
 * 
 * 平面是一个水平放置的矩形，位于Y=0平面上。
 * 法线指向Y+方向。
 * 由两个三角形组成。
 */
void GraphicsEngine3D::RenderPlaneImmediate(float width, float height) {
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    glBegin(GL_TRIANGLES);
    
    // 第一个三角形
    glNormal3f(0.0f, 1.0f, 0.0f);  // 法线指向Y+
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfWidth, 0.0f, -halfHeight);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(halfWidth, 0.0f, -halfHeight);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(halfWidth, 0.0f, halfHeight);
    
    // 第二个三角形
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfWidth, 0.0f, -halfHeight);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(halfWidth, 0.0f, halfHeight);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfWidth, 0.0f, halfHeight);
    
    glEnd();
}
