/**
 * @file LightingDialog.cpp
 * @brief 3D光照设置对话框实现
 * @author 计算机图形学项目组
 * 
 * 实现了3D场景光照参数设置对话框的功能，允许用户调整：
 * - 光源位置（Light Position）：控制光源在3D空间中的位置
 * - 光照强度（Light Intensity）：控制环境光、漫反射、镜面反射的强度
 * - 光源颜色（Light Color）：控制光源发出的光的颜色
 * 
 * 光照模型说明（Phong光照模型）：
 * 
 * 1. 环境光（Ambient Light）：
 *    - 模拟场景中的间接光照
 *    - 不依赖于光源位置和物体表面法线
 *    - 公式：I_ambient = k_a * I_a
 *    - 其中 k_a 是材质的环境光反射系数，I_a 是环境光强度
 * 
 * 2. 漫反射（Diffuse Reflection）：
 *    - 模拟粗糙表面对光的散射
 *    - 遵循Lambert余弦定律：光照强度与光线和法线夹角的余弦成正比
 *    - 公式：I_diffuse = k_d * I_d * max(0, N·L)
 *    - 其中 N 是表面法线，L 是指向光源的方向向量
 * 
 * 3. 镜面反射（Specular Reflection）：
 *    - 模拟光滑表面的高光效果
 *    - 依赖于观察者位置
 *    - 公式：I_specular = k_s * I_s * max(0, R·V)^n
 *    - 其中 R 是反射光方向，V 是观察方向，n 是光泽度
 * 
 * 最终颜色 = 环境光 + 漫反射 + 镜面反射
 */

#include "Dialogs3D.h"
#include "MenuIDs.h"
#include "../engine/GraphicsEngine3D.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ============================================================================
// LightingDialog 实现
// ============================================================================

/**
 * @brief 静态成员初始化
 * 
 * s_currentLight 用于在静态回调函数中访问当前正在编辑的光源
 */
Light* LightingDialog::s_currentLight = nullptr;

/**
 * @brief 显示光照设置对话框
 * @param parent 父窗口句柄
 * @param light 要编辑的光源指针，不能为nullptr
 * @return 用户点击确定返回true，取消返回false
 */
bool LightingDialog::Show(HWND parent, Light* light) {
    // 参数验证
    if (!light) {
        return false;
    }
    
    // 保存当前光源指针
    s_currentLight = light;
    
    // 获取应用程序实例句柄
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE);
    
    // 显示模态对话框
    INT_PTR result = DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_LIGHTING3D), 
                                parent, DialogProc);
    
    // 清理静态成员
    s_currentLight = nullptr;
    
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
INT_PTR CALLBACK LightingDialog::DialogProc(HWND hwnd, UINT msg, 
                                             WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG: {
            // ================================================================
            // 对话框初始化
            // 将当前光源的参数填充到各个编辑框中
            // ================================================================
            if (s_currentLight) {
                // 设置光源位置
                // 光源位置决定了光线照射的方向
                SetFloatValue(hwnd, IDC_EDIT_LIGHT_POS_X, s_currentLight->positionX);
                SetFloatValue(hwnd, IDC_EDIT_LIGHT_POS_Y, s_currentLight->positionY);
                SetFloatValue(hwnd, IDC_EDIT_LIGHT_POS_Z, s_currentLight->positionZ);
                
                // 设置光照强度
                // 环境光强度：影响整体亮度，通常设置较低值（0.1-0.3）
                // 漫反射强度：主要光照来源，通常设置较高值（0.5-1.0）
                // 镜面反射强度：高光亮度，根据需要调整（0.3-1.0）
                SetFloatValue(hwnd, IDC_EDIT_AMBIENT, s_currentLight->ambientIntensity);
                SetFloatValue(hwnd, IDC_EDIT_DIFFUSE, s_currentLight->diffuseIntensity);
                SetFloatValue(hwnd, IDC_EDIT_SPECULAR, s_currentLight->specularIntensity);
                
                // 设置光源颜色（RGB，范围0.0-1.0）
                // 白光：(1.0, 1.0, 1.0)
                // 暖光：(1.0, 0.9, 0.8)
                // 冷光：(0.8, 0.9, 1.0)
                SetFloatValue(hwnd, IDC_EDIT_LIGHT_R, s_currentLight->color[0]);
                SetFloatValue(hwnd, IDC_EDIT_LIGHT_G, s_currentLight->color[1]);
                SetFloatValue(hwnd, IDC_EDIT_LIGHT_B, s_currentLight->color[2]);
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
                    // 验证所有输入并应用光照参数
                    // ========================================================
                    if (!s_currentLight) {
                        EndDialog(hwnd, IDCANCEL);
                        return TRUE;
                    }
                    
                    // 临时变量存储新值
                    float posX, posY, posZ;           // 光源位置
                    float ambient, diffuse, specular; // 光照强度
                    float colorR, colorG, colorB;     // 光源颜色
                    
                    // --------------------------------------------------------
                    // 验证光源位置输入
                    // 位置可以是任意浮点数
                    // --------------------------------------------------------
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_LIGHT_POS_X, posX)) {
                        MessageBoxW(hwnd, L"请输入有效的光源位置X值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_LIGHT_POS_X));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_LIGHT_POS_Y, posY)) {
                        MessageBoxW(hwnd, L"请输入有效的光源位置Y值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_LIGHT_POS_Y));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_LIGHT_POS_Z, posZ)) {
                        MessageBoxW(hwnd, L"请输入有效的光源位置Z值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_LIGHT_POS_Z));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 验证光照强度输入
                    // 强度值必须在0.0到1.0之间
                    // --------------------------------------------------------
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_AMBIENT, ambient)) {
                        MessageBoxW(hwnd, L"请输入有效的环境光强度值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_AMBIENT));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_DIFFUSE, diffuse)) {
                        MessageBoxW(hwnd, L"请输入有效的漫反射强度值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_DIFFUSE));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_SPECULAR, specular)) {
                        MessageBoxW(hwnd, L"请输入有效的镜面反射强度值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_SPECULAR));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 验证光源颜色输入
                    // 颜色值必须在0.0到1.0之间
                    // --------------------------------------------------------
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_LIGHT_R, colorR)) {
                        MessageBoxW(hwnd, L"请输入有效的光源颜色R值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_LIGHT_R));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_LIGHT_G, colorG)) {
                        MessageBoxW(hwnd, L"请输入有效的光源颜色G值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_LIGHT_G));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_LIGHT_B, colorB)) {
                        MessageBoxW(hwnd, L"请输入有效的光源颜色B值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_LIGHT_B));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 验证光照强度范围（0.0 - 1.0）
                    // --------------------------------------------------------
                    if (ambient < 0.0f || ambient > 1.0f) {
                        MessageBoxW(hwnd, L"环境光强度必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_AMBIENT));
                        return TRUE;
                    }
                    if (diffuse < 0.0f || diffuse > 1.0f) {
                        MessageBoxW(hwnd, L"漫反射强度必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_DIFFUSE));
                        return TRUE;
                    }
                    if (specular < 0.0f || specular > 1.0f) {
                        MessageBoxW(hwnd, L"镜面反射强度必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_SPECULAR));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 验证颜色范围（0.0 - 1.0）
                    // --------------------------------------------------------
                    if (colorR < 0.0f || colorR > 1.0f) {
                        MessageBoxW(hwnd, L"颜色R值必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_LIGHT_R));
                        return TRUE;
                    }
                    if (colorG < 0.0f || colorG > 1.0f) {
                        MessageBoxW(hwnd, L"颜色G值必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_LIGHT_G));
                        return TRUE;
                    }
                    if (colorB < 0.0f || colorB > 1.0f) {
                        MessageBoxW(hwnd, L"颜色B值必须在0.0到1.0之间", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_LIGHT_B));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 所有验证通过，应用新的光照参数
                    // --------------------------------------------------------
                    s_currentLight->positionX = posX;
                    s_currentLight->positionY = posY;
                    s_currentLight->positionZ = posZ;
                    
                    s_currentLight->ambientIntensity = ambient;
                    s_currentLight->diffuseIntensity = diffuse;
                    s_currentLight->specularIntensity = specular;
                    
                    s_currentLight->color[0] = colorR;
                    s_currentLight->color[1] = colorG;
                    s_currentLight->color[2] = colorB;
                    
                    // 调试输出
                    char debugMsg[512];
                    sprintf_s(debugMsg, 
                        "Lighting applied: Pos(%.2f, %.2f, %.2f) Intensity(A:%.2f, D:%.2f, S:%.2f) Color(%.2f, %.2f, %.2f)",
                        posX, posY, posZ, ambient, diffuse, specular, colorR, colorG, colorB);
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
bool LightingDialog::ValidateFloatInput(HWND hwnd, int controlID, float& outValue) {
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
void LightingDialog::SetFloatValue(HWND hwnd, int controlID, float value) {
    char buffer[64];
    sprintf_s(buffer, "%.2f", value);
    SetDlgItemTextA(hwnd, controlID, buffer);
}
