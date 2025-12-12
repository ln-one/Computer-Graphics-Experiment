#include "../framework.h"
#include "engine/GraphicsEngine.h"
#include "engine/GraphicsEngine3D.h"
#include "ui/MenuIDs.h"

GraphicsEngine g_engine;
GraphicsEngine3D g_engine3D;
bool is3DMode = false;

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
            
            // 新增：模式切换菜单
            HMENU hModeMenu = CreatePopupMenu();
            AppendMenuW(hModeMenu, MF_STRING, ID_MODE_2D, L"2D Mode");
            AppendMenuW(hModeMenu, MF_STRING, ID_MODE_3D, L"3D Mode");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hModeMenu, L"Mode");
            
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
            
            HMENU hTransformMenu = CreatePopupMenu();
            AppendMenuW(hTransformMenu, MF_STRING, ID_TRANSFORM_SELECT, L"Select Shape");
            AppendMenuW(hTransformMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hTransformMenu, MF_STRING, ID_TRANSFORM_TRANSLATE, L"Translate");
            AppendMenuW(hTransformMenu, MF_STRING, ID_TRANSFORM_SCALE, L"Scale");
            AppendMenuW(hTransformMenu, MF_STRING, ID_TRANSFORM_ROTATE, L"Rotate");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hTransformMenu, L"Transform");
            
            HMENU hClipMenu = CreatePopupMenu();
            HMENU hLineClipMenu = CreatePopupMenu();
            AppendMenuW(hLineClipMenu, MF_STRING, ID_CLIP_COHEN_SUTHERLAND, L"Cohen-Sutherland");
            AppendMenuW(hLineClipMenu, MF_STRING, ID_CLIP_MIDPOINT, L"Midpoint Subdivision");
            AppendMenuW(hClipMenu, MF_POPUP, (UINT_PTR)hLineClipMenu, L"Line Clipping");
            
            HMENU hPolyClipMenu = CreatePopupMenu();
            AppendMenuW(hPolyClipMenu, MF_STRING, ID_CLIP_SUTHERLAND_HODGMAN, L"Sutherland-Hodgman");
            AppendMenuW(hPolyClipMenu, MF_STRING, ID_CLIP_WEILER_ATHERTON, L"Weiler-Atherton");
            AppendMenuW(hClipMenu, MF_POPUP, (UINT_PTR)hPolyClipMenu, L"Polygon Clipping");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hClipMenu, L"Clipping");
            
            // 新增：3D图形菜单
            HMENU h3DMenu = CreatePopupMenu();
            AppendMenuW(h3DMenu, MF_STRING, ID_3D_SPHERE, L"Sphere");
            AppendMenuW(h3DMenu, MF_STRING, ID_3D_CYLINDER, L"Cylinder");
            AppendMenuW(h3DMenu, MF_STRING, ID_3D_PLANE, L"Plane");
            AppendMenuW(h3DMenu, MF_STRING, ID_3D_CUBE, L"Cube");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)h3DMenu, L"3D Shapes");
            
            // 新增：3D控制菜单
            HMENU h3DControlMenu = CreatePopupMenu();
            AppendMenuW(h3DControlMenu, MF_STRING, ID_3D_SELECT, L"Select");
            AppendMenuW(h3DControlMenu, MF_STRING, ID_3D_VIEW_CONTROL, L"View Control");
            AppendMenuW(h3DControlMenu, MF_STRING, ID_3D_LIGHTING, L"Lighting");
            AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)h3DControlMenu, L"3D Control");
            
            SetMenu(hwnd, hMenuBar);
            
            // 初始化3D引擎
            g_engine3D.Initialize(hwnd);
            
            return 0;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            if (is3DMode) {
                g_engine3D.Render();  // 3D渲染
            } else {
                g_engine.Initialize(hwnd, hdc);
                if (g_engine.GetMode() == MODE_EXPR1) {
                    g_engine.DrawExpr1Graphics();
                } else {
                    g_engine.RenderAll();
                }
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            if (is3DMode) {
                g_engine3D.OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
            } else {
                HDC hdc = GetDC(hwnd);
                g_engine.Initialize(hwnd, hdc);
                g_engine.OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
                ReleaseDC(hwnd, hdc);
            }
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
                
                // 模式切换
                case ID_MODE_2D:
                    is3DMode = false;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case ID_MODE_3D:
                    is3DMode = true;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                
                // 3D图形绘制
                case ID_3D_SPHERE:
                    g_engine3D.SetMode(MODE_3D_SPHERE);
                    break;
                case ID_3D_CYLINDER:
                    g_engine3D.SetMode(MODE_3D_CYLINDER);
                    break;
                case ID_3D_PLANE:
                    g_engine3D.SetMode(MODE_3D_PLANE);
                    break;
                case ID_3D_CUBE:
                    g_engine3D.SetMode(MODE_3D_CUBE);
                    break;
                
                // 3D控制
                case ID_3D_SELECT:
                    g_engine3D.SetMode(MODE_3D_SELECT);
                    break;
                case ID_3D_VIEW_CONTROL:
                    g_engine3D.SetMode(MODE_3D_VIEW_CONTROL);
                    break;
                case ID_3D_LIGHTING:
                    // 光照对话框（后续实现）
                    break;
            }
            return 0;
        }
        
        case WM_DESTROY:
            g_engine3D.Shutdown();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
