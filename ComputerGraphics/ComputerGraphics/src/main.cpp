/**
 * @file main.cpp
 * @brief 计算机图形学实验程序主入口
 * @author 计算机图形学项目组
 * 
 * 这是整个计算机图形学实验系统的主程序文件，负责：
 * - 创建和管理主窗口
 * - 处理用户界面事件
 * - 协调2D和3D图形引擎的工作
 * - 管理菜单系统和用户交互
 */

#include "../framework.h"
#include "engine/GraphicsEngine.h"
#include "engine/GraphicsEngine3D.h"
#include "ui/MenuIDs.h"

// === 全局变量 ===
GraphicsEngine g_engine;        ///< 2D图形引擎实例
GraphicsEngine3D g_engine3D;    ///< 3D图形引擎实例
bool is3DMode = false;          ///< 当前是否为3D模式标志

/**
 * @brief 窗口过程函数声明
 * @param hwnd 窗口句柄
 * @param msg 消息类型
 * @param wParam 消息参数1
 * @param lParam 消息参数2
 * @return 消息处理结果
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * @brief 程序主入口函数
 * @param hInstance 应用程序实例句柄
 * @param hPrevInstance 前一个实例句柄（Win32中总是NULL）
 * @param lpCmdLine 命令行参数
 * @param nCmdShow 窗口显示方式
 * @return 程序退出代码
 * 
 * 负责初始化窗口类、创建主窗口并启动消息循环
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"GraphicsApp";

    // 注册窗口类
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;                           // 窗口过程函数
    wc.hInstance = hInstance;                           // 应用程序实例
    wc.lpszClassName = CLASS_NAME;                      // 窗口类名
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);      // 背景画刷
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);        // 鼠标光标
    RegisterClass(&wc);

    // 创建主窗口
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"计算机图形学实验系统",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    // 消息循环
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // 创建菜单栏
            HMENU hMenuBar = CreateMenu();
            
            // === 文件菜单 ===
            HMENU hFileMenu = CreatePopupMenu();
            AppendMenuW(hFileMenu, MF_STRING, ID_FILE_NEW, L"新建(&N)");
            AppendMenuW(hFileMenu, MF_STRING, ID_FILE_EXIT, L"退出(&X)");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"文件(&F)");
            
            // === 模式切换菜单 ===
            HMENU hModeMenu = CreatePopupMenu();
            AppendMenuW(hModeMenu, MF_STRING, ID_MODE_2D, L"2D模式(&2)");
            AppendMenuW(hModeMenu, MF_STRING, ID_MODE_3D, L"3D模式(&3)");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hModeMenu, L"模式(&M)");
            
            // === 实验菜单 ===
            HMENU hExprMenu = CreatePopupMenu();
            AppendMenuW(hExprMenu, MF_STRING, ID_EXPR_EXPR1, L"实验一(&1)");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hExprMenu, L"实验(&E)");
            
            // === 2D绘图菜单 ===
            HMENU hDrawMenu = CreatePopupMenu();
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_LINE_DDA, L"直线 (DDA算法)(&D)");
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_LINE_BRES, L"直线 (Bresenham算法)(&B)");
            AppendMenuW(hDrawMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_MID, L"圆形 (中点算法)(&M)");
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_BRES, L"圆形 (Bresenham算法)(&C)");
            AppendMenuW(hDrawMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_RECTANGLE, L"矩形(&R)");
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_POLYLINE, L"折线 (右键结束)(&P)");
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_POLYGON, L"多边形 (右键结束)(&G)");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hDrawMenu, L"绘图(&D)");
            
            // === 填充菜单 ===
            HMENU hFillMenu = CreatePopupMenu();
            AppendMenuW(hFillMenu, MF_STRING, ID_FILL_BOUNDARY, L"边界填充(&B)");
            AppendMenuW(hFillMenu, MF_STRING, ID_FILL_SCANLINE, L"扫描线填充(&S)");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hFillMenu, L"填充(&F)");
            
            // === 几何变换菜单 ===
            HMENU hTransformMenu = CreatePopupMenu();
            AppendMenuW(hTransformMenu, MF_STRING, ID_TRANSFORM_SELECT, L"选择图形(&S)");
            AppendMenuW(hTransformMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hTransformMenu, MF_STRING, ID_TRANSFORM_TRANSLATE, L"平移(&T)");
            AppendMenuW(hTransformMenu, MF_STRING, ID_TRANSFORM_SCALE, L"缩放(&C)");
            AppendMenuW(hTransformMenu, MF_STRING, ID_TRANSFORM_ROTATE, L"旋转(&R)");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hTransformMenu, L"变换(&T)");
            
            // === 裁剪菜单 ===
            HMENU hClipMenu = CreatePopupMenu();
            
            // 直线裁剪子菜单
            HMENU hLineClipMenu = CreatePopupMenu();
            AppendMenuW(hLineClipMenu, MF_STRING, ID_CLIP_COHEN_SUTHERLAND, L"Cohen-Sutherland算法(&C)");
            AppendMenuW(hLineClipMenu, MF_STRING, ID_CLIP_MIDPOINT, L"中点分割算法(&M)");
            AppendMenuW(hClipMenu, MF_POPUP, (UINT_PTR)hLineClipMenu, L"直线裁剪(&L)");
            
            // 多边形裁剪子菜单
            HMENU hPolyClipMenu = CreatePopupMenu();
            AppendMenuW(hPolyClipMenu, MF_STRING, ID_CLIP_SUTHERLAND_HODGMAN, L"Sutherland-Hodgman算法(&S)");
            AppendMenuW(hPolyClipMenu, MF_STRING, ID_CLIP_WEILER_ATHERTON, L"Weiler-Atherton算法(&W)");
            AppendMenuW(hClipMenu, MF_POPUP, (UINT_PTR)hPolyClipMenu, L"多边形裁剪(&P)");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hClipMenu, L"裁剪(&C)");
            
            // === 3D图形菜单 ===
            HMENU h3DMenu = CreatePopupMenu();
            AppendMenuW(h3DMenu, MF_STRING, ID_3D_SPHERE, L"球体(&S)");
            AppendMenuW(h3DMenu, MF_STRING, ID_3D_CYLINDER, L"圆柱体(&C)");
            AppendMenuW(h3DMenu, MF_STRING, ID_3D_PLANE, L"平面(&P)");
            AppendMenuW(h3DMenu, MF_STRING, ID_3D_CUBE, L"立方体(&U)");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)h3DMenu, L"3D图形(&3)");
            
            // === 3D控制菜单 ===
            HMENU h3DControlMenu = CreatePopupMenu();
            AppendMenuW(h3DControlMenu, MF_STRING, ID_3D_SELECT, L"选择(&S)");
            AppendMenuW(h3DControlMenu, MF_STRING, ID_3D_VIEW_CONTROL, L"视角控制(&V)");
            AppendMenuW(h3DControlMenu, MF_STRING, ID_3D_LIGHTING, L"光照设置(&L)");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)h3DControlMenu, L"3D控制(&O)");
            
            SetMenu(hwnd, hMenuBar);
            
            // 初始化3D引擎
            g_engine3D.Initialize(hwnd);
            
            return 0;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            if (is3DMode) {
                // 3D模式渲染
                g_engine3D.Render();
            } else {
                // 2D模式渲染
                g_engine.Initialize(hwnd, hdc);
                if (g_engine.GetMode() == MODE_EXPR1) {
                    g_engine.DrawExpr1Graphics();  // 绘制实验图形
                } else {
                    g_engine.RenderAll();           // 渲染所有图形
                }
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            // 处理鼠标左键按下事件
            if (is3DMode) {
                // 3D模式下的鼠标处理
                g_engine3D.OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
            } else {
                // 2D模式下的鼠标处理
                HDC hdc = GetDC(hwnd);
                g_engine.Initialize(hwnd, hdc);
                g_engine.OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
                ReleaseDC(hwnd, hdc);
            }
            InvalidateRect(hwnd, NULL, FALSE);  // 重绘窗口
            return 0;
        }
        
        case WM_RBUTTONDOWN: {
            // 处理鼠标右键按下事件（仅2D模式）
            HDC hdc = GetDC(hwnd);
            g_engine.Initialize(hwnd, hdc);
            g_engine.OnRButtonDown(LOWORD(lParam), HIWORD(lParam));
            ReleaseDC(hwnd, hdc);
            InvalidateRect(hwnd, NULL, FALSE);  // 重绘窗口
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            // 处理鼠标移动事件（仅2D模式）
            HDC hdc = GetDC(hwnd);
            g_engine.Initialize(hwnd, hdc);
            g_engine.OnMouseMove(LOWORD(lParam), HIWORD(lParam));
            ReleaseDC(hwnd, hdc);
            return 0;
        }
        
        case WM_COMMAND: {
            // 处理菜单命令消息
            switch (LOWORD(wParam)) {
                // === 文件菜单命令 ===
                case ID_FILE_NEW: {
                    // 新建 - 清空画布
                    HDC hdc = GetDC(hwnd);
                    g_engine.Initialize(hwnd, hdc);
                    g_engine.ClearCanvas();
                    ReleaseDC(hwnd, hdc);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                }
                case ID_FILE_EXIT:
                    // 退出程序
                    DestroyWindow(hwnd);
                    break;
                    
                // === 实验菜单命令 ===
                case ID_EXPR_EXPR1:
                    // 实验一
                    g_engine.SetMode(MODE_EXPR1);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                // === 2D绘图菜单命令 ===
                case ID_DRAW_LINE_DDA:
                    // DDA直线绘制算法
                    g_engine.SetMode(MODE_LINE_DDA);
                    break;
                case ID_DRAW_LINE_BRES:
                    // Bresenham直线绘制算法
                    g_engine.SetMode(MODE_LINE_BRESENHAM);
                    break;
                case ID_DRAW_CIRCLE_MID:
                    // 中点圆绘制算法
                    g_engine.SetMode(MODE_CIRCLE_MIDPOINT);
                    break;
                case ID_DRAW_CIRCLE_BRES:
                    // Bresenham圆绘制算法
                    g_engine.SetMode(MODE_CIRCLE_BRESENHAM);
                    break;
                case ID_DRAW_RECTANGLE:
                    // 矩形绘制
                    g_engine.SetMode(MODE_RECTANGLE);
                    break;
                case ID_DRAW_POLYLINE:
                    // 折线绘制
                    g_engine.SetMode(MODE_POLYLINE);
                    break;
                case ID_DRAW_POLYGON:
                    // 多边形绘制
                    g_engine.SetMode(MODE_POLYGON);
                    break;
                // === 填充算法菜单命令 ===
                case ID_FILL_BOUNDARY:
                    // 边界填充算法
                    g_engine.SetMode(MODE_FILL_BOUNDARY);
                    break;
                case ID_FILL_SCANLINE:
                    // 扫描线填充算法
                    g_engine.SetMode(MODE_FILL_SCANLINE);
                    break;
                    
                // === 几何变换菜单命令 ===
                case ID_TRANSFORM_SELECT:
                    // 图形选择模式
                    g_engine.SetMode(MODE_SELECT);
                    break;
                case ID_TRANSFORM_TRANSLATE:
                    // 平移变换
                    g_engine.SetMode(MODE_TRANSLATE);
                    break;
                case ID_TRANSFORM_SCALE:
                    // 缩放变换
                    g_engine.SetMode(MODE_SCALE);
                    break;
                case ID_TRANSFORM_ROTATE:
                    // 旋转变换
                    g_engine.SetMode(MODE_ROTATE);
                    break;
                // === 裁剪算法菜单命令 ===
                case ID_CLIP_COHEN_SUTHERLAND:
                    // Cohen-Sutherland直线裁剪算法
                    g_engine.SetMode(MODE_CLIP_COHEN_SUTHERLAND);
                    break;
                case ID_CLIP_MIDPOINT:
                    // 中点分割直线裁剪算法
                    g_engine.SetMode(MODE_CLIP_MIDPOINT);
                    break;
                case ID_CLIP_SUTHERLAND_HODGMAN:
                    // Sutherland-Hodgman多边形裁剪算法
                    g_engine.SetMode(MODE_CLIP_SUTHERLAND_HODGMAN);
                    break;
                case ID_CLIP_WEILER_ATHERTON:
                    // Weiler-Atherton多边形裁剪算法
                    g_engine.SetMode(MODE_CLIP_WEILER_ATHERTON);
                    break;
                
                // === 模式切换菜单命令 ===
                case ID_MODE_2D:
                    // 切换到2D模式
                    is3DMode = false;
                    // 释放OpenGL上下文，确保2D渲染正常
                    g_engine3D.ReleaseContext();
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case ID_MODE_3D:
                    // 切换到3D模式
                    is3DMode = true;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                
                // === 3D图形绘制菜单命令 ===
                case ID_3D_SPHERE:
                    // 3D球体绘制
                    g_engine3D.SetMode(MODE_3D_SPHERE);
                    break;
                case ID_3D_CYLINDER:
                    // 3D圆柱体绘制
                    g_engine3D.SetMode(MODE_3D_CYLINDER);
                    break;
                case ID_3D_PLANE:
                    // 3D平面绘制
                    g_engine3D.SetMode(MODE_3D_PLANE);
                    break;
                case ID_3D_CUBE:
                    // 3D立方体绘制
                    g_engine3D.SetMode(MODE_3D_CUBE);
                    break;
                
                // === 3D控制菜单命令 ===
                case ID_3D_SELECT:
                    // 3D图形选择模式
                    g_engine3D.SetMode(MODE_3D_SELECT);
                    break;
                case ID_3D_VIEW_CONTROL:
                    // 3D视角控制模式
                    g_engine3D.SetMode(MODE_3D_VIEW_CONTROL);
                    break;
                case ID_3D_LIGHTING:
                    // 3D光照设置（待实现）
                    break;
            }
            return 0;
        }
        
        case WM_DESTROY:
            // 窗口销毁时清理资源
            g_engine3D.Shutdown();  // 关闭3D引擎
            PostQuitMessage(0);     // 发送退出消息
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

/**
 * @brief 窗口过程函数实现
 * @param hwnd 窗口句柄
 * @param msg 消息类型
 * @param wParam 消息参数1
 * @param lParam 消息参数2
 * @return 消息处理结果
 * 
 * 处理所有发送到主窗口的Windows消息，包括：
 * - WM_CREATE: 窗口创建时初始化菜单和引擎
 * - WM_PAINT: 窗口重绘时渲染图形
 * - WM_LBUTTONDOWN/WM_RBUTTONDOWN: 鼠标按键事件
 * - WM_MOUSEMOVE: 鼠标移动事件
 * - WM_COMMAND: 菜单命令处理
 * - WM_DESTROY: 窗口销毁时清理资源
 */
