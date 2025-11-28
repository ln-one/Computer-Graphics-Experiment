#include "../framework.h"
#include "engine/GraphicsEngine.h"
#include "ui/MenuIDs.h"

GraphicsEngine g_engine;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"GraphicsApp";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"计算机图形学实验",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

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
            // 创建菜单
            HMENU hMenuBar = CreateMenu();
            HMENU hFileMenu = CreatePopupMenu();
            AppendMenu(hFileMenu, MF_STRING, ID_FILE_NEW, L"新建");
            AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, L"退出");
            AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"文件");
            
            HMENU hExprMenu = CreatePopupMenu();
            AppendMenu(hExprMenu, MF_STRING, ID_EXPR_EXPR1, L"实验一：基本图形");
            AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hExprMenu, L"实验");
            
            HMENU hDrawMenu = CreatePopupMenu();
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_LINE_DDA, L"直线(DDA)");
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_LINE_BRES, L"直线(Bresenham)");
            AppendMenu(hDrawMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_MID, L"圆(中点)");
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_BRES, L"圆(Bresenham)");
            AppendMenu(hDrawMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_RECTANGLE, L"矩形");
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_POLYLINE, L"多段线(右键结束)");
            AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_POLYGON, L"多边形(右键结束)");
            AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hDrawMenu, L"绘图");
            
            HMENU hFillMenu = CreatePopupMenu();
            AppendMenu(hFillMenu, MF_STRING, ID_FILL_BOUNDARY, L"边界填充");
            AppendMenu(hFillMenu, MF_STRING, ID_FILL_SCANLINE, L"扫描线填充");
            AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFillMenu, L"填充");
            
            SetMenu(hwnd, hMenuBar);
            return 0;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            g_engine.Initialize(hwnd, hdc);
            if (g_engine.GetMode() == MODE_EXPR1) g_engine.DrawExpr1Graphics();
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            HDC hdc = GetDC(hwnd);
            g_engine.Initialize(hwnd, hdc);
            g_engine.OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
            ReleaseDC(hwnd, hdc);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        
        case WM_RBUTTONDOWN: {
            HDC hdc = GetDC(hwnd);
            g_engine.Initialize(hwnd, hdc);
            g_engine.OnRButtonDown(LOWORD(lParam), HIWORD(lParam));
            ReleaseDC(hwnd, hdc);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            HDC hdc = GetDC(hwnd);
            g_engine.Initialize(hwnd, hdc);
            g_engine.OnMouseMove(LOWORD(lParam), HIWORD(lParam));
            ReleaseDC(hwnd, hdc);
            return 0;
        }
        
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case ID_FILE_NEW: {
                    HDC hdc = GetDC(hwnd);
                    g_engine.Initialize(hwnd, hdc);
                    g_engine.ClearCanvas();
                    ReleaseDC(hwnd, hdc);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                }
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
                case ID_DRAW_POLYGON:
                    g_engine.SetMode(MODE_POLYGON);
                    break;
                case ID_FILL_BOUNDARY:
                    g_engine.SetMode(MODE_FILL_BOUNDARY);
                    break;
                case ID_FILL_SCANLINE:
                    g_engine.SetMode(MODE_FILL_SCANLINE);
                    break;
            }
            return 0;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
