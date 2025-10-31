#pragma once
#include "framework.h"
#include <vector>
#include <cmath>
#include <stack>
#include <algorithm>

// Drawing mode enumeration
enum DrawMode
{
    MODE_NONE = 0,
    MODE_EXPR1,            // Experiment 1: Basic graphics drawing
    MODE_LINE_DDA,         // DDA line algorithm
    MODE_LINE_BRESENHAM,   // Bresenham line algorithm
    MODE_CIRCLE_MIDPOINT,  // Midpoint circle algorithm
    MODE_CIRCLE_BRESENHAM, // Bresenham circle algorithm
    MODE_RECTANGLE,        // Rectangle
    MODE_POLYLINE,         // Polyline
    MODE_BSPLINE,          // B-spline curve
    MODE_FILL_SCANLINE,    // Scanline fill
    MODE_FILL_BOUNDARY     // Boundary fill
};

// Point structure
struct Point2D
{
    int x, y;
    Point2D(int x = 0, int y = 0) : x(x), y(y) {}
};

// Graphics drawing engine class
class GraphicsEngine
{
private:
    HDC hdc;
    HWND hwnd;
    DrawMode currentMode;
    std::vector<Point2D> tempPoints;
    bool isDrawing;

public:
    GraphicsEngine();
    ~GraphicsEngine();

    void Initialize(HWND hwnd, HDC hdc);
    void SetMode(DrawMode mode);
    DrawMode GetMode() const { return currentMode; }

    // Mouse event handling
    void OnLButtonDown(int x, int y);
    void OnMouseMove(int x, int y);
    void OnRButtonDown(int x, int y);

    // Experiment 1: Basic graphics drawing
    void DrawExpr1Graphics();

    // Experiment 2: Basic graphics generation algorithms
    void DrawLineDDA(Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));
    void DrawLineBresenham(Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));
    void DrawCircleMidpoint(Point2D center, int radius, COLORREF color = RGB(0, 0, 0));
    void DrawCircleBresenham(Point2D center, int radius, COLORREF color = RGB(0, 0, 0));
    void DrawRectangle(Point2D p1, Point2D p2, COLORREF color = RGB(0, 0, 0));
    void DrawPolyline(const std::vector<Point2D> &points, COLORREF color = RGB(0, 0, 0));
    void DrawBSpline(const std::vector<Point2D> &controlPoints, COLORREF color = RGB(0, 0, 0));

    // Fill algorithms
    void BoundaryFill(int x, int y, COLORREF fillColor, COLORREF boundaryColor);
    void ScanlineFill(const std::vector<Point2D>& polygon, COLORREF fillColor);

    // Helper functions
    void SetPixel(int x, int y, COLORREF color);
    COLORREF GetPixel(int x, int y);
    void ClearCanvas();

private:
    Point2D CalculateBSplinePoint(float t, const std::vector<Point2D> &controlPoints);
};

// Menu ID definitions
#define ID_FILE_NEW 40001
#define ID_FILE_EXIT 40002
#define ID_EXPR_EXPR1 40101
#define ID_DRAW_LINE_DDA 40201
#define ID_DRAW_LINE_BRES 40202
#define ID_DRAW_CIRCLE_MID 40203
#define ID_DRAW_CIRCLE_BRES 40204
#define ID_DRAW_RECTANGLE 40205
#define ID_DRAW_POLYLINE 40206
#define ID_DRAW_BSPLINE 40207
#define ID_FILL_SCANLINE 40301
#define ID_FILL_BOUNDARY 40302
#define ID_HELP_ABOUT 40401