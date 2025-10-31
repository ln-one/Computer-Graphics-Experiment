#include "framework.h"
#include "GraphicsEngine.h"
#include "resource.h"

GraphicsEngine g_engine;
HINSTANCE g_hInst;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        HMENU hMenuBar = CreateMenu();

        HMENU hFileMenu = CreatePopupMenu();
        AppendMenu(hFileMenu, MF_STRING, ID_FILE_NEW, L"New(&N)\tCtrl+N");
        AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, L"Exit(&X)");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"File(&F)");

        HMENU hExprMenu = CreatePopupMenu();
        AppendMenu(hExprMenu, MF_STRING, ID_EXPR_EXPR1, L"Experiment 1: Basic Graphics");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hExprMenu, L"Experiment(&E)");

        HMENU hDrawMenu = CreatePopupMenu();
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_LINE_DDA, L"Line (DDA Algorithm)");
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_LINE_BRES, L"Line (Bresenham Algorithm)");
        AppendMenu(hDrawMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_MID, L"Circle (Midpoint Algorithm)");
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_BRES, L"Circle (Bresenham Algorithm)");
        AppendMenu(hDrawMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_RECTANGLE, L"Rectangle");
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_POLYLINE, L"Polyline (Right-click to end)");
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_BSPLINE, L"B-Spline Curve (4 control points)");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hDrawMenu, L"Draw(&D)");

        HMENU hFillMenu = CreatePopupMenu();
        AppendMenu(hFillMenu, MF_STRING, ID_FILL_SCANLINE, L"Scanline Fill");
        AppendMenu(hFillMenu, MF_STRING, ID_FILL_BOUNDARY, L"Boundary Fill");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFillMenu, L"Fill(&I)");

        HMENU hHelpMenu = CreatePopupMenu();
        AppendMenu(hHelpMenu, MF_STRING, ID_HELP_ABOUT, L"About(&A)");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hHelpMenu, L"Help(&H)");

        SetMenu(hwnd, hMenuBar);
    }
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        g_engine.Initialize(hwnd, hdc);

        if (g_engine.GetMode() == MODE_EXPR1)
        {
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
        switch (wmId)
        {
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
                       L"Computer Graphics Comprehensive Experiment\n"
                       L"Student ID: 2023112573\n"
                       L"Name: Zhang Chunran\n\n"
                       L"Experiment 1: Basic Graphics Drawing\n"
                       L"Experiment 2: Basic Graphics Generation Algorithms\n"
                       L"- DDA and Bresenham Line Algorithms\n"
                       L"- Midpoint and Bresenham Circle Algorithms\n"
                       L"- Rectangle, Polyline, B-Spline Curve\n"
                       L"- Scanline Fill and Boundary Fill",
                       L"About", MB_OK | MB_ICONINFORMATION);
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

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"SWJTU_ZCR_2023112573_ComputerGraphics";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"2023112573-ZhangChunran-Computer Graphics Experiment",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}