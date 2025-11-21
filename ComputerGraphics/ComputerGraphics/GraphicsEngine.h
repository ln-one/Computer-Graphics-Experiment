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
    MODE_FILL_BOUNDARY,    // Boundary fill
    // Experiment 3: Geometric transformations and clipping
    MODE_POLYGON,          // Polygon drawing
    MODE_SELECT,           // Select shape
    MODE_TRANSLATE,        // Translation transformation
    MODE_SCALE,            // Scaling transformation
    MODE_ROTATE,           // Rotation transformation
    MODE_CLIP_COHEN_SUTHERLAND,    // Cohen-Sutherland line clipping
    MODE_CLIP_MIDPOINT,            // Midpoint subdivision line clipping
    MODE_CLIP_SUTHERLAND_HODGMAN,  // Sutherland-Hodgman polygon clipping
    MODE_CLIP_WEILER_ATHERTON      // Weiler-Atherton polygon clipping
};

// Point structure
struct Point2D
{
    int x, y;
    Point2D(int x = 0, int y = 0) : x(x), y(y) {}
};

// Shape type enumeration
enum ShapeType
{
    SHAPE_LINE,
    SHAPE_CIRCLE,
    SHAPE_RECTANGLE,
    SHAPE_POLYLINE,
    SHAPE_POLYGON,
    SHAPE_BSPLINE
};

// Shape structure to store drawn graphics
struct Shape
{
    ShapeType type;
    std::vector<Point2D> points;
    COLORREF color;
    int radius;  // For circles
    bool selected;

    Shape() : type(SHAPE_LINE), color(RGB(0, 0, 0)), radius(0), selected(false) {}
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

    // Shape object management (Experiment 3)
    std::vector<Shape> shapes;           // Storage for all drawn shapes
    int selectedShapeIndex;              // Index of currently selected shape (-1 if none)
    bool hasSelection;                   // Flag indicating if a shape is selected

    // Transformation state variables
    Point2D transformStartPoint;         // Starting point for transformation
    Point2D transformAnchorPoint;        // Anchor point (e.g., rotation center)
    bool isTransforming;                 // Flag indicating if transformation is in progress
    Shape previewShape;                  // Preview of transformed shape
    double initialDistance;              // Initial distance for scaling
    double initialAngle;                 // Initial angle for rotation

    // Clipping state variables
    Point2D clipWindowStart;             // Starting point for clipping window
    Point2D clipWindowEnd;               // Ending point for clipping window
    bool isDefiningClipWindow;           // Flag indicating if defining clipping window
    bool hasClipWindow;                  // Flag indicating if clipping window is defined

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

    // Experiment 3: Polygon drawing
    void DrawPolygon(const std::vector<Point2D>& points, COLORREF color = RGB(0, 0, 0));

    // Helper functions
    void SetPixel(int x, int y, COLORREF color);
    COLORREF GetPixel(int x, int y);
    void ClearCanvas();
    void RenderAll();
    
    // Clipping window functions
    void DrawClipWindow(Point2D p1, Point2D p2, bool isDashed = true);
    
    // Cohen-Sutherland line clipping
    void ExecuteCohenSutherlandClipping();
    
    // Midpoint subdivision line clipping
    void ExecuteMidpointClipping();

    // Selection functions
    int SelectShapeAt(int x, int y);
    void DeselectAll();
    void DrawSelectionIndicator(const Shape& shape);

    // Transformation helper functions
    Point2D CalculateShapeCenter(const Shape& shape);
    void ApplyTranslation(Shape& shape, int dx, int dy);
    void ApplyScaling(Shape& shape, double scale, Point2D center);
    void ApplyRotation(Shape& shape, double angle, Point2D center);
    Shape CreateTransformedPreview(const Shape& shape);

private:
    Point2D CalculateBSplinePoint(float t, const std::vector<Point2D> &controlPoints);
    
    // Cohen-Sutherland clipping helper functions
    enum OutCode {
        INSIDE = 0,  // 0000
        LEFT = 1,    // 0001
        RIGHT = 2,   // 0010
        BOTTOM = 4,  // 0100
        TOP = 8      // 1000
    };
    
    int ComputeOutCode(Point2D point, int xmin, int ymin, int xmax, int ymax);
    bool ClipLineCohenSutherland(Point2D& p1, Point2D& p2, int xmin, int ymin, int xmax, int ymax);
    
    // Midpoint subdivision clipping helper functions
    bool IsInsideWindow(Point2D point, int xmin, int ymin, int xmax, int ymax);
    bool IsOutsideSameSide(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax);
    void ClipLineMidpointRecursive(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax,
                                    std::vector<std::pair<Point2D, Point2D>>& result, int depth = 0);
    
    // Hit test helper functions
    bool HitTestLine(Point2D point, Point2D p1, Point2D p2, int tolerance = 5);
    bool HitTestCircle(Point2D point, Point2D center, int radius, int tolerance = 5);
    bool HitTestPolygon(Point2D point, const std::vector<Point2D>& polygon);
    double PointToLineDistance(Point2D point, Point2D lineStart, Point2D lineEnd);
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
#define ID_DRAW_POLYGON 40208
#define ID_FILL_SCANLINE 40301
#define ID_FILL_BOUNDARY 40302
#define ID_TRANSFORM_SELECT 40501
#define ID_TRANSFORM_TRANSLATE 40502
#define ID_TRANSFORM_SCALE 40503
#define ID_TRANSFORM_ROTATE 40504
#define ID_CLIP_COHEN_SUTHERLAND 40601
#define ID_CLIP_MIDPOINT 40602
#define ID_CLIP_SUTHERLAND_HODGMAN 40603
#define ID_CLIP_WEILER_ATHERTON 40604
#define ID_HELP_ABOUT 40401