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
        AppendMenu(hFileMenu, MF_STRING, ID_FILE_NEW, L"新建(&N)\tCtrl+N");
        AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, L"退出(&X)");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"文件(&F)");

        HMENU hExprMenu = CreatePopupMenu();
        AppendMenu(hExprMenu, MF_STRING, ID_EXPR_EXPR1, L"实验一：基本图形绘制");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hExprMenu, L"实验(&E)");

        HMENU hDrawMenu = CreatePopupMenu();
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_LINE_DDA, L"直线（DDA算法）");
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_LINE_BRES, L"直线（Bresenham算法）");
        AppendMenu(hDrawMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_MID, L"圆（中点算法）");
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_CIRCLE_BRES, L"圆（Bresenham算法）");
        AppendMenu(hDrawMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_RECTANGLE, L"矩形");
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_POLYLINE, L"多段线（右键结束）");
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_POLYGON, L"多边形（右键结束）");
        AppendMenu(hDrawMenu, MF_STRING, ID_DRAW_BSPLINE, L"B样条曲线（4个控制点）");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hDrawMenu, L"绘图(&D)");

        HMENU hFillMenu = CreatePopupMenu();
        AppendMenu(hFillMenu, MF_STRING, ID_FILL_SCANLINE, L"扫描线填充（右键结束）");
        AppendMenu(hFillMenu, MF_STRING, ID_FILL_BOUNDARY, L"边界填充");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFillMenu, L"填充(&I)");

        // Transformation menu (Experiment 3)
        HMENU hTransformMenu = CreatePopupMenu();
        AppendMenu(hTransformMenu, MF_STRING, ID_TRANSFORM_SELECT, L"选择图形");
        AppendMenu(hTransformMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hTransformMenu, MF_STRING, ID_TRANSFORM_TRANSLATE, L"平移");
        AppendMenu(hTransformMenu, MF_STRING, ID_TRANSFORM_SCALE, L"缩放");
        AppendMenu(hTransformMenu, MF_STRING, ID_TRANSFORM_ROTATE, L"旋转");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hTransformMenu, L"变换(&T)");

        // Clipping menu (Experiment 3)
        HMENU hClipMenu = CreatePopupMenu();
        HMENU hLineClipMenu = CreatePopupMenu();
        AppendMenu(hLineClipMenu, MF_STRING, ID_CLIP_COHEN_SUTHERLAND, L"Cohen-Sutherland算法");
        AppendMenu(hLineClipMenu, MF_STRING, ID_CLIP_MIDPOINT, L"中点分割算法");
        AppendMenu(hClipMenu, MF_POPUP, (UINT_PTR)hLineClipMenu, L"直线裁剪");
        
        HMENU hPolyClipMenu = CreatePopupMenu();
        AppendMenu(hPolyClipMenu, MF_STRING, ID_CLIP_SUTHERLAND_HODGMAN, L"Sutherland-Hodgman算法");
        AppendMenu(hPolyClipMenu, MF_STRING, ID_CLIP_WEILER_ATHERTON, L"Weiler-Atherton算法");
        AppendMenu(hClipMenu, MF_POPUP, (UINT_PTR)hPolyClipMenu, L"多边形裁剪");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hClipMenu, L"裁剪(&C)");

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

        case ID_DRAW_POLYGON:
            g_engine.SetMode(MODE_POLYGON);
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

        case ID_HELP_ABOUT:
            MessageBox(hwnd,
                       L"Computer Graphics Experiment\n\n"
                       L"Student ID: 2023112573\n"
                       L"Name: Zhang Chunran\n"
                       L"School: Southwest Jiaotong University\n"
                       L"College: Computer and AI\n"
                       L"Class: Software Engineering 2023-02\n\n"
                       L"Experiment 1: Basic Graphics Drawing\n"
                       L"Experiment 2: Basic Graphics Algorithms\n"
                       L"  - DDA Line Algorithm\n"
                       L"  - Bresenham Line Algorithm\n"
                       L"  - Midpoint Circle Algorithm\n"
                       L"  - Bresenham Circle Algorithm\n"
                       L"  - Rectangle, Polyline, B-spline\n"
                       L"  - Scanline Fill, Boundary Fill\n"
                       L"Experiment 3: Transformations and Clipping\n"
                       L"  - Polygon Drawing\n"
                       L"  - Shape Selection and Transformation\n"
                       L"  - Line Clipping (Cohen-Sutherland, Midpoint)\n"
                       L"  - Polygon Clipping (Sutherland-Hodgman, Weiler-Atherton)",
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
        L"计算机图形学实验 - 2023112573 张春冉 - 西南交通大学 软件工程2023-02班",
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