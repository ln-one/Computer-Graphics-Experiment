#include "framework.h"
#include "GraphicsEngine.h"
#include "resource.h"

// 全局变量
GraphicsEngine g_engine;
HINSTANCE g_hInst;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        {
            // 创建菜单
            HMENU hMenuBar = CreateMenu();
            
            // 文件菜单
            HMENU hFileMenu = CreatePopupMenu();
            AppendMenu(hFileMenu, MF_STRING, ID_FILE_NEW, L"新建(&N)\tCtrl+N");
            AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, L"退出(&X)");
            AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"文件(&F)");
            
            // 实验菜单
            HMENU hExprMenu = CreatePopupMenu();
            AppendMenu(hExprMenu, MF_STRING, ID_EXPR_EXPR1, L"实验一：基本图形绘制");
            AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hExprMenu, L"实验(&E)");
            
            // 绘图菜单
            HMENU hDrawMenu = CreatePopupMenu();
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_LINE_DDA, L"直线(DDA算法)");
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_LINE_BRES, L"直线(Bresenham算法)");
            AppendMenu(hDrawMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_MID, L"圆(中点算法)");
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_BRES, L"圆(Bresenham算法)");
            AppendMenu(hDrawMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_RECTANGLE, L"矩形");
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_POLYLINE, L"多段线(右键结束)");
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_BSPLINE, L"B样条曲线(4个控制点)");
            AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hDrawMenu, L"绘图(&D)");
            
            // 填充菜单
            HMENU hFillMenu = CreatePopupMenu();
            AppendMenu(hFillMenu, MF_STRING, ID_FILL_SCANLINE, L"扫描线填充");
            AppendMenu(hFillMenu, MF_STRING, ID_FILL_BOUNDARY, L"边界填充");
            AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFillMenu, L"填充(&I)");
            
            // 帮助菜单
            HMENU hHelpMenu = CreatePopupMenu();
            AppendMenu(hHelpMenu, MF_STRING, ID_HELP_ABOUT, L"关于(&A)");
            AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hHelpMenu, L"帮助(&H)");
            
            SetMenu(hwnd, hMenuBar);
        }
        return 0;
        
    case WM_PAINT: 
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            g_engine.Initialize(hwnd, hdc);
            
            // 根据当前模式绘制
            if (g_engine.GetMode() == MODE_EXPR1) {
                g_engine.DrawExpr1Graphics();
            }
            
            EndPaint(hwnd, &ps);
        }
        return 0;
        
    case WM_LBUTTONDOWN:
        {
            HDC hdc = GetDC(hwnd);
            g_engine.Initialize(hwnd, hdc);
            g_engine.OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
            ReleaseDC(hwnd, hdc);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
        
    case WM_RBUTTONDOWN:
        {
            HDC hdc = GetDC(hwnd);
            g_engine.Initialize(hwnd, hdc);
            g_engine.OnRButtonDown(LOWORD(lParam), HIWORD(lParam));
            ReleaseDC(hwnd, hdc);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
        
    case WM_MOUSEMOVE:
        {
            HDC hdc = GetDC(hwnd);
            g_engine.Initialize(hwnd, hdc);
            g_engine.OnMouseMove(LOWORD(lParam), HIWORD(lParam));
            ReleaseDC(hwnd, hdc);
        }
        return 0;
        
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId) {
            case ID_FILE_NEW:
                {
                    HDC hdc = GetDC(hwnd);
                    g_engine.Initialize(hwnd, hdc);
                    g_engine.ClearCanvas();
                    ReleaseDC(hwnd, hdc);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                break;
                
            case ID_FILE_EXIT:
                DestroyWindow(hwnd);
                break;
                
            case ID_EXPR_EXPR1:
                g_engine.SetMode(MODE_EXPR1);
                InvalidateRect(hwnd, NULL, TRUE);
                break;
                
            case ID_DRAW_LINE_DDA:
                g_engine.SetMode(MODE_LINE_DDA);
                break;
                
            case ID_DRAW_LINE_BRES:
                g_engine.SetMode(MODE_LINE_BRESENHAM);
                break;
                
            case ID_DRAW_CIRCLE_MID:
                g_engine.SetMode(MODE_CIRCLE_MIDPOINT);
                break;
                
            case ID_DRAW_CIRCLE_BRES:
                g_engine.SetMode(MODE_CIRCLE_BRESENHAM);
                break;
                
            case ID_DRAW_RECTANGLE:
                g_engine.SetMode(MODE_RECTANGLE);
                break;
                
            case ID_DRAW_POLYLINE:
                g_engine.SetMode(MODE_POLYLINE);
                break;
                
            case ID_DRAW_BSPLINE:
                g_engine.SetMode(MODE_BSPLINE);
                break;
                
            case ID_FILL_SCANLINE:
                g_engine.SetMode(MODE_FILL_SCANLINE);
                break;
                
            case ID_FILL_BOUNDARY:
                g_engine.SetMode(MODE_FILL_BOUNDARY);
                break;
                
            case ID_HELP_ABOUT:
                MessageBox(hwnd, 
                    L"计算机图形学综合实验\n"
                    L"学号：2023112573\n"
                    L"姓名：张春冉\n\n"
                    L"实验一：基本图形绘制\n"
                    L"实验二：基本图形生成算法\n"
                    L"- DDA和Bresenham直线算法\n"
                    L"- 中点和Bresenham圆算法\n"
                    L"- 矩形、多段线、B样条曲线\n"
                    L"- 扫描线填充和边界填充",
                    L"关于", MB_OK | MB_ICONINFORMATION);
                break;
                
            default:
                return DefWindowProc(hwnd, msg, wParam, lParam);
            }
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"SWJTU_ZCR_2023112573_computer_graphics_expr_1";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"2023112573-张春冉-计算机图形学实验-1",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}