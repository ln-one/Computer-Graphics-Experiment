/**
 * @file GraphicsEngine.cpp
 * @brief 二维图形引擎实现
 * @author ln1.opensource@gmail.com
 * 
 * 本文件实现了二维图形引擎的核心功能，包括：
 * 1. 图形绑定和渲染管理
 * 2. 鼠标交互事件处理
 * 3. 各种绘图模式的状态机
 * 4. 几何变换（平移、缩放、旋转）
 * 5. 图形裁剪算法的调用
 */

#include "GraphicsEngine.h"
#include "ShapeRenderer.h"
#include "ShapeSelector.h"
#include "../algorithms/LineDrawer.h"
#include "../algorithms/CircleDrawer.h"
#include "../algorithms/FillAlgorithms.h"
#include "../algorithms/TransformAlgorithms.h"
#include "../algorithms/ClippingAlgorithms.h"
#include <cmath>

// ============================================================================
// 构造函数和初始化
// ============================================================================

/**
 * @brief 构造函数，初始化所有成员变量
 */
GraphicsEngine::GraphicsEngine() 
    : hdc(nullptr), hwnd(nullptr), currentMode(MODE_NONE), isDrawing(false),
      selectedShapeIndex(-1), hasSelection(false), isTransforming(false),
      initialDistance(0.0), initialAngle(0.0), isDefiningClipWindow(false), 
      hasClipWindow(false) {}

/**
 * @brief 析构函数
 */
GraphicsEngine::~GraphicsEngine() {}

/**
 * @brief 初始化图形引擎
 * @param hwnd 窗口句柄
 * @param hdc 设备上下文句柄
 */
void GraphicsEngine::Initialize(HWND hwnd, HDC hdc) {
    this->hwnd = hwnd;
    this->hdc = hdc;
}

/**
 * @brief 设置当前绘图模式
 * @param mode 新的绘图模式
 * 
 * 切换模式时会重置绘图状态和临时点集合
 */
void GraphicsEngine::SetMode(DrawMode mode) {
    currentMode = mode;
    isDrawing = false;
    tempPoints.clear();
}

/**
 * @brief 清空画布
 * 
 * 清除所有已绘制的图形，重置选择状态
 */
void GraphicsEngine::ClearCanvas() {
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
    shapes.clear();
    hasSelection = false;
    selectedShapeIndex = -1;
}

/**
 * @brief 渲染所有图形
 * 
 * 遍历图形集合，绘制每个图形
 * 选中的图形用红色显示，并绘制选择指示器
 */
void GraphicsEngine::RenderAll() {
    for (const auto& shape : shapes) {
        // 选中的图形用黄色显示
        COLORREF color = shape.selected ? RGB(255, 0, 0) : shape.color;
        ShapeRenderer::DrawShape(hdc, shape, color);
        // 为选中的图形绘制选择指示器
        if (shape.selected) {
            ShapeSelector::DrawSelectionIndicator(hdc, shape);
        }
    }
}

// ============================================================================
// 鼠标事件处理
// ============================================================================

/**
 * @brief 处理鼠标左键按下事件
 * @param x 鼠标x坐标
 * @param y 鼠标y坐标
 * 
 * 根据当前绘图模式分发到相应的处理函数
 */
void GraphicsEngine::OnLButtonDown(int x, int y) {
    Point2D clickPoint(x, y);
    
    switch (currentMode) {
        // 直线绘制模式
        case MODE_LINE_DDA:
        case MODE_LINE_BRESENHAM:
            HandleLineDrawing(clickPoint);
            break;
        // 圆形绘制模式
        case MODE_CIRCLE_MIDPOINT:
        case MODE_CIRCLE_BRESENHAM:
            HandleCircleDrawing(clickPoint);
            break;
        // 矩形绘制模式
        case MODE_RECTANGLE:
            HandleRectangleDrawing(clickPoint);
            break;
        // 多边形/折线绘制模式
        case MODE_POLYLINE:
        case MODE_POLYGON:
            HandlePolyDrawing(clickPoint);
            break;
        // 边界填充模式
        case MODE_FILL_BOUNDARY:
            FillAlgorithms::BoundaryFill(hdc, hwnd, x, y, RGB(255, 0, 0), RGB(0, 0, 0));
            break;
        // 扫描线填充模式
        case MODE_FILL_SCANLINE:
            HandleScanlineFillDrawing(clickPoint);
            break;
        // 图形选择模式
        case MODE_SELECT:
            HandleSelection(clickPoint);
            break;
        // 平移变换模式
        case MODE_TRANSLATE:
            HandleTranslation(clickPoint);
            break;
        // 缩放变换模式
        case MODE_SCALE:
            HandleScaling(clickPoint);
            break;
        // 旋转变换模式
        case MODE_ROTATE:
            HandleRotation(clickPoint);
            break;
        // 裁剪模式
        case MODE_CLIP_COHEN_SUTHERLAND:
        case MODE_CLIP_MIDPOINT:
        case MODE_CLIP_SUTHERLAND_HODGMAN:
        case MODE_CLIP_WEILER_ATHERTON:
            HandleClippingWindow(clickPoint);
            break;
    }
}

/**
 * @brief 处理鼠标移动事件
 * @param x 鼠标x坐标
 * @param y 鼠标y坐标
 * 
 * 主要用于旋转操作的实时预览
 */
void GraphicsEngine::OnMouseMove(int x, int y) {
    Point2D currentPoint(x, y);
    
    // 处理旋转预览
    if (currentMode == MODE_ROTATE && isTransforming && hasSelection) {
        // 重绘所有图形
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
        RenderAll();
        
        // 绘制旋转中心标记（十字形）
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        int markerSize = 5;
        MoveToEx(hdc, transformAnchorPoint.x - markerSize, transformAnchorPoint.y, NULL);
        LineTo(hdc, transformAnchorPoint.x + markerSize, transformAnchorPoint.y);
        MoveToEx(hdc, transformAnchorPoint.x, transformAnchorPoint.y - markerSize, NULL);
        LineTo(hdc, transformAnchorPoint.x, transformAnchorPoint.y + markerSize);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
        
        // 计算旋转角度
        int dx = currentPoint.x - transformAnchorPoint.x;
        int dy = currentPoint.y - transformAnchorPoint.y;
        double currentAngle = atan2(dy, dx);
        
        // 首次移动时记录初始角度
        static bool firstMove = true;
        static Point2D lastAnchor;
        if (firstMove || lastAnchor.x != transformAnchorPoint.x || lastAnchor.y != transformAnchorPoint.y) {
            initialAngle = currentAngle;
            firstMove = false;
            lastAnchor = transformAnchorPoint;
        }
        
        double rotationAngle = currentAngle - initialAngle;
        
        // 创建并绘制旋转预览（使用浅蓝色）
        Shape preview = shapes[selectedShapeIndex];
        TransformAlgorithms::ApplyRotation(preview, rotationAngle, transformAnchorPoint);
        ShapeRenderer::DrawShape(hdc, preview, RGB(128, 128, 255));
        
        // 绘制从中心到鼠标的指示线
        DrawLineBresenham(transformAnchorPoint, currentPoint, RGB(255, 0, 0));
    }
}

/**
 * @brief 处理鼠标右键按下事件
 * @param x 鼠标x坐标
 * @param y 鼠标y坐标
 * 
 * 右键用于结束多点绘图操作（折线、多边形、扫描线填充）
 * 以及确认旋转操作
 */
void GraphicsEngine::OnRButtonDown(int x, int y) {
    // 折线模式：右键结束绘制
    if (currentMode == MODE_POLYLINE && tempPoints.size() >= 2) {
        Shape polyline;
        polyline.type = SHAPE_POLYLINE;
        polyline.points = tempPoints;
        polyline.color = RGB(0, 0, 0);
        polyline.selected = false;
        shapes.push_back(polyline);
        tempPoints.clear();
        isDrawing = false;
    }
    // 多边形模式：右键结束绘制并闭合
    else if (currentMode == MODE_POLYGON && tempPoints.size() >= 3) {
        DrawPolygon(tempPoints);
        Shape polygon;
        polygon.type = SHAPE_POLYGON;
        polygon.points = tempPoints;
        polygon.color = RGB(0, 0, 0);
        polygon.selected = false;
        shapes.push_back(polygon);
        tempPoints.clear();
        isDrawing = false;
    }
    // 扫描线填充模式：右键结束并执行填充
    else if (currentMode == MODE_FILL_SCANLINE && tempPoints.size() >= 3) {
        // 闭合多边形
        DrawLineBresenham(tempPoints.back(), tempPoints.front());
        // 执行扫描线填充
        FillAlgorithms::ScanlineFill(hdc, tempPoints, RGB(255, 0, 0));
        tempPoints.clear();
        isDrawing = false;
    }
    // 旋转模式：右键确认旋转
    else if (currentMode == MODE_ROTATE && isTransforming && hasSelection) {
        Point2D currentPoint(x, y);
        int dx = currentPoint.x - transformAnchorPoint.x;
        int dy = currentPoint.y - transformAnchorPoint.y;
        double angle = atan2(dy, dx);
        
        // 应用旋转变换
        TransformAlgorithms::ApplyRotation(shapes[selectedShapeIndex], angle - initialAngle, transformAnchorPoint);
        
        isTransforming = false;
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

/**
 * @brief 绘制实验一的示例图形
 * 
 * 绘制一个带圆角的矩形框架，包含内部矩形和四个角的圆孔
 * 用于演示基本图形绑定功能
 */
void GraphicsEngine::DrawExpr1Graphics() {
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    
    // 设置绘图参数
    int offsetX = 100, offsetY = 100, scale = 5;
    
    // 绘制外部圆角矩形
    RoundRect(hdc, offsetX, offsetY, offsetX + 66 * scale, offsetY + 46 * scale, 7 * scale, 7 * scale);
    
    // 绘制内部圆角矩形
    RoundRect(hdc, offsetX + (66 - 43) / 2 * scale, offsetY + (46 - 30) / 2 * scale,
              offsetX + (66 + 43) / 2 * scale, offsetY + (46 + 30) / 2 * scale, 3 * scale, 3 * scale);
    
    // 计算四个角圆孔的参数
    int holeR = static_cast<int>(7.0 / 2.0 * scale);
    int holeCenterOffsetX = (66 - 52) / 2 * scale;
    int holeCenterOffsetY = (46 - 32) / 2 * scale;
    
    // 四个圆孔的中心坐标
    int centers[4][2] = {
        {offsetX + holeCenterOffsetX, offsetY + holeCenterOffsetY},                           // 左上
        {offsetX + 66 * scale - holeCenterOffsetX, offsetY + holeCenterOffsetY},              // 右上
        {offsetX + holeCenterOffsetX, offsetY + 46 * scale - holeCenterOffsetY},              // 左下
        {offsetX + 66 * scale - holeCenterOffsetX, offsetY + 46 * scale - holeCenterOffsetY}  // 右下
    };
    
    // 绘制四个圆孔
    for (int i = 0; i < 4; i++) {
        Ellipse(hdc, centers[i][0] - holeR, centers[i][1] - holeR,
                centers[i][0] + holeR, centers[i][1] + holeR);
    }
    
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

// ============================================================================
// 私有辅助方法 - 绑定模式处理
// ============================================================================

/**
 * @brief 处理直线绘制模式的鼠标点击
 * @param clickPoint 点击位置
 * 
 * 第一次点击记录起点，第二次点击记录终点并绘制直线
 */
void GraphicsEngine::HandleLineDrawing(Point2D clickPoint) {
    if (!isDrawing) {
        // 第一次点击：记录起点
        tempPoints.clear();
        tempPoints.push_back(clickPoint);
        isDrawing = true;
    } else {
        // 第二次点击：记录终点并绘制
        tempPoints.push_back(clickPoint);
        if (currentMode == MODE_LINE_DDA)
            DrawLineDDA(tempPoints[0], tempPoints[1]);
        else
            DrawLineBresenham(tempPoints[0], tempPoints[1]);
        
        // 保存图形到集合
        Shape line;
        line.type = SHAPE_LINE;
        line.points = tempPoints;
        line.color = RGB(0, 0, 0);
        line.selected = false;
        shapes.push_back(line);
        isDrawing = false;
    }
}

/**
 * @brief 处理圆形绘制模式的鼠标点击
 * @param clickPoint 点击位置
 * 
 * 第一次点击记录圆心，第二次点击确定半径并绘制圆
 */
void GraphicsEngine::HandleCircleDrawing(Point2D clickPoint) {
    if (!isDrawing) {
        // 第一次点击：记录圆心
        tempPoints.clear();
        tempPoints.push_back(clickPoint);
        isDrawing = true;
    } else {
        // 第二次点击：计算半径并绘制
        tempPoints.push_back(clickPoint);
        int radius = (int)sqrt(pow(tempPoints[1].x - tempPoints[0].x, 2) +
                             pow(tempPoints[1].y - tempPoints[0].y, 2));
        if (currentMode == MODE_CIRCLE_MIDPOINT)
            DrawCircleMidpoint(tempPoints[0], radius);
        else
            DrawCircleBresenham(tempPoints[0], radius);
        
        // 保存图形到集合
        Shape circle;
        circle.type = SHAPE_CIRCLE;
        circle.points.push_back(tempPoints[0]);
        circle.radius = radius;
        circle.color = RGB(0, 0, 0);
        circle.selected = false;
        shapes.push_back(circle);
        isDrawing = false;
    }
}

/**
 * @brief 处理矩形绘制模式的鼠标点击
 * @param clickPoint 点击位置
 * 
 * 第一次点击记录一个角点，第二次点击记录对角点并绘制矩形
 */
void GraphicsEngine::HandleRectangleDrawing(Point2D clickPoint) {
    if (!isDrawing) {
        // 第一次点击：记录第一个角点
        tempPoints.clear();
        tempPoints.push_back(clickPoint);
        isDrawing = true;
    } else {
        // 第二次点击：记录对角点并绘制
        tempPoints.push_back(clickPoint);
        DrawRectangle(tempPoints[0], tempPoints[1]);
        
        // 保存图形到集合
        Shape rectangle;
        rectangle.type = SHAPE_RECTANGLE;
        rectangle.points = tempPoints;
        rectangle.color = RGB(0, 0, 0);
        rectangle.selected = false;
        shapes.push_back(rectangle);
        isDrawing = false;
    }
}

/**
 * @brief 处理多边形/折线绘制模式的鼠标点击
 * @param clickPoint 点击位置
 * 
 * 每次点击添加一个顶点，并绘制到前一个顶点的连线
 * 右键结束绘制
 */
void GraphicsEngine::HandlePolyDrawing(Point2D clickPoint) {
    tempPoints.push_back(clickPoint);
    if (!isDrawing) isDrawing = true;
    // 绘制到前一个顶点的连线
    if (tempPoints.size() >= 2) {
        DrawLineBresenham(tempPoints[tempPoints.size()-2], tempPoints.back());
    }
}

/**
 * @brief 处理扫描线填充模式的鼠标点击
 * @param clickPoint 点击位置
 * 
 * 每次点击添加一个多边形顶点
 * 右键结束并执行扫描线填充
 */
void GraphicsEngine::HandleScanlineFillDrawing(Point2D clickPoint) {
    tempPoints.push_back(clickPoint);
    if (!isDrawing) isDrawing = true;
    // 绘制多边形边界
    if (tempPoints.size() >= 2) {
        DrawLineBresenham(tempPoints[tempPoints.size()-2], tempPoints.back());
    }
}

// ============================================================================
// 私有辅助方法 - 图形选择和变换
// ============================================================================

/**
 * @brief 处理图形选择模式的鼠标点击
 * @param clickPoint 点击位置
 * 
 * 点击图形选中它，点击空白处取消选择
 */
void GraphicsEngine::HandleSelection(Point2D clickPoint) {
    int hitIndex = ShapeSelector::SelectShapeAt(clickPoint, shapes);
    if (hitIndex >= 0) {
        // 取消所有选择，选中点击的图形
        for (auto& shape : shapes) shape.selected = false;
        shapes[hitIndex].selected = true;
        selectedShapeIndex = hitIndex;
        hasSelection = true;
    } else if (hasSelection) {
        // 点击空白处，取消选择
        for (auto& shape : shapes) shape.selected = false;
        hasSelection = false;
        selectedShapeIndex = -1;
    }
    InvalidateRect(hwnd, NULL, TRUE);
}

/**
 * @brief 处理平移变换模式的鼠标点击
 * @param clickPoint 点击位置
 * 
 * 第一次点击记录起始位置，第二次点击计算位移并应用平移
 */
void GraphicsEngine::HandleTranslation(Point2D clickPoint) {
    if (!hasSelection) {
        MessageBoxW(hwnd, L"请先选择一个图形", L"平移", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (!isTransforming) {
        // 第一次点击：记录起始位置
        transformStartPoint = clickPoint;
        isTransforming = true;
    } else {
        // 第二次点击：计算位移并应用平移
        int dx = clickPoint.x - transformStartPoint.x;
        int dy = clickPoint.y - transformStartPoint.y;
        TransformAlgorithms::ApplyTranslation(shapes[selectedShapeIndex], dx, dy);
        isTransforming = false;
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

/**
 * @brief 处理缩放变换模式的鼠标点击
 * @param clickPoint 点击位置
 * 
 * 第一次点击记录初始距离，第二次点击根据距离变化计算缩放比例
 */
void GraphicsEngine::HandleScaling(Point2D clickPoint) {
    if (!hasSelection) {
        MessageBoxW(hwnd, L"请先选择一个图形", L"缩放", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (!isTransforming) {
        // 第一次点击：计算图形中心和初始距离
        transformAnchorPoint = TransformAlgorithms::CalculateShapeCenter(shapes[selectedShapeIndex]);
        transformStartPoint = clickPoint;
        int dx = clickPoint.x - transformAnchorPoint.x;
        int dy = clickPoint.y - transformAnchorPoint.y;
        initialDistance = sqrt(dx * dx + dy * dy);
        if (initialDistance < 1.0) initialDistance = 1.0;  // 防止除零
        isTransforming = true;
    } else {
        // 第二次点击：计算缩放比例并应用
        int dx = clickPoint.x - transformAnchorPoint.x;
        int dy = clickPoint.y - transformAnchorPoint.y;
        double currentDistance = sqrt(dx * dx + dy * dy);
        double scale = currentDistance / initialDistance;
        TransformAlgorithms::ApplyScaling(shapes[selectedShapeIndex], scale, transformAnchorPoint);
        isTransforming = false;
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

/**
 * @brief 处理旋转变换模式的鼠标点击
 * @param clickPoint 点击位置
 * 
 * 第一次点击设置旋转中心，然后通过鼠标移动预览旋转效果
 * 右键确认旋转
 */
void GraphicsEngine::HandleRotation(Point2D clickPoint) {
    if (!hasSelection) {
        MessageBoxW(hwnd, L"请先选择一个图形", L"旋转", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (!isTransforming) {
        // 第一次点击：设置旋转中心
        transformAnchorPoint = clickPoint;
        isTransforming = true;
        
        // 重绘并显示旋转中心标记
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
        RenderAll();
        
        // 绘制十字形旋转中心标记
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
}

// ============================================================================
// 私有辅助方法 - 裁剪操作
// ============================================================================

/**
 * @brief 处理裁剪窗口定义模式的鼠标点击
 * @param clickPoint 点击位置
 * 
 * 第一次点击记录裁剪窗口起点，第二次点击记录终点并执行裁剪
 */
void GraphicsEngine::HandleClippingWindow(Point2D clickPoint) {
    if (!isDefiningClipWindow) {
        // 第一次点击：记录裁剪窗口起点
        clipWindowStart = clickPoint;
        isDefiningClipWindow = true;
    } else {
        // 第二次点击：记录终点并执行裁剪
        clipWindowEnd = clickPoint;
        isDefiningClipWindow = false;
        hasClipWindow = true;
        
        // 重绘并显示裁剪窗口
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
        RenderAll();
        DrawClipWindow(clipWindowStart, clipWindowEnd);
        
        // 根据当前模式执行相应的裁剪算法
        if (currentMode == MODE_CLIP_COHEN_SUTHERLAND)
            ExecuteCohenSutherlandClipping();
        else if (currentMode == MODE_CLIP_MIDPOINT)
            ExecuteMidpointClipping();
        else if (currentMode == MODE_CLIP_SUTHERLAND_HODGMAN)
            ExecuteSutherlandHodgmanClipping();
        else if (currentMode == MODE_CLIP_WEILER_ATHERTON)
            ExecuteWeilerAthertonClipping();
    }
}

/**
 * @brief 绘制裁剪窗口
 * @param p1 裁剪窗口的一个角点
 * @param p2 裁剪窗口的对角点
 * 
 * 用红色线条绘制裁剪窗口的边界
 */
void GraphicsEngine::DrawClipWindow(Point2D p1, Point2D p2) {
    // 计算裁剪窗口的边界
    int xmin = (p1.x < p2.x) ? p1.x : p2.x;
    int ymin = (p1.y < p2.y) ? p1.y : p2.y;
    int xmax = (p1.x > p2.x) ? p1.x : p2.x;
    int ymax = (p1.y > p2.y) ? p1.y : p2.y;

    // 用红色线条绘制裁剪窗口
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, xmin, ymin, NULL);
    LineTo(hdc, xmax, ymin);
    LineTo(hdc, xmax, ymax);
    LineTo(hdc, xmin, ymax);
    LineTo(hdc, xmin, ymin);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

// ============================================================================
// 裁剪算法执行
// ============================================================================

/**
 * @brief 执行Cohen-Sutherland直线裁剪算法
 * 
 * 对所有直线图形应用Cohen-Sutherland裁剪算法
 * 裁剪后的直线替换原直线，完全在窗口外的直线被删除
 */
void GraphicsEngine::ExecuteCohenSutherlandClipping() {
    // 计算裁剪窗口边界
    int xmin = (clipWindowStart.x < clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymin = (clipWindowStart.y < clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;
    int xmax = (clipWindowStart.x > clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymax = (clipWindowStart.y > clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;

    std::vector<Shape> clippedShapes;
    for (Shape& shape : shapes) {
        if (shape.type == SHAPE_LINE && shape.points.size() >= 2) {
            // 对直线应用Cohen-Sutherland裁剪
            Point2D p1 = shape.points[0], p2 = shape.points[1];
            if (ClippingAlgorithms::ClipLineCohenSutherland(p1, p2, xmin, ymin, xmax, ymax)) {
                Shape clippedLine = shape;
                clippedLine.points[0] = p1;
                clippedLine.points[1] = p2;
                clippedShapes.push_back(clippedLine);
            }
            // 如果返回false，直线完全在窗口外，不添加到结果中
        } else {
            // 非直线图形保持不变
            clippedShapes.push_back(shape);
        }
    }
    shapes = clippedShapes;
    hasClipWindow = false;
    InvalidateRect(hwnd, NULL, TRUE);
    MessageBoxW(hwnd, L"Cohen-Sutherland裁剪完成！", L"完成", MB_OK | MB_ICONINFORMATION);
}

/**
 * @brief 执行中点分割直线裁剪算法
 * 
 * 对所有直线图形应用中点分割裁剪算法
 * 一条直线可能被裁剪成多个线段
 */
void GraphicsEngine::ExecuteMidpointClipping() {
    // 计算裁剪窗口边界
    int xmin = (clipWindowStart.x < clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymin = (clipWindowStart.y < clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;
    int xmax = (clipWindowStart.x > clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymax = (clipWindowStart.y > clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;

    std::vector<Shape> clippedShapes;
    for (Shape& shape : shapes) {
        if (shape.type == SHAPE_LINE && shape.points.size() >= 2) {
            // 对直线应用中点分割裁剪
            std::vector<std::pair<Point2D, Point2D>> segments;
            ClippingAlgorithms::ClipLineMidpoint(shape.points[0], shape.points[1], xmin, ymin, xmax, ymax, segments);
            // 每个裁剪后的线段作为独立的直线
            for (const auto& seg : segments) {
                Shape clippedLine = shape;
                clippedLine.points.clear();
                clippedLine.points.push_back(seg.first);
                clippedLine.points.push_back(seg.second);
                clippedShapes.push_back(clippedLine);
            }
        } else {
            // 非直线图形保持不变
            clippedShapes.push_back(shape);
        }
    }
    shapes = clippedShapes;
    hasClipWindow = false;
    InvalidateRect(hwnd, NULL, TRUE);
    MessageBoxW(hwnd, L"中点分割裁剪完成！", L"完成", MB_OK | MB_ICONINFORMATION);
}

/**
 * @brief 执行Sutherland-Hodgman多边形裁剪算法
 * 
 * 对所有多边形图形应用Sutherland-Hodgman裁剪算法
 * 适用于凸多边形裁剪
 */
void GraphicsEngine::ExecuteSutherlandHodgmanClipping() {
    // 计算裁剪窗口边界
    int xmin = (clipWindowStart.x < clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymin = (clipWindowStart.y < clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;
    int xmax = (clipWindowStart.x > clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymax = (clipWindowStart.y > clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;

    std::vector<Shape> clippedShapes;
    for (Shape& shape : shapes) {
        if (shape.type == SHAPE_POLYGON && shape.points.size() >= 3) {
            // 对多边形应用Sutherland-Hodgman裁剪
            std::vector<Point2D> clipped = ClippingAlgorithms::ClipPolygonSutherlandHodgman(
                shape.points, xmin, ymin, xmax, ymax);
            // 只有裁剪后仍有至少3个顶点才保留
            if (clipped.size() >= 3) {
                Shape clippedShape = shape;
                clippedShape.points = clipped;
                clippedShapes.push_back(clippedShape);
            }
        } else {
            // 非多边形图形保持不变
            clippedShapes.push_back(shape);
        }
    }
    shapes = clippedShapes;
    hasClipWindow = false;
    InvalidateRect(hwnd, NULL, TRUE);
    MessageBoxW(hwnd, L"Sutherland-Hodgman裁剪完成！", L"完成", MB_OK | MB_ICONINFORMATION);
}

// ============================================================================
// 基础绑定方法包装
// ============================================================================
/**
 * @brief 使用DDA算法绘制直线
 * @param p1 起点
 * @param p2 终点
 * @param color 线条颜色
 */
void GraphicsEngine::DrawLineDDA(Point2D p1, Point2D p2, COLORREF color) {
    LineDrawer::DrawDDA(hdc, p1, p2, color);
}

/**
 * @brief 使用Bresenham算法绘制直线
 * @param p1 起点
 * @param p2 终点
 * @param color 线条颜色
 */
void GraphicsEngine::DrawLineBresenham(Point2D p1, Point2D p2, COLORREF color) {
    LineDrawer::DrawBresenham(hdc, p1, p2, color);
}

/**
 * @brief 使用中点算法绘制圆形
 * @param center 圆心
 * @param radius 半径
 * @param color 线条颜色
 */
void GraphicsEngine::DrawCircleMidpoint(Point2D center, int radius, COLORREF color) {
    CircleDrawer::DrawMidpoint(hdc, center, radius, color);
}

/**
 * @brief 使用Bresenham算法绘制圆形
 * @param center 圆心
 * @param radius 半径
 * @param color 线条颜色
 */
void GraphicsEngine::DrawCircleBresenham(Point2D center, int radius, COLORREF color) {
    CircleDrawer::DrawBresenham(hdc, center, radius, color);
}

/**
 * @brief 绘制矩形
 * @param p1 矩形的一个角点
 * @param p2 矩形的对角点
 * @param color 线条颜色
 * 
 * 使用四条直线绘制矩形边框
 */
void GraphicsEngine::DrawRectangle(Point2D p1, Point2D p2, COLORREF color) {
    // 绘制四条边
    LineDrawer::DrawBresenham(hdc, Point2D(p1.x, p1.y), Point2D(p2.x, p1.y), color);  // 上边
    LineDrawer::DrawBresenham(hdc, Point2D(p2.x, p1.y), Point2D(p2.x, p2.y), color);  // 右边
    LineDrawer::DrawBresenham(hdc, Point2D(p2.x, p2.y), Point2D(p1.x, p2.y), color);  // 下边
    LineDrawer::DrawBresenham(hdc, Point2D(p1.x, p2.y), Point2D(p1.x, p1.y), color);  // 左边
}

/**
 * @brief 绘制折线
 * @param points 折线顶点序列
 * @param color 线条颜色
 */
void GraphicsEngine::DrawPolyline(const std::vector<Point2D>& points, COLORREF color) {
    for (size_t i = 1; i < points.size(); i++) {
        LineDrawer::DrawBresenham(hdc, points[i-1], points[i], color);
    }
}

/**
 * @brief 绘制多边形
 * @param points 多边形顶点序列
 * @param color 线条颜色
 * 
 * 自动闭合多边形（连接最后一个顶点和第一个顶点）
 */
void GraphicsEngine::DrawPolygon(const std::vector<Point2D>& points, COLORREF color) {
    if (points.size() < 3) return;
    for (size_t i = 0; i < points.size(); i++) {
        Point2D p1 = points[i];
        Point2D p2 = points[(i + 1) % points.size()];  // 自动闭合
        LineDrawer::DrawBresenham(hdc, p1, p2, color);
    }
}

/**
 * @brief 执行Weiler-Atherton多边形裁剪算法
 * 
 * 对所有多边形图形应用Weiler-Atherton裁剪算法
 * 支持凹多边形，可能产生多个裁剪结果
 */
void GraphicsEngine::ExecuteWeilerAthertonClipping() {
    if (!hasClipWindow) {
        MessageBoxW(hwnd, L"请先定义裁剪窗口", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
    
    // 计算裁剪窗口边界
    int xmin = (clipWindowStart.x < clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymin = (clipWindowStart.y < clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;
    int xmax = (clipWindowStart.x > clipWindowEnd.x) ? clipWindowStart.x : clipWindowEnd.x;
    int ymax = (clipWindowStart.y > clipWindowEnd.y) ? clipWindowStart.y : clipWindowEnd.y;

    std::vector<Shape> clippedShapes;
    
    for (Shape& shape : shapes) {
        if (shape.type == SHAPE_POLYGON && shape.points.size() >= 3) {
            // 检查是否完全在窗口内
            bool allInside = true;
            for (const Point2D& pt : shape.points) {
                if (pt.x < xmin || pt.x > xmax || pt.y < ymin || pt.y > ymax) {
                    allInside = false;
                    break;
                }
            }
            
            if (allInside) {
                // 完全在窗口内，保持不变
                clippedShapes.push_back(shape);
                continue;
            }
            
            // 检查是否完全在窗口外
            bool allOutside = true;
            for (const Point2D& pt : shape.points) {
                if (pt.x >= xmin && pt.x <= xmax && pt.y >= ymin && pt.y <= ymax) {
                    allOutside = false;
                    break;
                }
            }
            
            if (allOutside) {
                // 完全在窗口外，跳过
                continue;
            }
            
            // 多边形与窗口相交，应用Weiler-Atherton算法
            std::vector<std::vector<Point2D>> clippedPolygons = 
                ClippingAlgorithms::ClipPolygonWeilerAtherton(shape.points, xmin, ymin, xmax, ymax);
            
            if (!clippedPolygons.empty()) {
                // 添加所有裁剪后的多边形
                for (const auto& poly : clippedPolygons) {
                    if (poly.size() >= 3) {
                        Shape clippedShape = shape;
                        clippedShape.points = poly;
                        clippedShapes.push_back(clippedShape);
                    }
                }
            } else {
                // 备用方案：如果算法失败，保留大部分在窗口内的多边形
                int insideCount = 0;
                for (const Point2D& pt : shape.points) {
                    if (pt.x >= xmin && pt.x <= xmax && pt.y >= ymin && pt.y <= ymax) {
                        insideCount++;
                    }
                }
                if (insideCount > (int)shape.points.size() / 2) {
                    clippedShapes.push_back(shape);
                }
            }
        } else {
            // 非多边形图形保持不变
            clippedShapes.push_back(shape);
        }
    }
    
    shapes = clippedShapes;
    hasClipWindow = false;
    InvalidateRect(hwnd, NULL, TRUE);
    MessageBoxW(hwnd, L"Weiler-Atherton裁剪完成！", L"完成", MB_OK | MB_ICONINFORMATION);
}
