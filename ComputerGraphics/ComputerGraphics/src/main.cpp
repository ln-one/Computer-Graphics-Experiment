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
            HMENU hMenuBar = CreateMenu();
            HMENU hFileMenu = CreatePopupMenu();
            AppendMenuW(hFileMenu, MF_STRING, ID_FILE_NEW, L"新建");
            AppendMenuW(hFileMenu, MF_STRING, ID_FILE_EXIT, L"退出");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"文件");
            
            HMENU hExprMenu = CreatePopupMenu();
            AppendMenuW(hExprMenu, MF_STRING, ID_EXPR_EXPR1, L"实验一");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hExprMenu, L"实验");
            
            HMENU hDrawMenu = CreatePopupMenu();
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_LINE_DDA, L"直线 (DDA算法)");
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_LINE_BRES, L"直线 (Bresenham算法)");
            AppendMenuW(hDrawMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_MID, L"圆 (中点算法)");
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_BRES, L"圆 (Bresenham算法)");
            AppendMenuW(hDrawMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_RECTANGLE, L"矩形");
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_POLYLINE, L"折线 (右键结束)");
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_POLYGON, L"多边形 (右键结束)");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hDrawMenu, L"绘制");
            
            HMENU hFillMenu = CreatePopupMenu();
            AppendMenuW(hFillMenu, MF_STRING, ID_FILL_BOUNDARY, L"边界填充");
            AppendMenuW(hFillMenu, MF_STRING, ID_FILL_SCANLINE, L"扫描线填充");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hFillMenu, L"填充");
            
            HMENU hTransformMenu = CreatePopupMenu();
            AppendMenuW(hTransformMenu, MF_STRING, ID_TRANSFORM_SELECT, L"选择图形");
            AppendMenuW(hTransformMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hTransformMenu, MF_STRING, ID_TRANSFORM_TRANSLATE, L"平移");
            AppendMenuW(hTransformMenu, MF_STRING, ID_TRANSFORM_SCALE, L"缩放");
            AppendMenuW(hTransformMenu, MF_STRING, ID_TRANSFORM_ROTATE, L"旋转");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hTransformMenu, L"变换");
            
            HMENU hClipMenu = CreatePopupMenu();
            HMENU hLineClipMenu = CreatePopupMenu();
            AppendMenuW(hLineClipMenu, MF_STRING, ID_CLIP_COHEN_SUTHERLAND, L"Cohen-Sutherland算法");
            AppendMenuW(hLineClipMenu, MF_STRING, ID_CLIP_MIDPOINT, L"中点分割算法");
            AppendMenuW(hClipMenu, MF_POPUP, (UINT_PTR)hLineClipMenu, L"线段裁剪");
            
            HMENU hPolyClipMenu = CreatePopupMenu();
            AppendMenuW(hPolyClipMenu, MF_STRING, ID_CLIP_SUTHERLAND_HODGMAN, L"Sutherland-Hodgman算法");
            AppendMenuW(hPolyClipMenu, MF_STRING, ID_CLIP_WEILER_ATHERTON, L"Weiler-Atherton算法");
            AppendMenuW(hClipMenu, MF_POPUP, (UINT_PTR)hPolyClipMenu, L"多边形裁剪");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hClipMenu, L"裁剪");
            
            SetMenu(hwnd, hMenuBar);
            return 0;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            g_engine.Initialize(hwnd, hdc);
            if (g_engine.GetMode() == MODE_EXPR1) {
                g_engine.DrawExpr1Graphics();
            } else {
                g_engine.RenderAll();
            }
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
                case ID_TRANSFORM_SELECT:
                    g_engine.SetMode(MODE_SELECT);
                    break;
                case ID_TRANSFORM_TRANSLATE:
                    g_engine.SetMode(MODE_TRANSLATE);
                    break;
                case ID_TRANSFORM_SCALE:
                    g_engine.SetMode(MODE_SCALE);
                    break;
                case ID_TRANSFORM_ROTATE:
                    g_engine.SetMode(MODE_ROTATE);
                    break;
                case ID_CLIP_COHEN_SUTHERLAND:
                    g_engine.SetMode(MODE_CLIP_COHEN_SUTHERLAND);
                    break;
                case ID_CLIP_MIDPOINT:
                    g_engine.SetMode(MODE_CLIP_MIDPOINT);
                    break;
                case ID_CLIP_SUTHERLAND_HODGMAN:
                    g_engine.SetMode(MODE_CLIP_SUTHERLAND_HODGMAN);
                    break;
                case ID_CLIP_WEILER_ATHERTON:
                    g_engine.SetMode(MODE_CLIP_WEILER_ATHERTON);
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
