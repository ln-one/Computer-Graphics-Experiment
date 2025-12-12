#pragma once
#include <windows.h>

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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef int GLsizei;
typedef unsigned int GLuint;
typedef int GLint;
typedef unsigned char GLboolean;
typedef float GLfloat;
typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef unsigned int GLenum;
#endif

// OpenGL function pointers
typedef void (APIENTRY *PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void (APIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (APIENTRY *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void (APIENTRY *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRY *PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (APIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
typedef void (APIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);

// External function pointers (defined in GraphicsEngine3D.cpp)
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;