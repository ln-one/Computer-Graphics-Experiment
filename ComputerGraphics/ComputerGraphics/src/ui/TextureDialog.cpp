/**
 * @file TextureDialog.cpp
 * @brief 3D纹理设置对话框实现
 * @author ln1.opensource@gmail.com
 * 
 * 实现了3D图形纹理设置对话框的功能，允许用户：
 * - 选择纹理贴图文件（支持BMP、JPG、PNG、TGA、GIF等格式）
 * - 选择纹理映射方式
 * - 移除已应用的纹理
 * 
 * 纹理映射原理说明：
 * 
 * 纹理映射（Texture Mapping）是将2D图像贴到3D物体表面的技术。
 * 主要步骤：
 * 1. 加载纹理图像到GPU内存
 * 2. 为每个顶点指定纹理坐标（UV坐标）
 * 3. 在渲染时，根据UV坐标从纹理中采样颜色
 * 
 * 纹理坐标系统：
 * - U轴：水平方向，范围0.0-1.0（左到右）
 * - V轴：垂直方向，范围0.0-1.0（下到上）
 * - (0,0) 在左下角，(1,1) 在右上角
 * 
 * 常见纹理映射方式：
 * 
 * 1. 平面映射（Planar Mapping）：
 *    - 将纹理沿某个平面投影到物体上
 *    - 适合平面或近似平面的物体
 *    - 缺点：在垂直于投影方向的面上会产生拉伸
 * 
 * 2. 球面映射（Spherical Mapping）：
 *    - 将纹理包裹在球面上
 *    - 适合球形物体
 *    - 缺点：在极点处会产生奇点
 * 
 * 3. 柱面映射（Cylindrical Mapping）：
 *    - 将纹理包裹在圆柱面上
 *    - 适合圆柱形物体
 *    - 缺点：在顶部和底部需要特殊处理
 * 
 * 4. 盒式映射（Box Mapping）：
 *    - 从六个方向分别投影纹理
 *    - 适合立方体或类似形状的物体
 *    - 优点：避免严重的拉伸
 */

#include "Dialogs3D.h"
#include "MenuIDs.h"
#include "../engine/GraphicsEngine3D.h"
#include "../algorithms/TextureLoader.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <commdlg.h>

// ============================================================================
// TextureDialog 实现
// ============================================================================

/**
 * @brief 静态成员初始化
 */
Shape3D* TextureDialog::s_currentShape = nullptr;
std::string TextureDialog::s_texturePath = "";

/**
 * @brief 显示纹理设置对话框
 * @param parent 父窗口句柄
 * @param shape 要编辑的3D图形指针，不能为nullptr
 * @return 用户点击确定返回true，取消返回false
 */
bool TextureDialog::Show(HWND parent, Shape3D* shape) {
    if (!shape) {
        return false;
    }
    
    s_currentShape = shape;
    s_texturePath = "";  // 重置纹理路径
    
    // 获取应用程序实例句柄
    // 尝试从父窗口获取，如果失败则使用GetModuleHandle
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE);
    if (!hInstance) {
        hInstance = GetModuleHandle(NULL);
    }
    
    // 显示模态对话框
    INT_PTR result = DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_TEXTURE3D), 
                                parent, DialogProc);
    
    s_currentShape = nullptr;
    
    return (result == IDOK);
}

/**
 * @brief 对话框消息处理回调函数
 * @param hwnd 对话框窗口句柄
 * @param msg Windows消息类型
 * @param wParam 消息参数1
 * @param lParam 消息参数2
 * @return 消息是否被处理
 */
INT_PTR CALLBACK TextureDialog::DialogProc(HWND hwnd, UINT msg, 
                                            WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG: {
            // ================================================================
            // 对话框初始化
            // 显示当前纹理状态并初始化控件
            // ================================================================
            if (s_currentShape) {
                // 显示当前纹理状态
                if (s_currentShape->hasTexture && s_currentShape->textureID != 0) {
                    SetDlgItemTextA(hwnd, IDC_EDIT_TEXTURE_PATH, "(Texture loaded)");
                } else {
                    SetDlgItemTextA(hwnd, IDC_EDIT_TEXTURE_PATH, "(No texture)");
                }
                
                // 初始化纹理映射类型下拉框
                // 不同的映射方式适合不同形状的物体
                HWND hCombo = GetDlgItem(hwnd, IDC_COMBO_MAPPING_TYPE);
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Planar (平面映射)");
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Spherical (球面映射)");
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Cylindrical (柱面映射)");
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Box (盒式映射)");
                SendMessageW(hCombo, CB_SETCURSEL, 0, 0);  // 默认选择平面映射
                
                // 设置默认纹理参数
                // 缩放：控制纹理重复次数，1.0表示不重复
                // 偏移：控制纹理起始位置，0.0表示从原点开始
                SetFloatValue(hwnd, IDC_EDIT_TEX_SCALE_U, 1.0f);
                SetFloatValue(hwnd, IDC_EDIT_TEX_SCALE_V, 1.0f);
                SetFloatValue(hwnd, IDC_EDIT_TEX_OFFSET_U, 0.0f);
                SetFloatValue(hwnd, IDC_EDIT_TEX_OFFSET_V, 0.0f);
            }
            
            // 将对话框居中显示
            RECT rcParent, rcDialog;
            GetWindowRect(GetParent(hwnd), &rcParent);
            GetWindowRect(hwnd, &rcDialog);
            int x = rcParent.left + ((rcParent.right - rcParent.left) - 
                    (rcDialog.right - rcDialog.left)) / 2;
            int y = rcParent.top + ((rcParent.bottom - rcParent.top) - 
                    (rcDialog.bottom - rcDialog.top)) / 2;
            SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            
            return TRUE;
        }

        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case IDC_BTN_BROWSE_TEXTURE: {
                    // ========================================================
                    // 用户点击浏览按钮
                    // 打开文件选择对话框让用户选择纹理文件
                    // ========================================================
                    std::string filepath;
                    if (OpenFileDialog(hwnd, filepath)) {
                        s_texturePath = filepath;
                        SetDlgItemTextA(hwnd, IDC_EDIT_TEXTURE_PATH, filepath.c_str());
                    }
                    return TRUE;
                }
                
                case IDC_BTN_REMOVE_TEXTURE: {
                    // ========================================================
                    // 用户点击移除纹理按钮
                    // 删除当前图形的纹理
                    // ========================================================
                    if (s_currentShape) {
                        // 删除GPU中的纹理资源
                        if (s_currentShape->textureID != 0) {
                            TextureLoader::DeleteTexture(s_currentShape->textureID);
                            s_currentShape->textureID = 0;
                        }
                        s_currentShape->hasTexture = false;
                        s_texturePath = "";
                        SetDlgItemTextA(hwnd, IDC_EDIT_TEXTURE_PATH, "(No texture)");
                        
                        // 调试输出
                        OutputDebugStringA("Texture removed from shape");
                        
                        MessageBoxW(hwnd, L"纹理已移除", L"提示", MB_OK | MB_ICONINFORMATION);
                    }
                    return TRUE;
                }
                
                case IDOK: {
                    // ========================================================
                    // 用户点击确定按钮
                    // 如果选择了新纹理，加载并应用
                    // ========================================================
                    if (!s_currentShape) {
                        EndDialog(hwnd, IDCANCEL);
                        return TRUE;
                    }
                    
                    // 如果选择了新的纹理文件
                    if (!s_texturePath.empty()) {
                        // 检查文件格式是否支持
                        if (!TextureLoader::IsSupportedFormat(s_texturePath)) {
                            MessageBoxW(hwnd, L"不支持的纹理文件格式\n\n支持的格式: BMP, JPG, PNG, TGA, GIF", 
                                       L"错误", MB_OK | MB_ICONERROR);
                            return TRUE;
                        }

                        // 删除旧纹理（如果存在）
                        // 这是为了避免GPU内存泄漏
                        if (s_currentShape->textureID != 0) {
                            TextureLoader::DeleteTexture(s_currentShape->textureID);
                            s_currentShape->textureID = 0;
                        }
                        
                        // 加载新纹理
                        // TextureLoader会将图像数据上传到GPU并返回纹理ID
                        unsigned int newTextureID = TextureLoader::LoadTexture(s_texturePath);
                        if (newTextureID == 0) {
                            // 加载失败，TextureLoader已经显示了错误消息
                            s_currentShape->hasTexture = false;
                            return TRUE;
                        }
                        
                        // 应用新纹理
                        s_currentShape->textureID = newTextureID;
                        s_currentShape->hasTexture = true;
                        
                        // 调试输出
                        char debugMsg[512];
                        sprintf_s(debugMsg, "Texture applied: %s (ID: %u)", 
                                  s_texturePath.c_str(), newTextureID);
                        OutputDebugStringA(debugMsg);
                    }
                    
                    EndDialog(hwnd, IDOK);
                    return TRUE;
                }
                
                case IDCANCEL: {
                    // 用户点击取消按钮
                    EndDialog(hwnd, IDCANCEL);
                    return TRUE;
                }
            }
            break;
        }
        
        case WM_CLOSE: {
            EndDialog(hwnd, IDCANCEL);
            return TRUE;
        }
    }
    
    return FALSE;
}

/**
 * @brief 验证编辑框中的输入是否为有效的浮点数
 * @param hwnd 对话框窗口句柄
 * @param controlID 编辑框控件ID
 * @param outValue 输出参数，存储解析后的浮点数值
 * @return 输入有效返回true，无效返回false
 */
bool TextureDialog::ValidateFloatInput(HWND hwnd, int controlID, float& outValue) {
    char buffer[256];
    GetDlgItemTextA(hwnd, controlID, buffer, sizeof(buffer));
    
    // 检查是否为空
    if (strlen(buffer) == 0) {
        return false;
    }
    
    // 尝试解析浮点数
    char* endPtr;
    float value = strtof(buffer, &endPtr);
    
    // 检查是否完全解析成功
    if (*endPtr != '\0') {
        // 跳过尾部空白
        while (*endPtr == ' ' || *endPtr == '\t') {
            endPtr++;
        }
        if (*endPtr != '\0') {
            return false;
        }
    }
    
    outValue = value;
    return true;
}

/**
 * @brief 设置编辑框的浮点数值
 * @param hwnd 对话框窗口句柄
 * @param controlID 编辑框控件ID
 * @param value 要设置的浮点数值
 */
void TextureDialog::SetFloatValue(HWND hwnd, int controlID, float value) {
    char buffer[64];
    sprintf_s(buffer, "%.2f", value);
    SetDlgItemTextA(hwnd, controlID, buffer);
}

/**
 * @brief 打开文件选择对话框
 * @param hwnd 父窗口句柄
 * @param filepath 输出参数，存储用户选择的文件路径
 * @return 用户选择了文件返回true，取消返回false
 * 
 * 支持的图像格式：
 * - BMP：Windows位图，无压缩，文件较大
 * - JPG/JPEG：有损压缩，适合照片
 * - PNG：无损压缩，支持透明度
 * - TGA：Targa格式，游戏开发常用
 * - GIF：支持动画，但颜色有限
 */
bool TextureDialog::OpenFileDialog(HWND hwnd, std::string& filepath) {
    char szFile[MAX_PATH] = "";
    
    // 初始化OPENFILENAME结构体
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    
    // 设置文件过滤器
    // 格式："显示名称\0过滤模式\0..."，以双空字符结尾
    ofn.lpstrFilter = "Image Files\0*.bmp;*.jpg;*.jpeg;*.png;*.tga;*.gif\0"
                      "BMP Files (*.bmp)\0*.bmp\0"
                      "JPEG Files (*.jpg;*.jpeg)\0*.jpg;*.jpeg\0"
                      "PNG Files (*.png)\0*.png\0"
                      "TGA Files (*.tga)\0*.tga\0"
                      "All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;  // 默认选择第一个过滤器（所有图像文件）
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;  // 使用系统默认目录
    ofn.lpstrTitle = "Select Texture File";
    
    // 设置对话框标志
    // OFN_PATHMUSTEXIST：路径必须存在
    // OFN_FILEMUSTEXIST：文件必须存在
    // OFN_NOCHANGEDIR：不改变当前工作目录
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    
    // 显示文件选择对话框
    if (GetOpenFileNameA(&ofn) == TRUE) {
        filepath = szFile;
        return true;
    }
    
    return false;
}
