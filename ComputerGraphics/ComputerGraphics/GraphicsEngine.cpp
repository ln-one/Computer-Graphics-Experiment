#include "GraphicsEngine.h"
#include <algorithm>

GraphicsEngine::GraphicsEngine() : hdc(nullptr), hwnd(nullptr),
                                   currentMode(MODE_NONE), isDrawing(false),
                                   selectedShapeIndex(-1), hasSelection(false),
                                   isTransforming(false), initialDistance(0.0), initialAngle(0.0),
                                   isDefiningClipWindow(false), hasClipWindow(false)
{
}

GraphicsEngine::~GraphicsEngine()
{
}

void GraphicsEngine::Initialize(HWND hwnd, HDC hdc)
{
    this->hwnd = hwnd;
    this->hdc = hdc;
}

void GraphicsEngine::SetMode(DrawMode mode)
{
    currentMode = mode;
    isDrawing = false;
    tempPoints.clear();
}

void GraphicsEngine::OnLButtonDown(int x, int y)
{
    Point2D clickPoint(x, y);

    switch (currentMode)
    {
    case MODE_LINE_DDA:
    case MODE_LINE_BRESENHAM:
        if (!isDrawing)
        {
            tempPoints.clear();
            tempPoints.push_back(clickPoint);
            isDrawing = true;
        }
        else
        {
            tempPoints.push_back(clickPoint);
            if (currentMode == MODE_LINE_DDA)
            {
                DrawLineDDA(tempPoints[0], tempPoints[1]);
            }
            else
            {
                DrawLineBresenham(tempPoints[0], tempPoints[1]);
            }
            isDrawing = false;
        }
        break;

    case MODE_CIRCLE_MIDPOINT:
    case MODE_CIRCLE_BRESENHAM:
        if (!isDrawing)
        {
            tempPoints.clear();
            tempPoints.push_back(clickPoint);
            isDrawing = true;
        }
        else
        {
            tempPoints.push_back(clickPoint);
            int radius = (int)sqrt(pow(tempPoints[1].x - tempPoints[0].x, 2) +
                                   pow(tempPoints[1].y - tempPoints[0].y, 2));
            if (currentMode == MODE_CIRCLE_MIDPOINT)
            {
                DrawCircleMidpoint(tempPoints[0], radius);
            }
            else
            {
                DrawCircleBresenham(tempPoints[0], radius);
            }
            isDrawing = false;
        }
        break;

    case MODE_RECTANGLE:
        if (!isDrawing)
        {
            tempPoints.clear();
            tempPoints.push_back(clickPoint);
            isDrawing = true;
        }
        else
        {
            tempPoints.push_back(clickPoint);
            DrawRectangle(tempPoints[0], tempPoints[1]);
            isDrawing = false;
        }
        break;

    case MODE_POLYLINE:
        tempPoints.push_back(clickPoint);
        if (!isDrawing)
        {
            isDrawing = true;
        }
        break;

    case MODE_BSPLINE:
        tempPoints.push_back(clickPoint);
        if (!isDrawing)
        {
            isDrawing = true;
        }
        if (tempPoints.size() >= 4)
        {
            DrawBSpline(tempPoints);
            tempPoints.clear();
            isDrawing = false;
        }
        break;

    case MODE_FILL_BOUNDARY:
        BoundaryFill(x, y, RGB(255, 0, 0), RGB(0, 0, 0));
        break;
        
    case MODE_FILL_SCANLINE:
        tempPoints.push_back(clickPoint);
        if (!isDrawing) {
            isDrawing = true;
        }
        // Need at least 3 points to form a polygon
        if (tempPoints.size() >= 3) {
            // Show current polygon outline in real-time
            DrawPolyline(tempPoints, RGB(0, 0, 0));
        }
        break;

    case MODE_POLYGON:
        tempPoints.push_back(clickPoint);
        if (!isDrawing)
        {
            isDrawing = true;
        }
        // Draw current polygon edges as preview
        if (tempPoints.size() >= 2)
        {
            DrawPolyline(tempPoints, RGB(0, 0, 0));
        }
        break;

    case MODE_SELECT:
        // Handle shape selection
        {
            int hitIndex = SelectShapeAt(x, y);
            if (hitIndex >= 0)
            {
                // Deselect all shapes first
                DeselectAll();
                
                // Select the clicked shape
                shapes[hitIndex].selected = true;
                selectedShapeIndex = hitIndex;
                hasSelection = true;
                
                // Redraw to show selection
                ClearCanvas();
                RenderAll();
            }
            else
            {
                // Clicked on empty area, deselect all
                if (hasSelection)
                {
                    DeselectAll();
                    ClearCanvas();
                    RenderAll();
                }
            }
        }
        break;

    case MODE_TRANSLATE:
        // Handle translation
        if (!hasSelection)
        {
            MessageBox(hwnd, L"Please select a shape first", L"Translation", MB_OK | MB_ICONINFORMATION);
            break;
        }
        
        if (!isTransforming)
        {
            // Start translation - record starting point
            transformStartPoint = clickPoint;
            isTransforming = true;
        }
        else
        {
            // End translation - apply the transformation
            int dx = clickPoint.x - transformStartPoint.x;
            int dy = clickPoint.y - transformStartPoint.y;
            
            ApplyTranslation(shapes[selectedShapeIndex], dx, dy);
            
            isTransforming = false;
            ClearCanvas();
            RenderAll();
        }
        break;

    case MODE_SCALE:
        // Handle scaling
        if (!hasSelection)
        {
            MessageBox(hwnd, L"Please select a shape first", L"Scaling", MB_OK | MB_ICONINFORMATION);
            break;
        }
        
        if (!isTransforming)
        {
            // Start scaling - record anchor point (shape center) and initial distance
            transformAnchorPoint = CalculateShapeCenter(shapes[selectedShapeIndex]);
            transformStartPoint = clickPoint;
            
            int dx = clickPoint.x - transformAnchorPoint.x;
            int dy = clickPoint.y - transformAnchorPoint.y;
            initialDistance = sqrt(dx * dx + dy * dy);
            
            if (initialDistance < 1.0)
            {
                initialDistance = 1.0; // Avoid division by zero
            }
            
            isTransforming = true;
        }
        else
        {
            // End scaling - apply the transformation
            int dx = clickPoint.x - transformAnchorPoint.x;
            int dy = clickPoint.y - transformAnchorPoint.y;
            double currentDistance = sqrt(dx * dx + dy * dy);
            
            double scale = currentDistance / initialDistance;
            
            ApplyScaling(shapes[selectedShapeIndex], scale, transformAnchorPoint);
            
            isTransforming = false;
            ClearCanvas();
            RenderAll();
        }
        break;

    case MODE_ROTATE:
        // Handle rotation
        if (!hasSelection)
        {
            MessageBox(hwnd, L"Please select a shape first", L"Rotation", MB_OK | MB_ICONINFORMATION);
            break;
        }
        
        if (!isTransforming)
        {
            // First click - set rotation center
            transformAnchorPoint = clickPoint;
            isTransforming = true;
            
            // Draw a marker at rotation center
            ClearCanvas();
            RenderAll();
            
            // Draw rotation center marker
            HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            
            int markerSize = 5;
            MoveToEx(hdc, clickPoint.x - markerSize, clickPoint.y, NULL);
            LineTo(hdc, clickPoint.x + markerSize, clickPoint.y);
            MoveToEx(hdc, clickPoint.x, clickPoint.y - markerSize, NULL);
            LineTo(hdc, clickPoint.x, clickPoint.y + markerSize);
            
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
        }
        else
        {
            // Second click - calculate and apply rotation
            // Calculate initial angle (from first click after setting center)
            // This is handled in mouse move, so we just finalize here
            isTransforming = false;
            ClearCanvas();
            RenderAll();
        }
        break;

    case MODE_CLIP_COHEN_SUTHERLAND:
    case MODE_CLIP_MIDPOINT:
    case MODE_CLIP_SUTHERLAND_HODGMAN:
    case MODE_CLIP_WEILER_ATHERTON:
        // Handle clipping window definition
        if (!isDefiningClipWindow)
        {
            // Start defining clipping window
            clipWindowStart = clickPoint;
            isDefiningClipWindow = true;
        }
        else
        {
            // Complete clipping window definition
            clipWindowEnd = clickPoint;
            isDefiningClipWindow = false;
            hasClipWindow = true;
            
            // Redraw with final clipping window
            ClearCanvas();
            RenderAll();
            DrawClipWindow(clipWindowStart, clipWindowEnd, false);
            
            // TODO: Execute clipping algorithm based on current mode
            // This will be implemented in subsequent tasks
            MessageBox(hwnd, L"Clipping window defined. Clipping algorithm will be executed in next tasks.", 
                      L"Clipping Window", MB_OK | MB_ICONINFORMATION);
        }
        break;
    }
}

void GraphicsEngine::OnRButtonDown(int x, int y)
{
    if (currentMode == MODE_POLYLINE && tempPoints.size() >= 2)
    {
        DrawPolyline(tempPoints);
        tempPoints.clear();
        isDrawing = false;
    }
    else if (currentMode == MODE_FILL_SCANLINE && tempPoints.size() >= 3)
    {
        // Close polygon and fill
        std::vector<Point2D> polygon = tempPoints;
        DrawPolyline(polygon, RGB(0, 0, 0)); // Draw boundary
        ScanlineFill(polygon, RGB(255, 0, 0)); // Fill
        tempPoints.clear();
        isDrawing = false;
    }
    else if (currentMode == MODE_POLYGON && tempPoints.size() >= 3)
    {
        // Complete polygon and save to shapes list
        DrawPolygon(tempPoints, RGB(0, 0, 0));
        
        // Save polygon to shapes vector
        Shape polygon;
        polygon.type = SHAPE_POLYGON;
        polygon.points = tempPoints;
        polygon.color = RGB(0, 0, 0);
        polygon.selected = false;
        shapes.push_back(polygon);
        
        tempPoints.clear();
        isDrawing = false;
    }
    else if (currentMode == MODE_POLYGON && tempPoints.size() < 3)
    {
        // Show error message if less than 3 vertices
        MessageBox(hwnd, L"Polygon requires at least 3 vertices", L"Warning", MB_OK | MB_ICONWARNING);
        tempPoints.clear();
        isDrawing = false;
    }
    else if (currentMode == MODE_ROTATE && isTransforming && hasSelection)
    {
        // Complete rotation with right click
        Point2D currentPoint(x, y);
        
        // Calculate angle from anchor to current point
        int dx = currentPoint.x - transformAnchorPoint.x;
        int dy = currentPoint.y - transformAnchorPoint.y;
        double angle = atan2(dy, dx);
        
        // Apply rotation
        ApplyRotation(shapes[selectedShapeIndex], angle - initialAngle, transformAnchorPoint);
        
        isTransforming = false;
        ClearCanvas();
        RenderAll();
    }
}

void GraphicsEngine::OnMouseMove(int x, int y)
{
    Point2D currentPoint(x, y);

    // Handle polygon drawing preview
    if (isDrawing && currentMode == MODE_POLYGON && tempPoints.size() >= 1)
    {
        // Redraw all shapes first
        ClearCanvas();
        RenderAll();
        
        // Draw existing polygon edges
        if (tempPoints.size() >= 2)
        {
            DrawPolyline(tempPoints, RGB(0, 0, 0));
        }
        
        // Draw preview line from last point to current mouse position
        DrawLineBresenham(tempPoints.back(), currentPoint, RGB(128, 128, 128));
        
        // Draw preview closing line (dashed effect by drawing every other pixel)
        if (tempPoints.size() >= 2)
        {
            Point2D first = tempPoints.front();
            int dx = abs(currentPoint.x - first.x);
            int dy = abs(currentPoint.y - first.y);
            int steps = (dx > dy) ? dx : dy;
            
            if (steps > 0)
            {
                float xInc = (float)(currentPoint.x - first.x) / steps;
                float yInc = (float)(currentPoint.y - first.y) / steps;
                
                for (int i = 0; i <= steps; i += 2)
                {
                    int px = first.x + (int)(i * xInc);
                    int py = first.y + (int)(i * yInc);
                    SetPixel(px, py, RGB(128, 128, 128));
                }
            }
        }
        return;
    }

    // Handle transformation previews
    if (!isTransforming || !hasSelection)
        return;

    // Translation preview
    if (currentMode == MODE_TRANSLATE)
    {
        ClearCanvas();
        RenderAll();
        
        // Calculate translation offset
        int dx = currentPoint.x - transformStartPoint.x;
        int dy = currentPoint.y - transformStartPoint.y;
        
        // Create preview shape
        Shape preview = shapes[selectedShapeIndex];
        ApplyTranslation(preview, dx, dy);
        
        // Draw preview with different color (semi-transparent effect using gray)
        COLORREF previewColor = RGB(128, 128, 255);
        
        switch (preview.type)
        {
        case SHAPE_LINE:
            if (preview.points.size() >= 2)
                DrawLineBresenham(preview.points[0], preview.points[1], previewColor);
            break;
        case SHAPE_CIRCLE:
            if (preview.points.size() >= 1)
                DrawCircleBresenham(preview.points[0], preview.radius, previewColor);
            break;
        case SHAPE_RECTANGLE:
            if (preview.points.size() >= 2)
                DrawRectangle(preview.points[0], preview.points[1], previewColor);
            break;
        case SHAPE_POLYLINE:
            if (preview.points.size() >= 2)
                DrawPolyline(preview.points, previewColor);
            break;
        case SHAPE_POLYGON:
            if (preview.points.size() >= 3)
                DrawPolygon(preview.points, previewColor);
            break;
        case SHAPE_BSPLINE:
            if (preview.points.size() >= 4)
                DrawBSpline(preview.points, previewColor);
            break;
        }
    }
    // Scaling preview
    else if (currentMode == MODE_SCALE)
    {
        ClearCanvas();
        RenderAll();
        
        // Calculate scale factor
        int dx = currentPoint.x - transformAnchorPoint.x;
        int dy = currentPoint.y - transformAnchorPoint.y;
        double currentDistance = sqrt(dx * dx + dy * dy);
        double scale = currentDistance / initialDistance;
        
        // Create preview shape
        Shape preview = shapes[selectedShapeIndex];
        ApplyScaling(preview, scale, transformAnchorPoint);
        
        // Draw preview
        COLORREF previewColor = RGB(128, 128, 255);
        
        switch (preview.type)
        {
        case SHAPE_LINE:
            if (preview.points.size() >= 2)
                DrawLineBresenham(preview.points[0], preview.points[1], previewColor);
            break;
        case SHAPE_CIRCLE:
            if (preview.points.size() >= 1)
                DrawCircleBresenham(preview.points[0], preview.radius, previewColor);
            break;
        case SHAPE_RECTANGLE:
            if (preview.points.size() >= 2)
                DrawRectangle(preview.points[0], preview.points[1], previewColor);
            break;
        case SHAPE_POLYLINE:
            if (preview.points.size() >= 2)
                DrawPolyline(preview.points, previewColor);
            break;
        case SHAPE_POLYGON:
            if (preview.points.size() >= 3)
                DrawPolygon(preview.points, previewColor);
            break;
        case SHAPE_BSPLINE:
            if (preview.points.size() >= 4)
                DrawBSpline(preview.points, previewColor);
            break;
        }
        
        // Draw scaling center marker
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        
        int markerSize = 5;
        MoveToEx(hdc, transformAnchorPoint.x - markerSize, transformAnchorPoint.y, NULL);
        LineTo(hdc, transformAnchorPoint.x + markerSize, transformAnchorPoint.y);
        MoveToEx(hdc, transformAnchorPoint.x, transformAnchorPoint.y - markerSize, NULL);
        LineTo(hdc, transformAnchorPoint.x, transformAnchorPoint.y + markerSize);
        
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
    // Rotation preview
    else if (currentMode == MODE_ROTATE)
    {
        ClearCanvas();
        RenderAll();
        
        // Draw rotation center marker
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        
        int markerSize = 5;
        MoveToEx(hdc, transformAnchorPoint.x - markerSize, transformAnchorPoint.y, NULL);
        LineTo(hdc, transformAnchorPoint.x + markerSize, transformAnchorPoint.y);
        MoveToEx(hdc, transformAnchorPoint.x, transformAnchorPoint.y - markerSize, NULL);
        LineTo(hdc, transformAnchorPoint.x, transformAnchorPoint.y + markerSize);
        
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
        
        // Calculate and show rotation angle
        int dx = currentPoint.x - transformAnchorPoint.x;
        int dy = currentPoint.y - transformAnchorPoint.y;
        double currentAngle = atan2(dy, dx);
        
        // On first mouse move after setting center, record initial angle
        static bool firstMove = true;
        if (firstMove)
        {
            initialAngle = currentAngle;
            firstMove = false;
        }
        
        // Reset firstMove when not transforming
        if (!isTransforming)
        {
            firstMove = true;
        }
        
        double rotationAngle = currentAngle - initialAngle;
        
        // Create preview shape
        Shape preview = shapes[selectedShapeIndex];
        ApplyRotation(preview, rotationAngle, transformAnchorPoint);
        
        // Draw preview
        COLORREF previewColor = RGB(128, 128, 255);
        
        switch (preview.type)
        {
        case SHAPE_LINE:
            if (preview.points.size() >= 2)
                DrawLineBresenham(preview.points[0], preview.points[1], previewColor);
            break;
        case SHAPE_CIRCLE:
            if (preview.points.size() >= 1)
                DrawCircleBresenham(preview.points[0], preview.radius, previewColor);
            break;
        case SHAPE_RECTANGLE:
            if (preview.points.size() >= 2)
                DrawRectangle(preview.points[0], preview.points[1], previewColor);
            break;
        case SHAPE_POLYLINE:
            if (preview.points.size() >= 2)
                DrawPolyline(preview.points, previewColor);
            break;
        case SHAPE_POLYGON:
            if (preview.points.size() >= 3)
                DrawPolygon(preview.points, previewColor);
            break;
        case SHAPE_BSPLINE:
            if (preview.points.size() >= 4)
                DrawBSpline(preview.points, previewColor);
            break;
        }
        
        // Draw line from center to current mouse position
        DrawLineBresenham(transformAnchorPoint, currentPoint, RGB(255, 0, 0));
    }

    // Handle clipping window preview
    if (isDefiningClipWindow && 
        (currentMode == MODE_CLIP_COHEN_SUTHERLAND ||
         currentMode == MODE_CLIP_MIDPOINT ||
         currentMode == MODE_CLIP_SUTHERLAND_HODGMAN ||
         currentMode == MODE_CLIP_WEILER_ATHERTON))
    {
        // Redraw all shapes first
        ClearCanvas();
        RenderAll();
        
        // Draw clipping window preview with dashed lines
        DrawClipWindow(clipWindowStart, currentPoint, true);
    }
}

void GraphicsEngine::DrawExpr1Graphics()
{
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    int offsetX = 100, offsetY = 100;
    int scale = 5;

    // Outer rounded rectangle (66x46, R7)
    RoundRect(hdc,
              offsetX, offsetY,
              offsetX + 66 * scale, offsetY + 46 * scale,
              7 * scale, 7 * scale);

    // Inner rounded rectangle hole (43x30, R3)
    RoundRect(hdc,
              offsetX + (66 - 43) / 2 * scale,
              offsetY + (46 - 30) / 2 * scale,
              offsetX + (66 + 43) / 2 * scale,
              offsetY + (46 + 30) / 2 * scale,
              3 * scale, 3 * scale);

    // Four circular holes (Phi 7)
    int holeR = static_cast<int>(7.0 / 2.0 * scale);
    int holeCenterOffsetX = (66 - 52) / 2 * scale;
    int holeCenterOffsetY = (46 - 32) / 2 * scale;

    int centers[4][2] = {
        {offsetX + holeCenterOffsetX, offsetY + holeCenterOffsetY},
        {offsetX + 66 * scale - holeCenterOffsetX, offsetY + holeCenterOffsetY},
        {offsetX + holeCenterOffsetX, offsetY + 46 * scale - holeCenterOffsetY},
        {offsetX + 66 * scale - holeCenterOffsetX, offsetY + 46 * scale - holeCenterOffsetY}};

    for (int i = 0; i < 4; i++)
    {
        Ellipse(hdc,
                centers[i][0] - holeR, centers[i][1] - holeR,
                centers[i][0] + holeR, centers[i][1] + holeR);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void GraphicsEngine::DrawLineDDA(Point2D p1, Point2D p2, COLORREF color)
{
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    if (steps == 0)
        return;

    float xIncrement = (float)dx / steps;
    float yIncrement = (float)dy / steps;

    float x = (float)p1.x;
    float y = (float)p1.y;

    for (int i = 0; i <= steps; i++)
    {
        SetPixel((int)(x + 0.5), (int)(y + 0.5), color);
        x += xIncrement;
        y += yIncrement;
    }
}

void GraphicsEngine::DrawLineBresenham(Point2D p1, Point2D p2, COLORREF color)
{
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);
    int sx = p1.x < p2.x ? 1 : -1;
    int sy = p1.y < p2.y ? 1 : -1;
    int err = dx - dy;

    int x = p1.x;
    int y = p1.y;

    while (true)
    {
        SetPixel(x, y, color);

        if (x == p2.x && y == p2.y)
            break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
}

void GraphicsEngine::DrawCircleMidpoint(Point2D center, int radius, COLORREF color)
{
    int x = 0;
    int y = radius;
    int d = 1 - radius;

    while (x <= y)
    {
        SetPixel(center.x + x, center.y + y, color);
        SetPixel(center.x - x, center.y + y, color);
        SetPixel(center.x + x, center.y - y, color);
        SetPixel(center.x - x, center.y - y, color);
        SetPixel(center.x + y, center.y + x, color);
        SetPixel(center.x - y, center.y + x, color);
        SetPixel(center.x + y, center.y - x, color);
        SetPixel(center.x - y, center.y - x, color);

        if (d < 0)
        {
            d += 2 * x + 3;
        }
        else
        {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

void GraphicsEngine::DrawCircleBresenham(Point2D center, int radius, COLORREF color)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y)
    {
        SetPixel(center.x + x, center.y + y, color);
        SetPixel(center.x - x, center.y + y, color);
        SetPixel(center.x + x, center.y - y, color);
        SetPixel(center.x - x, center.y - y, color);
        SetPixel(center.x + y, center.y + x, color);
        SetPixel(center.x - y, center.y + x, color);
        SetPixel(center.x + y, center.y - x, color);
        SetPixel(center.x - y, center.y - x, color);

        if (d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void GraphicsEngine::DrawRectangle(Point2D p1, Point2D p2, COLORREF color)
{
    DrawLineBresenham(Point2D(p1.x, p1.y), Point2D(p2.x, p1.y), color);
    DrawLineBresenham(Point2D(p2.x, p1.y), Point2D(p2.x, p2.y), color);
    DrawLineBresenham(Point2D(p2.x, p2.y), Point2D(p1.x, p2.y), color);
    DrawLineBresenham(Point2D(p1.x, p2.y), Point2D(p1.x, p1.y), color);
}

void GraphicsEngine::DrawPolyline(const std::vector<Point2D> &points, COLORREF color)
{
    for (int i = 1; i < static_cast<int>(points.size()); i++)
    {
        DrawLineBresenham(points[i - 1], points[i], color);
    }
}

void GraphicsEngine::DrawBSpline(const std::vector<Point2D> &controlPoints, COLORREF color)
{
    if (controlPoints.size() < 4)
        return;

    const int segments = 100;
    std::vector<Point2D> curvePoints;

    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        Point2D point = CalculateBSplinePoint(t, controlPoints);
        curvePoints.push_back(point);
    }

    DrawPolyline(curvePoints, color);
}

Point2D GraphicsEngine::CalculateBSplinePoint(float t, const std::vector<Point2D> &controlPoints)
{
    int n = static_cast<int>(controlPoints.size());
    if (n < 4)
        return Point2D(0, 0);

    float u = t * (n - 3);
    int k = (int)u;
    if (k >= n - 3)
        k = n - 4;
    u = u - k;

    float b0 = (1 - u) * (1 - u) * (1 - u) / 6;
    float b1 = (3 * u * u * u - 6 * u * u + 4) / 6;
    float b2 = (-3 * u * u * u + 3 * u * u + 3 * u + 1) / 6;
    float b3 = u * u * u / 6;

    Point2D result;
    result.x = (int)(b0 * controlPoints[k].x + b1 * controlPoints[k + 1].x +
                     b2 * controlPoints[k + 2].x + b3 * controlPoints[k + 3].x);
    result.y = (int)(b0 * controlPoints[k].y + b1 * controlPoints[k + 1].y +
                     b2 * controlPoints[k + 2].y + b3 * controlPoints[k + 3].y);

    return result;
}

void GraphicsEngine::BoundaryFill(int x, int y, COLORREF fillColor, COLORREF boundaryColor)
{
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    
    COLORREF startColor = GetPixel(x, y);
    if (startColor == boundaryColor || startColor == fillColor) {
        return;
    }
    
    // Use scanline seed fill algorithm
    std::stack<Point2D> seedStack;
    seedStack.push(Point2D(x, y));
    
    int maxIterations = 100000;
    int iterations = 0;
    
    // Create pen for faster line drawing
    HPEN hPen = CreatePen(PS_SOLID, 1, fillColor);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    
    while (!seedStack.empty() && iterations < maxIterations) {
        Point2D seed = seedStack.top();
        seedStack.pop();
        iterations++;
        
        if (seed.x < 0 || seed.x >= clientRect.right || 
            seed.y < 0 || seed.y >= clientRect.bottom) {
            continue;
        }
        
        COLORREF c = GetPixel(seed.x, seed.y);
        if (c == boundaryColor || c == fillColor) {
            continue;
        }
        
        // Find left and right boundaries
        int left = seed.x;
        int right = seed.x;
        
        while (left > 0) {
            c = GetPixel(left - 1, seed.y);
            if (c == boundaryColor || c == fillColor) break;
            left--;
        }
        
        while (right < clientRect.right - 1) {
            c = GetPixel(right + 1, seed.y);
            if (c == boundaryColor || c == fillColor) break;
            right++;
        }
        
        // Draw horizontal line (faster than SetPixel)
        MoveToEx(hdc, left, seed.y, NULL);
        LineTo(hdc, right + 1, seed.y);
        
        // Add seeds for adjacent scanlines
        bool inSpan = false;
        for (int i = left; i <= right; i++) {
            // Check upper line
            if (seed.y > 0) {
                c = GetPixel(i, seed.y - 1);
                if (c != boundaryColor && c != fillColor) {
                    if (!inSpan) {
                        seedStack.push(Point2D(i, seed.y - 1));
                        inSpan = true;
                    }
                } else {
                    inSpan = false;
                }
            }
        }
        
        inSpan = false;
        for (int i = left; i <= right; i++) {
            // Check lower line
            if (seed.y < clientRect.bottom - 1) {
                c = GetPixel(i, seed.y + 1);
                if (c != boundaryColor && c != fillColor) {
                    if (!inSpan) {
                        seedStack.push(Point2D(i, seed.y + 1));
                        inSpan = true;
                    }
                } else {
                    inSpan = false;
                }
            }
        }
    }
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    
    if (iterations >= maxIterations) {
        MessageBox(hwnd, L"Fill area too large, stopped filling", L"Warning", MB_OK | MB_ICONWARNING);
    }
}

void GraphicsEngine::SetPixel(int x, int y, COLORREF color)
{
    SetPixelV(hdc, x, y, color);
}

COLORREF GraphicsEngine::GetPixel(int x, int y)
{
    return ::GetPixel(hdc, x, y);
}

void GraphicsEngine::ScanlineFill(const std::vector<Point2D>& polygon, COLORREF fillColor)
{
    if (polygon.size() < 3) return;
    
    // Find polygon boundaries
    int minY = polygon[0].y, maxY = polygon[0].y;
    for (size_t i = 0; i < polygon.size(); i++) {
        if (polygon[i].y < minY) minY = polygon[i].y;
        if (polygon[i].y > maxY) maxY = polygon[i].y;
    }
    
    // Create a pen for faster drawing
    HPEN hPen = CreatePen(PS_SOLID, 1, fillColor);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    
    // For each scanline
    for (int y = minY; y <= maxY; y++) {
        std::vector<int> intersections;
        
        // Find intersections with scanline
        for (size_t i = 0; i < polygon.size(); i++) {
            Point2D p1 = polygon[i];
            Point2D p2 = polygon[(i + 1) % polygon.size()];
            
            if ((p1.y <= y && p2.y > y) || (p2.y <= y && p1.y > y)) {
                // Calculate x coordinate of intersection
                int x = p1.x + (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
                intersections.push_back(x);
            }
        }
        
        // Sort intersections (bubble sort)
        for (size_t i = 0; i < intersections.size(); i++) {
            for (size_t j = i + 1; j < intersections.size(); j++) {
                if (intersections[i] > intersections[j]) {
                    int temp = intersections[i];
                    intersections[i] = intersections[j];
                    intersections[j] = temp;
                }
            }
        }
        
        // Fill pixels between intersections using lines for speed
        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            MoveToEx(hdc, intersections[i], y, NULL);
            LineTo(hdc, intersections[i + 1], y);
        }
    }
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void GraphicsEngine::DrawPolygon(const std::vector<Point2D>& points, COLORREF color)
{
    if (points.size() < 3)
        return;

    // Draw all edges of the polygon
    for (size_t i = 0; i < points.size(); i++)
    {
        Point2D p1 = points[i];
        Point2D p2 = points[(i + 1) % points.size()];  // Wrap around to close the polygon
        DrawLineBresenham(p1, p2, color);
    }
}

void GraphicsEngine::RenderAll()
{
    // Redraw all saved shapes
    for (const Shape& shape : shapes)
    {
        switch (shape.type)
        {
        case SHAPE_LINE:
            if (shape.points.size() >= 2)
            {
                DrawLineBresenham(shape.points[0], shape.points[1], shape.color);
            }
            break;

        case SHAPE_CIRCLE:
            if (shape.points.size() >= 1)
            {
                DrawCircleBresenham(shape.points[0], shape.radius, shape.color);
            }
            break;

        case SHAPE_RECTANGLE:
            if (shape.points.size() >= 2)
            {
                DrawRectangle(shape.points[0], shape.points[1], shape.color);
            }
            break;

        case SHAPE_POLYLINE:
            if (shape.points.size() >= 2)
            {
                DrawPolyline(shape.points, shape.color);
            }
            break;

        case SHAPE_POLYGON:
            if (shape.points.size() >= 3)
            {
                DrawPolygon(shape.points, shape.color);
            }
            break;

        case SHAPE_BSPLINE:
            if (shape.points.size() >= 4)
            {
                DrawBSpline(shape.points, shape.color);
            }
            break;
        }

        // Draw selection indicator if shape is selected
        if (shape.selected)
        {
            DrawSelectionIndicator(shape);
        }
    }
}

void GraphicsEngine::ClearCanvas()
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
}

// Selection functions implementation
int GraphicsEngine::SelectShapeAt(int x, int y)
{
    Point2D clickPoint(x, y);
    
    // Search from back to front (most recently drawn shapes first)
    for (int i = static_cast<int>(shapes.size()) - 1; i >= 0; i--)
    {
        const Shape& shape = shapes[i];
        
        switch (shape.type)
        {
        case SHAPE_LINE:
            if (shape.points.size() >= 2)
            {
                if (HitTestLine(clickPoint, shape.points[0], shape.points[1]))
                {
                    return i;
                }
            }
            break;

        case SHAPE_CIRCLE:
            if (shape.points.size() >= 1)
            {
                if (HitTestCircle(clickPoint, shape.points[0], shape.radius))
                {
                    return i;
                }
            }
            break;

        case SHAPE_RECTANGLE:
            if (shape.points.size() >= 2)
            {
                // Test all four edges of the rectangle
                Point2D p1 = shape.points[0];
                Point2D p2 = shape.points[1];
                
                if (HitTestLine(clickPoint, Point2D(p1.x, p1.y), Point2D(p2.x, p1.y)) ||
                    HitTestLine(clickPoint, Point2D(p2.x, p1.y), Point2D(p2.x, p2.y)) ||
                    HitTestLine(clickPoint, Point2D(p2.x, p2.y), Point2D(p1.x, p2.y)) ||
                    HitTestLine(clickPoint, Point2D(p1.x, p2.y), Point2D(p1.x, p1.y)))
                {
                    return i;
                }
            }
            break;

        case SHAPE_POLYLINE:
            if (shape.points.size() >= 2)
            {
                for (size_t j = 1; j < shape.points.size(); j++)
                {
                    if (HitTestLine(clickPoint, shape.points[j - 1], shape.points[j]))
                    {
                        return i;
                    }
                }
            }
            break;

        case SHAPE_POLYGON:
            if (shape.points.size() >= 3)
            {
                // First check if point is inside polygon
                if (HitTestPolygon(clickPoint, shape.points))
                {
                    return i;
                }
                
                // Also check edges
                for (size_t j = 0; j < shape.points.size(); j++)
                {
                    Point2D p1 = shape.points[j];
                    Point2D p2 = shape.points[(j + 1) % shape.points.size()];
                    if (HitTestLine(clickPoint, p1, p2))
                    {
                        return i;
                    }
                }
            }
            break;

        case SHAPE_BSPLINE:
            // For B-spline, check proximity to control points or curve
            if (shape.points.size() >= 4)
            {
                // Check control points
                for (const Point2D& pt : shape.points)
                {
                    int dx = clickPoint.x - pt.x;
                    int dy = clickPoint.y - pt.y;
                    if (dx * dx + dy * dy <= 25) // 5 pixel radius
                    {
                        return i;
                    }
                }
            }
            break;
        }
    }
    
    return -1; // No shape found
}

void GraphicsEngine::DeselectAll()
{
    for (Shape& shape : shapes)
    {
        shape.selected = false;
    }
    selectedShapeIndex = -1;
    hasSelection = false;
}

void GraphicsEngine::DrawSelectionIndicator(const Shape& shape)
{
    // Create a dashed pen for selection indicator
    HPEN hDashedPen = CreatePen(PS_DASH, 1, RGB(0, 0, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hDashedPen);
    
    // Calculate bounding box
    if (shape.points.empty())
    {
        SelectObject(hdc, hOldPen);
        DeleteObject(hDashedPen);
        return;
    }
    
    int minX = shape.points[0].x;
    int maxX = shape.points[0].x;
    int minY = shape.points[0].y;
    int maxY = shape.points[0].y;
    
    for (const Point2D& pt : shape.points)
    {
        if (pt.x < minX) minX = pt.x;
        if (pt.x > maxX) maxX = pt.x;
        if (pt.y < minY) minY = pt.y;
        if (pt.y > maxY) maxY = pt.y;
    }
    
    // For circles, adjust bounding box
    if (shape.type == SHAPE_CIRCLE)
    {
        minX -= shape.radius;
        maxX += shape.radius;
        minY -= shape.radius;
        maxY += shape.radius;
    }
    
    // Add padding
    int padding = 5;
    minX -= padding;
    minY -= padding;
    maxX += padding;
    maxY += padding;
    
    // Draw dashed bounding box
    MoveToEx(hdc, minX, minY, NULL);
    LineTo(hdc, maxX, minY);
    LineTo(hdc, maxX, maxY);
    LineTo(hdc, minX, maxY);
    LineTo(hdc, minX, minY);
    
    // Draw control points (small squares at corners)
    int cpSize = 3;
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    
    Rectangle(hdc, minX - cpSize, minY - cpSize, minX + cpSize, minY + cpSize);
    Rectangle(hdc, maxX - cpSize, minY - cpSize, maxX + cpSize, minY + cpSize);
    Rectangle(hdc, maxX - cpSize, maxY - cpSize, maxX + cpSize, maxY + cpSize);
    Rectangle(hdc, minX - cpSize, maxY - cpSize, minX + cpSize, maxY + cpSize);
    
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hDashedPen);
}

// Hit test helper functions
bool GraphicsEngine::HitTestLine(Point2D point, Point2D p1, Point2D p2, int tolerance)
{
    double distance = PointToLineDistance(point, p1, p2);
    return distance <= tolerance;
}

bool GraphicsEngine::HitTestCircle(Point2D point, Point2D center, int radius, int tolerance)
{
    int dx = point.x - center.x;
    int dy = point.y - center.y;
    double distance = sqrt(dx * dx + dy * dy);
    
    // Check if point is near the circle's circumference
    return abs(distance - radius) <= tolerance;
}

bool GraphicsEngine::HitTestPolygon(Point2D point, const std::vector<Point2D>& polygon)
{
    // Ray casting algorithm to check if point is inside polygon
    int n = static_cast<int>(polygon.size());
    bool inside = false;
    
    for (int i = 0, j = n - 1; i < n; j = i++)
    {
        int xi = polygon[i].x, yi = polygon[i].y;
        int xj = polygon[j].x, yj = polygon[j].y;
        
        bool intersect = ((yi > point.y) != (yj > point.y)) &&
                        (point.x < (xj - xi) * (point.y - yi) / (yj - yi) + xi);
        
        if (intersect)
        {
            inside = !inside;
        }
    }
    
    return inside;
}

double GraphicsEngine::PointToLineDistance(Point2D point, Point2D lineStart, Point2D lineEnd)
{
    // Calculate distance from point to line segment
    int dx = lineEnd.x - lineStart.x;
    int dy = lineEnd.y - lineStart.y;
    
    if (dx == 0 && dy == 0)
    {
        // Line segment is a point
        int px = point.x - lineStart.x;
        int py = point.y - lineStart.y;
        return sqrt(px * px + py * py);
    }
    
    // Calculate parameter t for projection onto line
    double t = ((point.x - lineStart.x) * dx + (point.y - lineStart.y) * dy) / 
               (double)(dx * dx + dy * dy);
    
    // Clamp t to [0, 1] to stay within line segment
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    
    // Calculate closest point on line segment
    double closestX = lineStart.x + t * dx;
    double closestY = lineStart.y + t * dy;
    
    // Calculate distance
    double distX = point.x - closestX;
    double distY = point.y - closestY;
    
    return sqrt(distX * distX + distY * distY);
}

// Transformation helper functions implementation

Point2D GraphicsEngine::CalculateShapeCenter(const Shape& shape)
{
    if (shape.points.empty())
    {
        return Point2D(0, 0);
    }
    
    // Calculate centroid
    int sumX = 0, sumY = 0;
    for (const Point2D& pt : shape.points)
    {
        sumX += pt.x;
        sumY += pt.y;
    }
    
    return Point2D(sumX / static_cast<int>(shape.points.size()), 
                   sumY / static_cast<int>(shape.points.size()));
}

void GraphicsEngine::ApplyTranslation(Shape& shape, int dx, int dy)
{
    // Translate all points
    for (Point2D& pt : shape.points)
    {
        pt.x += dx;
        pt.y += dy;
    }
}

void GraphicsEngine::ApplyScaling(Shape& shape, double scale, Point2D center)
{
    // Scale all points relative to center
    for (Point2D& pt : shape.points)
    {
        int dx = pt.x - center.x;
        int dy = pt.y - center.y;
        
        pt.x = center.x + static_cast<int>(dx * scale);
        pt.y = center.y + static_cast<int>(dy * scale);
    }
    
    // For circles, also scale the radius
    if (shape.type == SHAPE_CIRCLE)
    {
        shape.radius = static_cast<int>(shape.radius * scale);
    }
}

void GraphicsEngine::ApplyRotation(Shape& shape, double angle, Point2D center)
{
    // Rotate all points around center
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);
    
    for (Point2D& pt : shape.points)
    {
        // Translate to origin
        int dx = pt.x - center.x;
        int dy = pt.y - center.y;
        
        // Rotate
        int newX = static_cast<int>(dx * cosAngle - dy * sinAngle);
        int newY = static_cast<int>(dx * sinAngle + dy * cosAngle);
        
        // Translate back
        pt.x = center.x + newX;
        pt.y = center.y + newY;
    }
}

Shape GraphicsEngine::CreateTransformedPreview(const Shape& shape)
{
    // Create a copy of the shape for preview
    return shape;
}

// Clipping window drawing function
void GraphicsEngine::DrawClipWindow(Point2D p1, Point2D p2, bool isDashed)
{
    // Normalize coordinates to ensure p1 is top-left and p2 is bottom-right
    int xmin = (p1.x < p2.x) ? p1.x : p2.x;
    int ymin = (p1.y < p2.y) ? p1.y : p2.y;
    int xmax = (p1.x > p2.x) ? p1.x : p2.x;
    int ymax = (p1.y > p2.y) ? p1.y : p2.y;
    
    Point2D topLeft(xmin, ymin);
    Point2D topRight(xmax, ymin);
    Point2D bottomRight(xmax, ymax);
    Point2D bottomLeft(xmin, ymax);
    
    if (isDashed)
    {
        // Draw dashed rectangle for preview
        HPEN hPen = CreatePen(PS_DOT, 1, RGB(255, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        
        // Draw four edges
        MoveToEx(hdc, topLeft.x, topLeft.y, NULL);
        LineTo(hdc, topRight.x, topRight.y);
        LineTo(hdc, bottomRight.x, bottomRight.y);
        LineTo(hdc, bottomLeft.x, bottomLeft.y);
        LineTo(hdc, topLeft.x, topLeft.y);
        
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
    else
    {
        // Draw solid rectangle for final clipping window
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        
        // Draw four edges
        MoveToEx(hdc, topLeft.x, topLeft.y, NULL);
        LineTo(hdc, topRight.x, topRight.y);
        LineTo(hdc, bottomRight.x, bottomRight.y);
        LineTo(hdc, bottomLeft.x, bottomLeft.y);
        LineTo(hdc, topLeft.x, topLeft.y);
        
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
}
