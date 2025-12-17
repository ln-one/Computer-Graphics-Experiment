/**
 * @dir ui
 * @brief 用户界面目录
 * 
 * 本目录包含对话框和菜单相关代码，负责用户界面的交互。
 * 
 * 目录内容：
 * 
 * 【对话框类】
 * - Dialogs3D.h           - 3D对话框类声明
 * - TransformDialog3D.cpp - 变换参数对话框（平移、旋转、缩放）
 * - LightingDialog.cpp    - 光照设置对话框（光源位置、颜色、强度）
 * - MaterialDialog.cpp    - 材质编辑对话框（环境光、漫反射、镜面反射）
 * - TextureDialog.cpp     - 纹理设置对话框（加载、应用纹理）
 * 
 * 【菜单定义】
 * - MenuIDs.h             - 菜单ID定义，用于消息处理
 * 
 * 使用说明：
 * 所有对话框类都提供静态 Show() 方法，显示模态对话框。
 * 对话框使用 Windows API 创建，支持中文界面。
 */

#pragma once
#include "../core/Shape3D.h"
#include <windows.h>
#include <string>

/**
 * @file Dialogs3D.h
 * @brief 3D对话框类定义
 * @author ln1.opensource@gmail.com
 * 
 * 定义了3D图形系统中使用的各种对话框类，包括变换参数编辑、
 * 光照设置、材质编辑和纹理设置对话框
 */

// 前向声明Light结构体
struct Light;

// 前向声明GraphicsEngine3D类
class GraphicsEngine3D;

/**
 * @class TransformDialog3D
 * @brief 3D变换参数编辑对话框
 * 
 * 允许用户精确设置3D图形的位置、旋转和缩放参数
 * 通过对话框界面提供9个输入框（位置XYZ、旋转XYZ、缩放XYZ）
 */
class TransformDialog3D {
public:
    /**
     * @brief 显示变换参数对话框
     * @param parent 父窗口句柄
     * @param shape 要编辑的3D图形指针
     * @return 用户点击确定返回true，取消返回false
     * 
     * 显示模态对话框，允许用户编辑图形的变换参数
     * 对话框会显示当前图形的参数值，用户可以修改后确认或取消
     */
    static bool Show(HWND parent, Shape3D* shape);
    
private:
    /**
     * @brief 对话框过程函数
     * @param hwnd 对话框窗口句柄
     * @param msg 消息类型
     * @param wParam 消息参数1
     * @param lParam 消息参数2
     * @return 消息处理结果
     * 
     * 处理对话框的所有Windows消息，包括初始化、按钮点击等
     */
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, 
                                       WPARAM wParam, LPARAM lParam);
    
    /**
     * @brief 验证输入值是否为有效浮点数
     * @param hwnd 对话框窗口句柄
     * @param controlID 控件ID
     * @param outValue 输出的浮点数值
     * @return 输入有效返回true，无效返回false
     */
    static bool ValidateFloatInput(HWND hwnd, int controlID, float& outValue);
    
    /**
     * @brief 设置编辑框的浮点数值
     * @param hwnd 对话框窗口句柄
     * @param controlID 控件ID
     * @param value 要设置的浮点数值
     */
    static void SetFloatValue(HWND hwnd, int controlID, float value);
    
    static Shape3D* s_currentShape;  ///< 当前正在编辑的图形指针
};

/**
 * @class LightingDialog
 * @brief 3D光照设置对话框
 * 
 * 允许用户设置3D场景的光照参数，包括：
 * - 光源位置（X, Y, Z坐标）
 * - 光照强度（环境光、漫反射、镜面反射）
 * - 光源颜色（RGB）
 */
class LightingDialog {
public:
    /**
     * @brief 显示光照设置对话框
     * @param parent 父窗口句柄
     * @param light 要编辑的光源指针
     * @return 用户点击确定返回true，取消返回false
     * 
     * 显示模态对话框，允许用户编辑光照参数
     * 对话框会显示当前光源的参数值，用户可以修改后确认或取消
     */
    static bool Show(HWND parent, Light* light);
    
private:
    /**
     * @brief 对话框过程函数
     * @param hwnd 对话框窗口句柄
     * @param msg 消息类型
     * @param wParam 消息参数1
     * @param lParam 消息参数2
     * @return 消息处理结果
     * 
     * 处理对话框的所有Windows消息，包括初始化、按钮点击等
     */
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, 
                                       WPARAM wParam, LPARAM lParam);
    
    /**
     * @brief 验证输入值是否为有效浮点数
     * @param hwnd 对话框窗口句柄
     * @param controlID 控件ID
     * @param outValue 输出的浮点数值
     * @return 输入有效返回true，无效返回false
     */
    static bool ValidateFloatInput(HWND hwnd, int controlID, float& outValue);
    
    /**
     * @brief 设置编辑框的浮点数值
     * @param hwnd 对话框窗口句柄
     * @param controlID 控件ID
     * @param value 要设置的浮点数值
     */
    static void SetFloatValue(HWND hwnd, int controlID, float value);
    
    static Light* s_currentLight;  ///< 当前正在编辑的光源指针
};

/**
 * @class MaterialDialog
 * @brief 3D材质编辑对话框
 * 
 * 允许用户编辑3D图形的材质参数，包括：
 * - 环境光反射系数（RGB）
 * - 漫反射系数（RGB）
 * - 镜面反射系数（RGB）
 * - 光泽度（shininess）
 */
class MaterialDialog {
public:
    /**
     * @brief 显示材质编辑对话框
     * @param parent 父窗口句柄
     * @param shape 要编辑的3D图形指针
     * @return 用户点击确定返回true，取消返回false
     * 
     * 显示模态对话框，允许用户编辑图形的材质参数
     * 对话框会显示当前图形的材质值，用户可以修改后确认或取消
     */
    static bool Show(HWND parent, Shape3D* shape);
    
private:
    /**
     * @brief 对话框过程函数
     * @param hwnd 对话框窗口句柄
     * @param msg 消息类型
     * @param wParam 消息参数1
     * @param lParam 消息参数2
     * @return 消息处理结果
     * 
     * 处理对话框的所有Windows消息，包括初始化、按钮点击等
     */
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, 
                                       WPARAM wParam, LPARAM lParam);
    
    /**
     * @brief 验证输入值是否为有效浮点数
     * @param hwnd 对话框窗口句柄
     * @param controlID 控件ID
     * @param outValue 输出的浮点数值
     * @return 输入有效返回true，无效返回false
     */
    static bool ValidateFloatInput(HWND hwnd, int controlID, float& outValue);
    
    /**
     * @brief 设置编辑框的浮点数值
     * @param hwnd 对话框窗口句柄
     * @param controlID 控件ID
     * @param value 要设置的浮点数值
     */
    static void SetFloatValue(HWND hwnd, int controlID, float value);
    
    static Shape3D* s_currentShape;  ///< 当前正在编辑的图形指针
};

/**
 * @class TextureDialog
 * @brief 3D纹理设置对话框
 * 
 * 允许用户为3D图形设置纹理，包括：
 * - 选择纹理贴图文件（支持BMP、JPG、PNG等格式）
 * - 选择纹理映射方式（平面映射、球面映射、柱面映射等）
 * - 调整纹理的缩放和偏移参数
 * - 移除已应用的纹理
 */
class TextureDialog {
public:
    /**
     * @brief 显示纹理设置对话框
     * @param parent 父窗口句柄
     * @param shape 要编辑的3D图形指针
     * @return 用户点击确定返回true，取消返回false
     * 
     * 显示模态对话框，允许用户设置图形的纹理
     * 对话框会显示当前图形的纹理状态，用户可以选择纹理文件或移除纹理
     */
    static bool Show(HWND parent, Shape3D* shape);
    
private:
    /**
     * @brief 对话框过程函数
     * @param hwnd 对话框窗口句柄
     * @param msg 消息类型
     * @param wParam 消息参数1
     * @param lParam 消息参数2
     * @return 消息处理结果
     * 
     * 处理对话框的所有Windows消息，包括初始化、按钮点击等
     */
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, 
                                       WPARAM wParam, LPARAM lParam);
    
    /**
     * @brief 验证输入值是否为有效浮点数
     * @param hwnd 对话框窗口句柄
     * @param controlID 控件ID
     * @param outValue 输出的浮点数值
     * @return 输入有效返回true，无效返回false
     */
    static bool ValidateFloatInput(HWND hwnd, int controlID, float& outValue);
    
    /**
     * @brief 设置编辑框的浮点数值
     * @param hwnd 对话框窗口句柄
     * @param controlID 控件ID
     * @param value 要设置的浮点数值
     */
    static void SetFloatValue(HWND hwnd, int controlID, float value);
    
    /**
     * @brief 打开文件选择对话框
     * @param hwnd 父窗口句柄
     * @param filepath 输出的文件路径
     * @return 用户选择了文件返回true，取消返回false
     */
    static bool OpenFileDialog(HWND hwnd, std::string& filepath);
    
    static Shape3D* s_currentShape;  ///< 当前正在编辑的图形指针
    static std::string s_texturePath;  ///< 当前选择的纹理文件路径
};
