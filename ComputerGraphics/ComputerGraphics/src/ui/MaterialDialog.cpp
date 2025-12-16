/**
 * @file MaterialDialog.cpp
 * @brief 3D材质编辑对话框实现
 * @author 计算机图形学项目组
 * 
 * 实现了3D图形材质参数编辑对话框的功能，允许用户调整：
 * - 环境光反射系数（Ambient）：物体对环境光的反射能力
 * - 漫反射系数（Diffuse）：物体对漫反射光的反射能力，决定物体的基本颜色
 * - 镜面反射系数（Specular）：物体对镜面反射光的反射能力，决定高光颜色
 * - 光泽度（Shininess）：控制高光的大小和锐利程度
 * 
 * 材质属性说明：
 * 
 * 1. 环境光反射系数（Ambient Coefficient）：
 *    - RGB三个分量，范围0.0-1.0
 *    - 决定物体在没有直接光照时的颜色
 *    - 通常设置为与漫反射相同或略暗的颜色
 *    - 例如：红色物体的环境光系数可能是 (0.2, 0.0, 0.0)
 * 
 * 2. 漫反射系数（Diffuse Coefficient）：
 *    - RGB三个分量，范围0.0-1.0
 *    - 决定物体的主要颜色
 *    - 这是物体最明显的颜色特征
 *    - 例如：红色物体的漫反射系数可能是 (0.8, 0.0, 0.0)
 * 
 * 3. 镜面反射系数（Specular Coefficient）：
 *    - RGB三个分量，范围0.0-1.0
 *    - 决定高光的颜色
 *    - 金属材质通常使用与漫反射相同的颜色
 *    - 非金属材质通常使用白色或灰色 (0.5, 0.5, 0.5)
 * 
 * 4. 光泽度（Shininess）：
 *    - 范围1-256
 *    - 值越大，高光越小越锐利（更像镜面）
 *    - 值越小，高光越大越模糊（更像粗糙表面）
 *    - 典型值：塑料约32，金属约64-128，镜面约256
 */

#include "Dialogs3D.h"
#include "MenuIDs.h"
#include "../engine/GraphicsEngine3D.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ============================================================================
// MaterialDialog 实现
// ============================================================================

/**
 * @brief 静态成员初始化
 */
Shape3D* MaterialDialog::s_currentShape = nullptr;

/**
 * @brief 显示材质编辑对话框
 * @param parent 父窗口句柄
 * @param shape 要编辑的3D图形指针，不能为nullptr
 * @return 用户点击确定返回true，取消返回false
 */
bool MaterialDialog::Show(HWND parent, Shape3D* shape) {
    if (!shape) {
        return false;
    }
    
    s_currentShape = shape;
    
    // 获取应用程序实例句柄
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE);
    
    // 显示模态对话框
    INT_PTR result = DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_MATERIAL3D), 
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
INT_PTR CALLBACK MaterialDialog::DialogProc(HWND hwnd, UINT msg, 
                                             WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG: {
            // ================================================================
            // 对话框初始化
            // 将当前图形的材质参数填充到各个编辑框中
            // ================================================================
            if (s_currentShape) {
                // 设置环境光反射系数（Ambient）
                // 这些值决定物体在阴影区域的颜色
                SetFloatValue(hwnd, IDC_EDIT_MAT_AMBIENT_R, s_currentShape->ambient[0]);
                SetFloatValue(hwnd, IDC_EDIT_MAT_AMBIENT_G, s_currentShape->ambient[1]);
                SetFloatValue(hwnd, IDC_EDIT_MAT_AMBIENT_B, s_currentShape->ambient[2]);
                
                // 设置漫反射系数（Diffuse）
                // 这些值决定物体的主要颜色
                SetFloatValue(hwnd, IDC_EDIT_MAT_DIFFUSE_R, s_currentShape->diffuse[0]);
                SetFloatValue(hwnd, IDC_EDIT_MAT_DIFFUSE_G, s_currentShape->diffuse[1]);
                SetFloatValue(hwnd, IDC_EDIT_MAT_DIFFUSE_B, s_currentShape->diffuse[2]);
                
                // 设置镜面反射系数（Specular）
                // 这些值决定高光的颜色
                SetFloatValue(hwnd, IDC_EDIT_MAT_SPECULAR_R, s_currentShape->specular[0]);
                SetFloatValue(hwnd, IDC_EDIT_MAT_SPECULAR_G, s_currentShape->specular[1]);
                SetFloatValue(hwnd, IDC_EDIT_MAT_SPECULAR_B, s_currentShape->specular[2]);
                
                // 设置光泽度（Shininess）
                // 值越大高光越小越锐利
                SetFloatValue(hwnd, IDC_EDIT_MAT_SHININESS, s_currentShape->shininess);
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
                case IDOK: {
                    // ========================================================
                    // 用户点击确定按钮
                    // 验证所有输入并应用材质参数
                    // ========================================================
                    if (!s_currentShape) {
                        EndDialog(hwnd, IDCANCEL);
                        return TRUE;
                    }
                    
                    // 临时变量存储新值
                    float ambientR, ambientG, ambientB;   // 环境光反射系数
                    float diffuseR, diffuseG, diffuseB;   // 漫反射系数
                    float specularR, specularG, specularB; // 镜面反射系数
                    float shininess;                       // 光泽度
                    
                    // --------------------------------------------------------
                    // 验证环境光反射系数输入
                    // --------------------------------------------------------
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_MAT_AMBIENT_R, ambientR)) {
                        MessageBoxW(hwnd, L"请输入有效的环境光R值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_AMBIENT_R));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_MAT_AMBIENT_G, ambientG)) {
                        MessageBoxW(hwnd, L"请输入有效的环境光G值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_AMBIENT_G));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_MAT_AMBIENT_B, ambientB)) {
                        MessageBoxW(hwnd, L"请输入有效的环境光B值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_AMBIENT_B));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 验证漫反射系数输入
                    // --------------------------------------------------------
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_MAT_DIFFUSE_R, diffuseR)) {
                        MessageBoxW(hwnd, L"请输入有效的漫反射R值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_DIFFUSE_R));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_MAT_DIFFUSE_G, diffuseG)) {
                        MessageBoxW(hwnd, L"请输入有效的漫反射G值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_DIFFUSE_G));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_MAT_DIFFUSE_B, diffuseB)) {
                        MessageBoxW(hwnd, L"请输入有效的漫反射B值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_DIFFUSE_B));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 验证镜面反射系数输入
                    // --------------------------------------------------------
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_MAT_SPECULAR_R, specularR)) {
                        MessageBoxW(hwnd, L"请输入有效的镜面反射R值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_SPECULAR_R));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_MAT_SPECULAR_G, specularG)) {
                        MessageBoxW(hwnd, L"请输入有效的镜面反射G值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_SPECULAR_G));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_MAT_SPECULAR_B, specularB)) {
                        MessageBoxW(hwnd, L"请输入有效的镜面反射B值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_SPECULAR_B));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 验证光泽度输入
                    // --------------------------------------------------------
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_MAT_SHININESS, shininess)) {
                        MessageBoxW(hwnd, L"请输入有效的光泽度值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_SHININESS));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 验证环境光范围（0.0 - 1.0）
                    // --------------------------------------------------------
                    if (ambientR < 0.0f || ambientR > 1.0f) {
                        MessageBoxW(hwnd, L"环境光R值必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_AMBIENT_R));
                        return TRUE;
                    }
                    if (ambientG < 0.0f || ambientG > 1.0f) {
                        MessageBoxW(hwnd, L"环境光G值必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_AMBIENT_G));
                        return TRUE;
                    }
                    if (ambientB < 0.0f || ambientB > 1.0f) {
                        MessageBoxW(hwnd, L"环境光B值必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_AMBIENT_B));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 验证漫反射范围（0.0 - 1.0）
                    // --------------------------------------------------------
                    if (diffuseR < 0.0f || diffuseR > 1.0f) {
                        MessageBoxW(hwnd, L"漫反射R值必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_DIFFUSE_R));
                        return TRUE;
                    }
                    if (diffuseG < 0.0f || diffuseG > 1.0f) {
                        MessageBoxW(hwnd, L"漫反射G值必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_DIFFUSE_G));
                        return TRUE;
                    }
                    if (diffuseB < 0.0f || diffuseB > 1.0f) {
                        MessageBoxW(hwnd, L"漫反射B值必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_DIFFUSE_B));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 验证镜面反射范围（0.0 - 1.0）
                    // --------------------------------------------------------
                    if (specularR < 0.0f || specularR > 1.0f) {
                        MessageBoxW(hwnd, L"镜面反射R值必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_SPECULAR_R));
                        return TRUE;
                    }
                    if (specularG < 0.0f || specularG > 1.0f) {
                        MessageBoxW(hwnd, L"镜面反射G值必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_SPECULAR_G));
                        return TRUE;
                    }
                    if (specularB < 0.0f || specularB > 1.0f) {
                        MessageBoxW(hwnd, L"镜面反射B值必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_SPECULAR_B));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 验证光泽度范围（1 - 256）
                    // 光泽度决定高光的大小：
                    // - 低值（1-10）：非常模糊的高光，类似粗糙表面
                    // - 中值（32-64）：适中的高光，类似塑料
                    // - 高值（128-256）：锐利的高光，类似金属或镜面
                    // --------------------------------------------------------
                    if (shininess < 1.0f || shininess > 256.0f) {
                        MessageBoxW(hwnd, L"光泽度必须在1到256之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_MAT_SHININESS));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 所有验证通过，应用新的材质参数
                    // --------------------------------------------------------
                    s_currentShape->ambient[0] = ambientR;
                    s_currentShape->ambient[1] = ambientG;
                    s_currentShape->ambient[2] = ambientB;
                    
                    s_currentShape->diffuse[0] = diffuseR;
                    s_currentShape->diffuse[1] = diffuseG;
                    s_currentShape->diffuse[2] = diffuseB;
                    
                    s_currentShape->specular[0] = specularR;
                    s_currentShape->specular[1] = specularG;
                    s_currentShape->specular[2] = specularB;
                    
                    s_currentShape->shininess = shininess;
                    
                    // 调试输出
                    char debugMsg[512];
                    sprintf_s(debugMsg, 
                        "Material applied: Ambient(%.2f, %.2f, %.2f) Diffuse(%.2f, %.2f, %.2f) Specular(%.2f, %.2f, %.2f) Shininess(%.2f)",
                        ambientR, ambientG, ambientB, 
                        diffuseR, diffuseG, diffuseB, 
                        specularR, specularG, specularB, 
                        shininess);
                    OutputDebugStringA(debugMsg);
                    
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
bool MaterialDialog::ValidateFloatInput(HWND hwnd, int controlID, float& outValue) {
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
void MaterialDialog::SetFloatValue(HWND hwnd, int controlID, float value) {
    char buffer[64];
    sprintf_s(buffer, "%.2f", value);
    SetDlgItemTextA(hwnd, controlID, buffer);
}
