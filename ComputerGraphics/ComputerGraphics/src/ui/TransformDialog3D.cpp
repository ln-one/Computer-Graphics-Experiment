/**
 * @file TransformDialog3D.cpp
 * @brief 3D变换参数编辑对话框实现
 * @author 计算机图形学项目组
 * 
 * 实现了3D图形变换参数编辑对话框的功能，允许用户精确设置：
 * - 位置参数（Position）：控制物体在3D空间中的位置（X, Y, Z坐标）
 * - 旋转参数（Rotation）：控制物体绕各轴的旋转角度（以度为单位）
 * - 缩放参数（Scale）：控制物体在各轴方向的缩放比例
 * 
 * 变换原理说明：
 * 在3D图形学中，物体的变换通常按照以下顺序应用：
 * 1. 缩放（Scale）- 改变物体大小
 * 2. 旋转（Rotation）- 改变物体朝向
 * 3. 平移（Translation）- 改变物体位置
 * 这个顺序很重要，因为矩阵乘法不满足交换律
 */

#include "Dialogs3D.h"
#include "MenuIDs.h"
#include "../engine/GraphicsEngine3D.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ============================================================================
// TransformDialog3D 实现
// ============================================================================

/**
 * @brief 静态成员初始化
 * 
 * s_currentShape 用于在静态回调函数中访问当前正在编辑的图形
 * 由于Windows对话框回调函数必须是静态的，我们使用静态成员来传递数据
 */
Shape3D* TransformDialog3D::s_currentShape = nullptr;

/**
 * @brief 显示变换参数对话框
 * @param parent 父窗口句柄，对话框将相对于此窗口居中显示
 * @param shape 要编辑的3D图形指针，不能为nullptr
 * @return 用户点击确定返回true，取消或关闭返回false
 * 
 * 工作流程：
 * 1. 验证输入参数
 * 2. 保存当前图形指针到静态成员
 * 3. 创建并显示模态对话框
 * 4. 等待用户操作完成
 * 5. 清理静态成员并返回结果
 */
bool TransformDialog3D::Show(HWND parent, Shape3D* shape) {
    // 参数验证：确保传入的图形指针有效
    if (!shape) {
        return false;
    }
    
    // 保存当前图形指针，供对话框回调函数使用
    s_currentShape = shape;
    
    // 获取应用程序实例句柄，用于加载对话框资源
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE);

    // 显示模态对话框
    // DialogBoxW 会阻塞直到对话框关闭
    INT_PTR result = DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_TRANSFORM3D), 
                                parent, DialogProc);
    
    // 清理静态成员
    s_currentShape = nullptr;
    
    // 返回用户是否点击了确定按钮
    return (result == IDOK);
}

/**
 * @brief 对话框消息处理回调函数
 * @param hwnd 对话框窗口句柄
 * @param msg Windows消息类型
 * @param wParam 消息参数1（通常包含控件ID或通知码）
 * @param lParam 消息参数2（通常包含附加数据）
 * @return 消息是否被处理
 * 
 * 处理的主要消息：
 * - WM_INITDIALOG：初始化对话框，填充当前参数值
 * - WM_COMMAND：处理按钮点击和控件通知
 * - WM_CLOSE：处理对话框关闭
 */
INT_PTR CALLBACK TransformDialog3D::DialogProc(HWND hwnd, UINT msg, 
                                                WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG: {
            // ================================================================
            // 对话框初始化
            // 将当前图形的变换参数填充到各个编辑框中
            // ================================================================
            if (s_currentShape) {
                // 设置位置值（Position）
                // 位置表示物体在世界坐标系中的位置
                SetFloatValue(hwnd, IDC_EDIT_POS_X, s_currentShape->positionX);
                SetFloatValue(hwnd, IDC_EDIT_POS_Y, s_currentShape->positionY);
                SetFloatValue(hwnd, IDC_EDIT_POS_Z, s_currentShape->positionZ);
                
                // 设置旋转值（Rotation）
                // 旋转角度以度为单位，分别表示绕X、Y、Z轴的旋转
                SetFloatValue(hwnd, IDC_EDIT_ROT_X, s_currentShape->rotationX);
                SetFloatValue(hwnd, IDC_EDIT_ROT_Y, s_currentShape->rotationY);
                SetFloatValue(hwnd, IDC_EDIT_ROT_Z, s_currentShape->rotationZ);

                // 设置缩放值（Scale）
                // 缩放因子，1.0表示原始大小，>1放大，<1缩小
                SetFloatValue(hwnd, IDC_EDIT_SCALE_X, s_currentShape->scaleX);
                SetFloatValue(hwnd, IDC_EDIT_SCALE_Y, s_currentShape->scaleY);
                SetFloatValue(hwnd, IDC_EDIT_SCALE_Z, s_currentShape->scaleZ);
            }
            
            // 将对话框居中显示在父窗口中
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
                    // 验证所有输入并应用变换参数
                    // ========================================================
                    if (!s_currentShape) {
                        EndDialog(hwnd, IDCANCEL);
                        return TRUE;
                    }
                    
                    // 临时变量存储新值，验证通过后再应用
                    float posX, posY, posZ;      // 位置参数
                    float rotX, rotY, rotZ;      // 旋转参数
                    float scaleX, scaleY, scaleZ; // 缩放参数
                    
                    // --------------------------------------------------------
                    // 验证位置输入
                    // 位置值可以是任意浮点数，没有范围限制
                    // --------------------------------------------------------
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_POS_X, posX)) {
                        MessageBoxW(hwnd, L"请输入有效的位置X值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_POS_X));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_POS_Y, posY)) {
                        MessageBoxW(hwnd, L"请输入有效的位置Y值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_POS_Y));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_POS_Z, posZ)) {
                        MessageBoxW(hwnd, L"请输入有效的位置Z值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_POS_Z));
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 验证旋转输入
                    // 旋转值以度为单位，可以是任意浮点数
                    // --------------------------------------------------------
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_ROT_X, rotX)) {
                        MessageBoxW(hwnd, L"请输入有效的旋转X值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_ROT_X));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_ROT_Y, rotY)) {
                        MessageBoxW(hwnd, L"请输入有效的旋转Y值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_ROT_Y));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_ROT_Z, rotZ)) {
                        MessageBoxW(hwnd, L"请输入有效的旋转Z值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_ROT_Z));
                        return TRUE;
                    }

                    // --------------------------------------------------------
                    // 验证缩放输入
                    // 缩放值必须是非零浮点数，零值会导致物体消失
                    // --------------------------------------------------------
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_SCALE_X, scaleX)) {
                        MessageBoxW(hwnd, L"请输入有效的缩放X值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_SCALE_X));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_SCALE_Y, scaleY)) {
                        MessageBoxW(hwnd, L"请输入有效的缩放Y值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_SCALE_Y));
                        return TRUE;
                    }
                    if (!ValidateFloatInput(hwnd, IDC_EDIT_SCALE_Z, scaleZ)) {
                        MessageBoxW(hwnd, L"请输入有效的缩放Z值", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hwnd, IDC_EDIT_SCALE_Z));
                        return TRUE;
                    }
                    
                    // 验证缩放值不能为0（会导致物体不可见）
                    if (scaleX == 0.0f || scaleY == 0.0f || scaleZ == 0.0f) {
                        MessageBoxW(hwnd, L"缩放值不能为0", L"输入错误", 
                                   MB_OK | MB_ICONWARNING);
                        return TRUE;
                    }
                    
                    // --------------------------------------------------------
                    // 所有验证通过，应用新的变换参数
                    // --------------------------------------------------------
                    s_currentShape->positionX = posX;
                    s_currentShape->positionY = posY;
                    s_currentShape->positionZ = posZ;
                    
                    s_currentShape->rotationX = rotX;
                    s_currentShape->rotationY = rotY;
                    s_currentShape->rotationZ = rotZ;
                    
                    s_currentShape->scaleX = scaleX;
                    s_currentShape->scaleY = scaleY;
                    s_currentShape->scaleZ = scaleZ;
                    
                    // 调试输出，便于跟踪变换参数的变化
                    char debugMsg[512];
                    sprintf_s(debugMsg, 
                        "Transform applied: Pos(%.2f, %.2f, %.2f) Rot(%.2f, %.2f, %.2f) Scale(%.2f, %.2f, %.2f)",
                        posX, posY, posZ, rotX, rotY, rotZ, scaleX, scaleY, scaleZ);
                    OutputDebugStringA(debugMsg);
                    
                    EndDialog(hwnd, IDOK);
                    return TRUE;
                }
                
                case IDCANCEL: {
                    // 用户点击取消按钮，不做任何修改
                    EndDialog(hwnd, IDCANCEL);
                    return TRUE;
                }
                
                case IDC_BTN_MATERIAL: {
                    // 用户点击材质按钮，打开材质编辑对话框
                    if (s_currentShape) {
                        MaterialDialog::Show(hwnd, s_currentShape);
                    }
                    return TRUE;
                }
                
                case IDC_BTN_TEXTURE: {
                    // 用户点击纹理按钮，打开纹理设置对话框
                    if (s_currentShape) {
                        TextureDialog::Show(hwnd, s_currentShape);
                    }
                    return TRUE;
                }
            }
            break;
        }
        
        case WM_CLOSE: {
            // 用户点击关闭按钮，等同于取消
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
 * 
 * 验证规则：
 * 1. 输入不能为空
 * 2. 输入必须能够解析为浮点数
 * 3. 允许尾部有空白字符
 */
bool TransformDialog3D::ValidateFloatInput(HWND hwnd, int controlID, float& outValue) {
    char buffer[256];
    GetDlgItemTextA(hwnd, controlID, buffer, sizeof(buffer));
    
    // 检查是否为空
    if (strlen(buffer) == 0) {
        return false;
    }
    
    // 尝试解析浮点数
    // strtof 会将字符串转换为浮点数，并设置 endPtr 指向第一个无法解析的字符
    char* endPtr;
    float value = strtof(buffer, &endPtr);
    
    // 检查是否完全解析成功
    if (*endPtr != '\0') {
        // 跳过尾部空白字符
        while (*endPtr == ' ' || *endPtr == '\t') {
            endPtr++;
        }
        // 如果还有其他字符，说明输入无效
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
 * 
 * 将浮点数格式化为保留两位小数的字符串，然后设置到编辑框中
 */
void TransformDialog3D::SetFloatValue(HWND hwnd, int controlID, float value) {
    char buffer[64];
    sprintf_s(buffer, "%.2f", value);
    SetDlgItemTextA(hwnd, controlID, buffer);
}
