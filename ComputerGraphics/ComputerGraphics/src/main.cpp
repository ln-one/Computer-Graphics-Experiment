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

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Computer Graphics Experiment",
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
            AppendMenuW(hFileMenu, MF_STRING, ID_FILE_NEW, L"New");
            AppendMenuW(hFileMenu, MF_STRING, ID_FILE_EXIT, L"Exit");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
            
            HMENU hExprMenu = CreatePopupMenu();
            AppendMenuW(hExprMenu, MF_STRING, ID_EXPR_EXPR1, L"Experiment 1");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hExprMenu, L"Experiment");
            
            HMENU hDrawMenu = CreatePopupMenu();
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_LINE_DDA, L"Line (DDA)");
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_LINE_BRES, L"Line (Bresenham)");
            AppendMenuW(hDrawMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_MID, L"Circle (Midpoint)");
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_BRES, L"Circle (Bresenham)");
            AppendMenuW(hDrawMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_RECTANGLE, L"Rectangle");
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_POLYLINE, L"Polyline (Right-click to end)");
            AppendMenuW(hDrawMenu, MF_STRING, ID_DRAW_POLYGON, L"Polygon (Right-click to end)");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hDrawMenu, L"Draw");
            
            HMENU hFillMenu = CreatePopupMenu();
            AppendMenuW(hFillMenu, MF_STRING, ID_FILL_BOUNDARY, L"Boundary Fill");
            AppendMenuW(hFillMenu, MF_STRING, ID_FILL_SCANLINE, L"Scanline Fill");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hFillMenu, L"Fill");
            
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
