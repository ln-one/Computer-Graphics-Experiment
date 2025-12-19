/**
 * @file ClippingAlgorithms.cpp
 * @brief 图形裁剪算法实现
 * @author ln1.opensource@gmail.com
 * 
 * 本文件实现了四种经典的图形裁剪算法：
 * 
 * 1. Cohen-Sutherland直线裁剪算法
 *    - 使用区域编码快速判断直线与窗口的位置关系
 *    - 时间复杂度：O(1)到O(n)，取决于裁剪次数
 *    - 适用场景：大量直线需要裁剪时效率较高
 * 
 * 2. 中点分割直线裁剪算法
 *    - 使用二分法递归查找直线与窗口的交点
 *    - 时间复杂度：O(log n)
 *    - 适用场景：硬件实现简单，适合并行处理
 * 
 * 3. Sutherland-Hodgman多边形裁剪算法
 *    - 逐边裁剪，依次对左、右、下、上四条边进行裁剪
 *    - 时间复杂度：O(n)，n为多边形顶点数
 *    - 适用场景：凸多边形裁剪，实现简单
 * 
 * 4. Weiler-Atherton多边形裁剪算法
 *    - 支持凹多边形裁剪，可能产生多个裁剪结果
 *    - 时间复杂度：O(n*m)，n和m分别为两个多边形的顶点数
 *    - 适用场景：复杂多边形裁剪，需要精确结果
 */

#include "ClippingAlgorithms.h"
#include <cmath>
#include <set>

/**
 * @brief 计算点的区域编码（Cohen-Sutherland算法）
 * @param point 待编码的点
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @return 点的区域编码
 * 
 * 根据点相对于裁剪窗口的位置，生成4位二进制编码：
 * bit 0: LEFT (点在左侧)
 * bit 1: RIGHT (点在右侧)
 * bit 2: BOTTOM (点在下方)
 * bit 3: TOP (点在上方)
 */
int ClippingAlgorithms::ComputeOutCode(Point2D point, int xmin, int ymin, int xmax, int ymax) {
    int code = INSIDE;  // 初始化为内部区域
    
    // 检查水平位置
    if (point.x < xmin) 
        code |= LEFT;       // 点在左侧
    else if (point.x > xmax) 
        code |= RIGHT;      // 点在右侧
    
    // 检查垂直位置
    if (point.y < ymin) 
        code |= TOP;        // 点在上方
    else if (point.y > ymax) 
        code |= BOTTOM;     // 点在下方
    
    return code;
}

/**
 * @brief Cohen-Sutherland直线裁剪算法
 * @param p1 直线起点（引用，可能被修改）
 * @param p2 直线终点（引用，可能被修改）
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @return 如果直线与窗口有交集返回true，否则返回false
 * 
 * 算法步骤：
 * 1. 计算两端点的区域编码
 * 2. 如果两点都在窗口内，直接接受
 * 3. 如果两点在窗口同一侧外，直接拒绝
 * 4. 否则计算与窗口边界的交点，更新端点并重复
 */
bool ClippingAlgorithms::ClipLineCohenSutherland(Point2D& p1, Point2D& p2, int xmin, int ymin, int xmax, int ymax) {
    int outcode1 = ComputeOutCode(p1, xmin, ymin, xmax, ymax);  // 起点编码
    int outcode2 = ComputeOutCode(p2, xmin, ymin, xmax, ymax);  // 终点编码
    bool accept = false;

    while (true) {
        // 情况1：两点都在窗口内（编码都为0）
        if ((outcode1 | outcode2) == 0) {
            accept = true;  // 完全接受
            break;
        } 
        // 情况2：两点在窗口同一侧外（编码按位与不为0）
        else if ((outcode1 & outcode2) != 0) {
            break;  // 完全拒绝
        } 
        // 情况3：需要裁剪
        else {
            // 选择窗口外的点进行裁剪
            int outcodeOut = outcode1 ? outcode1 : outcode2;
            Point2D intersection;

            // 计算与窗口边界的交点
            if (outcodeOut & TOP) {
                // 与上边界相交
                intersection.x = p1.x + (p2.x - p1.x) * (ymin - p1.y) / (p2.y - p1.y);
                intersection.y = ymin;
            } else if (outcodeOut & BOTTOM) {
                // 与下边界相交
                intersection.x = p1.x + (p2.x - p1.x) * (ymax - p1.y) / (p2.y - p1.y);
                intersection.y = ymax;
            } else if (outcodeOut & RIGHT) {
                // 与右边界相交
                intersection.y = p1.y + (p2.y - p1.y) * (xmax - p1.x) / (p2.x - p1.x);
                intersection.x = xmax;
            } else if (outcodeOut & LEFT) {
                // 与左边界相交
                intersection.y = p1.y + (p2.y - p1.y) * (xmin - p1.x) / (p2.x - p1.x);
                intersection.x = xmin;
            }

            // 用交点替换窗口外的端点
            if (outcodeOut == outcode1) {
                p1 = intersection;
                outcode1 = ComputeOutCode(p1, xmin, ymin, xmax, ymax);
            } else {
                p2 = intersection;
                outcode2 = ComputeOutCode(p2, xmin, ymin, xmax, ymax);
            }
        }
    }
    return accept;
}

/**
 * @brief 判断点是否在裁剪窗口内部
 * @param point 待判断的点
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @return 如果点在窗口内返回true，否则返回false
 */
bool ClippingAlgorithms::IsInsideWindow(Point2D point, int xmin, int ymin, int xmax, int ymax) {
    return (point.x >= xmin && point.x <= xmax && point.y >= ymin && point.y <= ymax);
}

/**
 * @brief 判断两点是否在裁剪窗口的同一侧外部
 * @param p1 第一个点
 * @param p2 第二个点
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @return 如果两点在同一侧外部返回true
 * 
 * 用于中点分割算法的快速拒绝测试：
 * 如果两点都在窗口的同一侧（都在左边、都在右边、都在上边或都在下边），
 * 则整条线段必定在窗口外部，可以直接拒绝。
 */
bool ClippingAlgorithms::IsOutsideSameSide(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax) {
    bool p1Left = (p1.x < xmin), p2Left = (p2.x < xmin);
    bool p1Right = (p1.x > xmax), p2Right = (p2.x > xmax);
    bool p1Top = (p1.y < ymin), p2Top = (p2.y < ymin);
    bool p1Bottom = (p1.y > ymax), p2Bottom = (p2.y > ymax);
    return ((p1Left && p2Left) || (p1Right && p2Right) || (p1Top && p2Top) || (p1Bottom && p2Bottom));
}

/**
 * @brief 中点分割直线裁剪算法的递归实现
 * @param p1 线段起点
 * @param p2 线段终点
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @param result 输出的裁剪后线段集合
 * @param depth 当前递归深度
 * 
 * 算法原理：
 * 1. 如果两端点都在窗口内，直接接受整条线段
 * 2. 如果两端点在窗口同一侧外，直接拒绝
 * 3. 否则计算中点，将线段分成两半递归处理
 * 4. 当线段足够短（像素级别）时停止递归
 * 
 * 优点：
 * - 只使用加法和移位运算，适合硬件实现
 * - 可以并行处理两个子线段
 * 
 * 缺点：
 * - 递归深度可能较大
 * - 对于长线段效率不如Cohen-Sutherland
 */
void ClippingAlgorithms::ClipLineMidpointRecursive(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax,
                                                    std::vector<std::pair<Point2D, Point2D>>& result, int depth) {
    const int MAX_DEPTH = 20;  // 限制最大递归深度，防止栈溢出
    if (depth > MAX_DEPTH) return;

    // 判断两端点是否在窗口内
    bool p1Inside = IsInsideWindow(p1, xmin, ymin, xmax, ymax);
    bool p2Inside = IsInsideWindow(p2, xmin, ymin, xmax, ymax);

    // 情况1：两端点都在窗口内，完全接受
    if (p1Inside && p2Inside) {
        result.push_back(std::make_pair(p1, p2));
        return;
    }

    // 情况2：两端点在窗口同一侧外，完全拒绝
    if (IsOutsideSameSide(p1, p2, xmin, ymin, xmax, ymax)) return;

    // 情况3：计算中点，递归处理
    Point2D midpoint((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);

    // 终止条件：中点与端点距离小于等于1像素
    if ((abs(midpoint.x - p1.x) <= 1 && abs(midpoint.y - p1.y) <= 1) ||
        (abs(midpoint.x - p2.x) <= 1 && abs(midpoint.y - p2.y) <= 1)) {
        // 保留在窗口内的部分
        if (p1Inside) result.push_back(std::make_pair(p1, midpoint));
        else if (p2Inside) result.push_back(std::make_pair(midpoint, p2));
        return;
    }

    // 递归处理两个子线段
    ClipLineMidpointRecursive(p1, midpoint, xmin, ymin, xmax, ymax, result, depth + 1);
    ClipLineMidpointRecursive(midpoint, p2, xmin, ymin, xmax, ymax, result, depth + 1);
}

/**
 * @brief 中点分割直线裁剪算法入口
 * @param p1 线段起点
 * @param p2 线段终点
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @param result 输出的裁剪后线段集合
 * 
 * 中点分割算法是一种基于二分法的直线裁剪算法，
 * 通过不断将线段二分来逼近与窗口边界的交点。
 */
void ClippingAlgorithms::ClipLineMidpoint(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax,
                                          std::vector<std::pair<Point2D, Point2D>>& result) {
    ClipLineMidpointRecursive(p1, p2, xmin, ymin, xmax, ymax, result, 0);
}

/**
 * @brief 判断点是否在指定裁剪边的内侧
 * @param point 待判断的点
 * @param edge 裁剪边（左、右、下、上）
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @return 如果点在边的内侧返回true
 * 
 * 内侧定义：
 * - 左边界：x >= xmin
 * - 右边界：x <= xmax
 * - 下边界：y <= ymax
 * - 上边界：y >= ymin
 */
bool ClippingAlgorithms::IsInsideEdge(Point2D point, ClipEdge edge, int xmin, int ymin, int xmax, int ymax) {
    switch (edge) {
        case CLIP_LEFT: return point.x >= xmin;
        case CLIP_RIGHT: return point.x <= xmax;
        case CLIP_BOTTOM: return point.y <= ymax;
        case CLIP_TOP: return point.y >= ymin;
        default: return false;
    }
}

/**
 * @brief 计算线段与裁剪边的交点
 * @param p1 线段起点
 * @param p2 线段终点
 * @param edge 裁剪边（左、右、下、上）
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @return 交点坐标
 * 
 * 使用参数方程计算交点：
 * 对于垂直边界（左、右）：x = 边界值，y = p1.y + t * (p2.y - p1.y)
 * 对于水平边界（上、下）：y = 边界值，x = p1.x + t * (p2.x - p1.x)
 * 其中 t = (边界值 - p1坐标) / (p2坐标 - p1坐标)
 */
Point2D ClippingAlgorithms::ComputeIntersection(Point2D p1, Point2D p2, ClipEdge edge, int xmin, int ymin, int xmax, int ymax) {
    Point2D intersection;
    switch (edge) {
        case CLIP_LEFT:
            // 与左边界 x = xmin 相交
            intersection.x = xmin;
            intersection.y = (p2.x != p1.x) ? p1.y + (p2.y - p1.y) * (xmin - p1.x) / (p2.x - p1.x) : p1.y;
            break;
        case CLIP_RIGHT:
            // 与右边界 x = xmax 相交
            intersection.x = xmax;
            intersection.y = (p2.x != p1.x) ? p1.y + (p2.y - p1.y) * (xmax - p1.x) / (p2.x - p1.x) : p1.y;
            break;
        case CLIP_BOTTOM:
            // 与下边界 y = ymax 相交
            intersection.y = ymax;
            intersection.x = (p2.y != p1.y) ? p1.x + (p2.x - p1.x) * (ymax - p1.y) / (p2.y - p1.y) : p1.x;
            break;
        case CLIP_TOP:
            // 与上边界 y = ymin 相交
            intersection.y = ymin;
            intersection.x = (p2.y != p1.y) ? p1.x + (p2.x - p1.x) * (ymin - p1.y) / (p2.y - p1.y) : p1.x;
            break;
    }
    return intersection;
}

/**
 * @brief 用指定边裁剪多边形（Sutherland-Hodgman算法核心）
 * @param polygon 待裁剪的多边形顶点序列
 * @param edge 裁剪边
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @return 裁剪后的多边形顶点序列
 * 
 * 算法处理四种情况（从前一顶点到当前顶点）：
 * 1. 内→内：输出当前顶点
 * 2. 内→外：输出交点
 * 3. 外→内：输出交点和当前顶点
 * 4. 外→外：不输出任何顶点
 * 
 * 这是Sutherland-Hodgman算法的核心，通过对四条边依次调用此函数完成裁剪。
 */
std::vector<Point2D> ClippingAlgorithms::ClipPolygonAgainstEdge(const std::vector<Point2D>& polygon, ClipEdge edge,
                                                                 int xmin, int ymin, int xmax, int ymax) {
    std::vector<Point2D> outputList;
    if (polygon.empty()) return outputList;

    // 遍历多边形的每条边
    for (size_t i = 0; i < polygon.size(); i++) {
        Point2D currentVertex = polygon[i];
        Point2D previousVertex = polygon[(i + polygon.size() - 1) % polygon.size()];

        // 判断当前顶点和前一顶点是否在边的内侧
        bool currentInside = IsInsideEdge(currentVertex, edge, xmin, ymin, xmax, ymax);
        bool previousInside = IsInsideEdge(previousVertex, edge, xmin, ymin, xmax, ymax);

        if (previousInside && currentInside) {
            // 情况1：内→内，输出当前顶点
            outputList.push_back(currentVertex);
        } else if (previousInside && !currentInside) {
            // 情况2：内→外，输出交点
            Point2D intersection = ComputeIntersection(previousVertex, currentVertex, edge, xmin, ymin, xmax, ymax);
            outputList.push_back(intersection);
        } else if (!previousInside && currentInside) {
            // 情况3：外→内，输出交点和当前顶点
            Point2D intersection = ComputeIntersection(previousVertex, currentVertex, edge, xmin, ymin, xmax, ymax);
            outputList.push_back(intersection);
            outputList.push_back(currentVertex);
        }
        // 情况4：外→外，不输出任何顶点
    }
    return outputList;
}

/**
 * @brief Sutherland-Hodgman多边形裁剪算法
 * @param polygon 待裁剪的多边形顶点序列
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @return 裁剪后的多边形顶点序列
 * 
 * 算法原理：
 * 将多边形依次对裁剪窗口的四条边进行裁剪，
 * 每次裁剪的输出作为下一次裁剪的输入。
 * 
 * 裁剪顺序：左边界 → 右边界 → 下边界 → 上边界
 * 
 * 优点：
 * - 实现简单，易于理解
 * - 对凸多边形裁剪效果好
 * 
 * 缺点：
 * - 对凹多边形可能产生错误结果
 * - 只能产生一个输出多边形
 */
std::vector<Point2D> ClippingAlgorithms::ClipPolygonSutherlandHodgman(const std::vector<Point2D>& polygon,
                                                                       int xmin, int ymin, int xmax, int ymax) {
    std::vector<Point2D> clipped = polygon;
    
    // 依次对四条边进行裁剪
    if (!clipped.empty()) clipped = ClipPolygonAgainstEdge(clipped, CLIP_LEFT, xmin, ymin, xmax, ymax);
    if (!clipped.empty()) clipped = ClipPolygonAgainstEdge(clipped, CLIP_RIGHT, xmin, ymin, xmax, ymax);
    if (!clipped.empty()) clipped = ClipPolygonAgainstEdge(clipped, CLIP_BOTTOM, xmin, ymin, xmax, ymax);
    if (!clipped.empty()) clipped = ClipPolygonAgainstEdge(clipped, CLIP_TOP, xmin, ymin, xmax, ymax);
    
    return clipped;
}


// ============================================================================
// Weiler-Atherton 多边形裁剪算法实现
// ============================================================================
// 
// Weiler-Atherton算法是一种强大的多边形裁剪算法，能够处理：
// - 凹多边形的裁剪
// - 带孔多边形的裁剪
// - 可能产生多个独立的裁剪结果
// 
// 算法步骤：
// 1. 构建主多边形和裁剪窗口的顶点链表
// 2. 计算所有交点并插入到两个链表中
// 3. 标记每个交点是"进入点"还是"离开点"
// 4. 从进入点开始追踪，生成裁剪后的多边形
// ============================================================================

/**
 * @brief 计算两线段的交点（Weiler-Atherton算法辅助函数）
 * @param p1 第一条线段起点
 * @param p2 第一条线段终点
 * @param p3 第二条线段起点
 * @param p4 第二条线段终点
 * @param intersection 输出交点坐标
 * @param t1 第一条线段上交点的参数（0-1之间）
 * @param t2 第二条线段上交点的参数（0-1之间）
 * @return 如果线段相交返回true
 * 
 * 使用参数方程求解两线段交点：
 * 线段1: P = p1 + t1 * (p2 - p1), t1 ∈ [0,1]
 * 线段2: P = p3 + t2 * (p4 - p3), t2 ∈ [0,1]
 * 
 * 通过解线性方程组得到t1和t2，如果都在[0,1]范围内则相交。
 */
bool ClippingAlgorithms::SegmentIntersection(Point2D p1, Point2D p2, Point2D p3, Point2D p4,
                                              Point2D& intersection, double& t1, double& t2) {
    // 计算两线段的方向向量
    double dx1 = static_cast<double>(p2.x - p1.x);
    double dy1 = static_cast<double>(p2.y - p1.y);
    double dx2 = static_cast<double>(p4.x - p3.x);
    double dy2 = static_cast<double>(p4.y - p3.y);
    
    // 计算行列式（叉积），用于判断是否平行
    double denominator = dx1 * dy2 - dy1 * dx2;
    if (fabs(denominator) < 1e-10) return false;  // 平行或重合，无交点
    
    // 计算两起点之间的向量
    double dx = static_cast<double>(p3.x - p1.x);
    double dy = static_cast<double>(p3.y - p1.y);
    
    // 使用克莱姆法则求解参数t1和t2
    t1 = (dx * dy2 - dy * dx2) / denominator;
    t2 = (dx * dy1 - dy * dx1) / denominator;
    
    // 检查参数是否在有效范围内（线段内部）
    if (t1 >= 0.0 && t1 <= 1.0 && t2 >= 0.0 && t2 <= 1.0) {
        // 计算交点坐标（四舍五入到整数）
        intersection.x = static_cast<int>(p1.x + t1 * dx1 + 0.5);
        intersection.y = static_cast<int>(p1.y + t1 * dy1 + 0.5);
        return true;
    }
    return false;
}

/**
 * @brief 判断点是否在裁剪窗口内部（Weiler-Atherton算法辅助函数）
 * @param point 待判断的点
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @return 如果点在窗口内返回true
 */
bool ClippingAlgorithms::IsPointInsideWindow(Point2D point, int xmin, int ymin, int xmax, int ymax) {
    return (point.x >= xmin && point.x <= xmax && point.y >= ymin && point.y <= ymax);
}

/**
 * @brief 构建主多边形的顶点链表
 * @param polygon 多边形顶点序列
 * @return 顶点链表（循环链表）
 * 
 * 将多边形顶点转换为双向循环链表结构，便于后续插入交点和遍历。
 * 每个顶点包含坐标、是否为交点、进入/离开标记等信息。
 */
std::vector<ClippingAlgorithms::WAVertex*> ClippingAlgorithms::BuildPolygonVertexList(
    const std::vector<Point2D>& polygon) {
    std::vector<WAVertex*> vertexList;
    
    // 为每个顶点创建WAVertex对象
    for (size_t i = 0; i < polygon.size(); i++) {
        WAVertex* vertex = new WAVertex(polygon[i]);
        vertex->id = static_cast<int>(i);  // 分配唯一ID
        vertexList.push_back(vertex);
    }
    
    // 构建循环链表
    for (size_t i = 0; i < vertexList.size(); i++) {
        vertexList[i]->next = vertexList[(i + 1) % vertexList.size()];
    }
    
    return vertexList;
}

/**
 * @brief 构建裁剪窗口的顶点链表
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @return 裁剪窗口顶点链表（循环链表，逆时针方向）
 * 
 * 裁剪窗口是一个矩形，按逆时针方向构建顶点链表：
 * 左下 → 右下 → 右上 → 左上 → 左下（循环）
 */
std::vector<ClippingAlgorithms::WAVertex*> ClippingAlgorithms::BuildClipWindowVertexList(
    int xmin, int ymin, int xmax, int ymax) {
    std::vector<WAVertex*> vertexList;
    
    // 创建四个角点（逆时针方向）
    WAVertex* v1 = new WAVertex(Point2D(xmin, ymin));  // 左下
    WAVertex* v2 = new WAVertex(Point2D(xmax, ymin));  // 右下
    WAVertex* v3 = new WAVertex(Point2D(xmax, ymax));  // 右上
    WAVertex* v4 = new WAVertex(Point2D(xmin, ymax));  // 左上
    
    // 分配ID（使用1000+的范围与主多边形区分）
    v1->id = 1000; v2->id = 1001; v3->id = 1002; v4->id = 1003;
    vertexList.push_back(v1); vertexList.push_back(v2);
    vertexList.push_back(v3); vertexList.push_back(v4);
    
    // 构建循环链表
    v1->next = v2; v2->next = v3; v3->next = v4; v4->next = v1;
    
    return vertexList;
}

/**
 * @brief 计算并插入所有交点到顶点链表中
 * @param polyList 主多边形顶点链表
 * @param clipList 裁剪窗口顶点链表
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * 
 * 算法步骤：
 * 1. 遍历主多边形的每条边与裁剪窗口的每条边
 * 2. 计算所有交点
 * 3. 为每个交点创建两个顶点对象（分别插入两个链表）
 * 4. 建立交点之间的neighbor关系（用于链表间跳转）
 * 5. 按参数t排序后插入到链表中
 * 
 * 注意：交点需要按照在边上的位置（参数t）排序后插入，
 * 以保证链表的正确顺序。
 */
void ClippingAlgorithms::InsertIntersections(std::vector<WAVertex*>& polyList, 
                                              std::vector<WAVertex*>& clipList,
                                              int xmin, int ymin, int xmax, int ymax) {
    // 交点信息结构，用于排序和插入
    struct IntersectionInfo {
        WAVertex* beforeVertex;    // 交点前面的顶点
        double t;                  // 交点在边上的参数位置
        WAVertex* polyIntersect;   // 主多边形链表中的交点
        WAVertex* clipIntersect;   // 裁剪窗口链表中的交点
    };
    
    std::vector<IntersectionInfo> polyIntersections;   // 主多边形的交点信息
    std::vector<IntersectionInfo> clipIntersections;   // 裁剪窗口的交点信息
    
    // 遍历主多边形的每条边
    WAVertex* polyStart = polyList[0];
    WAVertex* polyCurrent = polyStart;
    do {
        Point2D p1 = polyCurrent->point;
        Point2D p2 = polyCurrent->next->point;
        
        // 遍历裁剪窗口的每条边
        WAVertex* clipStart = clipList[0];
        WAVertex* clipCurrent = clipStart;
        do {
            Point2D p3 = clipCurrent->point;
            Point2D p4 = clipCurrent->next->point;
            Point2D intersection;
            double t1, t2;
            
            // 计算两条边的交点
            if (SegmentIntersection(p1, p2, p3, p4, intersection, t1, t2)) {
                // 排除端点交点（避免重复）
                if (t1 > 0.0001 && t1 < 0.9999 && t2 > 0.0001 && t2 < 0.9999) {
                    // 创建两个交点顶点（分别用于两个链表）
                    WAVertex* polyIntersect = new WAVertex(intersection);
                    polyIntersect->isIntersection = true;
                    WAVertex* clipIntersect = new WAVertex(intersection);
                    clipIntersect->isIntersection = true;
                    
                    // 建立neighbor关系（用于在两个链表间跳转）
                    polyIntersect->neighbor = clipIntersect;
                    clipIntersect->neighbor = polyIntersect;
                    
                    // 分配唯一ID
                    polyIntersect->id = 2000 + static_cast<int>(polyIntersections.size());
                    clipIntersect->id = 3000 + static_cast<int>(clipIntersections.size());
                    
                    // 记录主多边形交点信息
                    IntersectionInfo polyInfo;
                    polyInfo.beforeVertex = polyCurrent;
                    polyInfo.t = t1;
                    polyInfo.polyIntersect = polyIntersect;
                    polyInfo.clipIntersect = clipIntersect;
                    polyIntersections.push_back(polyInfo);
                    
                    // 记录裁剪窗口交点信息
                    IntersectionInfo clipInfo;
                    clipInfo.beforeVertex = clipCurrent;
                    clipInfo.t = t2;
                    clipInfo.polyIntersect = polyIntersect;
                    clipInfo.clipIntersect = clipIntersect;
                    clipIntersections.push_back(clipInfo);
                }
            }
            clipCurrent = clipCurrent->next;
        } while (clipCurrent != clipStart);
        polyCurrent = polyCurrent->next;
    } while (polyCurrent != polyStart);
    
    // 按边和参数t排序主多边形交点（同一条边上的交点按t值排序）
    for (size_t i = 0; i < polyIntersections.size(); i++) {
        for (size_t j = i + 1; j < polyIntersections.size(); j++) {
            bool shouldSwap = false;
            if (polyIntersections[i].beforeVertex == polyIntersections[j].beforeVertex) {
                shouldSwap = polyIntersections[i].t > polyIntersections[j].t;
            }
            if (shouldSwap) {
                std::swap(polyIntersections[i], polyIntersections[j]);
            }
        }
    }
    
    // 倒序插入交点到主多边形链表（从大到小的t值）
    // 这样插入时不会影响之前交点的位置
    for (int i = static_cast<int>(polyIntersections.size()) - 1; i >= 0; i--) {
        WAVertex* before = polyIntersections[i].beforeVertex;
        WAVertex* intersect = polyIntersections[i].polyIntersect;
        intersect->next = before->next;
        before->next = intersect;
        polyList.push_back(intersect);
    }
    
    // 按边和参数t排序裁剪窗口交点
    for (size_t i = 0; i < clipIntersections.size(); i++) {
        for (size_t j = i + 1; j < clipIntersections.size(); j++) {
            bool shouldSwap = false;
            if (clipIntersections[i].beforeVertex == clipIntersections[j].beforeVertex) {
                shouldSwap = clipIntersections[i].t > clipIntersections[j].t;
            }
            if (shouldSwap) {
                std::swap(clipIntersections[i], clipIntersections[j]);
            }
        }
    }
    
    // 倒序插入交点到裁剪窗口链表
    for (int i = static_cast<int>(clipIntersections.size()) - 1; i >= 0; i--) {
        WAVertex* before = clipIntersections[i].beforeVertex;
        WAVertex* intersect = clipIntersections[i].clipIntersect;
        intersect->next = before->next;
        before->next = intersect;
        clipList.push_back(intersect);
    }
}

/**
 * @brief 标记交点的进入/离开属性
 * @param polyList 主多边形顶点链表（包含交点）
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * 
 * 算法原理：
 * 1. 从一个非交点顶点开始，确定其是否在裁剪窗口内
 * 2. 沿着多边形边界遍历
 * 3. 每遇到一个交点，根据当前状态标记：
 *    - 如果当前在窗口外，则该交点是"进入点"（isEntry = true）
 *    - 如果当前在窗口内，则该交点是"离开点"（isEntry = false）
 * 4. 穿过交点后，内外状态翻转
 * 
 * 进入点和离开点的标记对于后续追踪裁剪多边形至关重要：
 * - 从进入点开始追踪
 * - 遇到离开点时切换到裁剪窗口边界
 */
void ClippingAlgorithms::MarkEntryExit(std::vector<WAVertex*>& polyList, 
                                        int xmin, int ymin, int xmax, int ymax) {
    if (polyList.empty()) return;
    
    // 从第一个原始顶点开始遍历（不是交点）
    WAVertex* start = polyList[0];
    WAVertex* current = start;
    
    // 找到第一个非交点的顶点作为起始点
    int searchLimit = static_cast<int>(polyList.size()) * 2;
    int searchCount = 0;
    while (current->isIntersection && searchCount < searchLimit) {
        current = current->next;
        searchCount++;
        if (current == start) break;
    }
    
    if (current->isIntersection) {
        // 所有顶点都是交点，这种情况不应该发生
        return;
    }
    
    // 确定起始状态：当前顶点是否在裁剪窗口内
    bool inside = IsPointInsideWindow(current->point, xmin, ymin, xmax, ymax);
    start = current;
    
    // 遍历整个链表并标记每个交点
    int maxIterations = static_cast<int>(polyList.size()) * 2;
    int iterations = 0;
    
    do {
        current = current->next;
        iterations++;
        if (!current || iterations >= maxIterations) break;
        
        if (current->isIntersection) {
            // 根据当前状态标记交点类型
            // 如果当前在外面，这个交点是入点；如果在里面，是出点
            current->isEntry = !inside;
            inside = !inside; // 穿过交点后状态翻转
        }
    } while (current != start);
}

/**
 * @brief 追踪并生成裁剪后的多边形
 * @param polyList 主多边形顶点链表（包含已标记的交点）
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @return 裁剪后的多边形集合
 * 
 * 追踪算法：
 * 1. 从一个未访问的进入点开始
 * 2. 沿主多边形边界前进，收集顶点
 * 3. 遇到离开点时，通过neighbor指针切换到裁剪窗口边界
 * 4. 沿裁剪窗口边界前进
 * 5. 遇到下一个交点时，切换回主多边形
 * 6. 重复直到回到起始点
 * 7. 对所有未访问的进入点重复上述过程
 * 
 * 特殊情况处理：
 * - 如果没有进入点但有交点，说明多边形起点在窗口内
 * - 需要从窗口内的顶点开始追踪
 */
std::vector<std::vector<Point2D>> ClippingAlgorithms::TraceClippedPolygons(
    std::vector<WAVertex*>& polyList, int xmin, int ymin, int xmax, int ymax) {
    std::vector<std::vector<Point2D>> resultPolygons;
    
    // 重置所有顶点的访问标记
    for (WAVertex* v : polyList) {
        v->visited = false;
        if (v->neighbor) {
            v->neighbor->visited = false;
        }
    }
    
    // 检查是否有入点
    bool hasEntryPoint = false;
    for (WAVertex* v : polyList) {
        if (v->isIntersection && v->isEntry) {
            hasEntryPoint = true;
            break;
        }
    }
    
    // 特殊情况：如果没有入点但有交点，说明多边形起点在窗口内部
    // 需要收集窗口内的顶点，从第一个在窗口内的顶点开始，到出点结束
    if (!hasEntryPoint) {
        std::vector<Point2D> polygon;
        WAVertex* start = nullptr;
        
        // 找到第一个在窗口内的非交点顶点
        for (WAVertex* v : polyList) {
            if (!v->isIntersection && 
                v->point.x >= xmin && v->point.x <= xmax &&
                v->point.y >= ymin && v->point.y <= ymax) {
                start = v;
                break;
            }
        }
        
        if (start) {
            WAVertex* current = start;
            int maxIterations = 1000;
            int iterations = 0;
            
            do {
                polygon.push_back(current->point);
                current = current->next;
                iterations++;
                
                // 遇到出点（isEntry=false的交点），添加它然后切换到裁剪窗口
                if (current->isIntersection && !current->isEntry) {
                    polygon.push_back(current->point);
                    // 切换到裁剪窗口，沿着窗口走到下一个交点
                    if (current->neighbor) {
                        current = current->neighbor->next;
                        while (current && !current->isIntersection && iterations < maxIterations) {
                            // 添加裁剪窗口的顶点
                            polygon.push_back(current->point);
                            current = current->next;
                            iterations++;
                        }
                        if (current && current->isIntersection) {
                            polygon.push_back(current->point);
                            if (current->neighbor) {
                                current = current->neighbor->next;
                            }
                        }
                    }
                }
            } while (current && current != start && iterations < maxIterations);
            
            // 只有至少3个顶点才能构成多边形
            if (polygon.size() >= 3) {
                resultPolygons.push_back(polygon);
            }
        }
        return resultPolygons;
    }
    
    // 标准情况：从每个未访问的入点开始追踪多边形
    for (WAVertex* v : polyList) {
        if (v->isIntersection && v->isEntry && !v->visited) {
            std::vector<Point2D> polygon;
            WAVertex* start = v;
            WAVertex* current = v;
            int maxIterations = 1000;
            int iterations = 0;
            bool onSubjectPolygon = true; // 开始在主多边形上，从入点进入
            bool firstPoint = true;
            
            while (iterations < maxIterations) {
                // 标记当前交点为已访问
                if (current->isIntersection) {
                    current->visited = true;
                    if (current->neighbor) {
                        current->neighbor->visited = true;
                    }
                }
                
                // 添加当前点到多边形（避免重复）
                if (polygon.empty() || 
                    polygon.back().x != current->point.x || 
                    polygon.back().y != current->point.y) {
                    polygon.push_back(current->point);
                }
                
                // 如果当前是交点，需要判断是否切换多边形
                // 但是第一个点（入点）不切换，继续沿主多边形走
                if (!firstPoint && current->isIntersection && current->neighbor) {
                    if (onSubjectPolygon && !current->isEntry) {
                        // 主多边形上遇到出点，切换到裁剪窗口
                        current = current->neighbor;
                        onSubjectPolygon = false;
                    } else if (!onSubjectPolygon) {
                        // 裁剪窗口上遇到交点，切换回主多边形
                        current = current->neighbor;
                        onSubjectPolygon = true;
                    }
                }
                
                firstPoint = false;
                
                // 移动到下一个顶点
                current = current->next;
                iterations++;
                
                if (!current) break;
                
                // 检查是否回到起点
                if (current == start) {
                    break;
                }
                // 也检查坐标是否相同（防止指针不同但位置相同的情况）
                if (current->point.x == start->point.x && current->point.y == start->point.y) {
                    break;
                }
            }
            
            // 只有至少3个顶点才能构成多边形
            if (polygon.size() >= 3) {
                resultPolygons.push_back(polygon);
            }
        }
    }
    return resultPolygons;
}

/**
 * @brief 清理顶点链表内存
 * @param vertexList 待清理的顶点链表
 * 
 * 释放链表中所有顶点对象的内存。
 * 注意：交点顶点会被两个链表引用，需要特殊处理避免重复删除。
 */
void ClippingAlgorithms::CleanupVertexList(std::vector<WAVertex*>& vertexList) {
    for (WAVertex* v : vertexList) {
        if (v) {
            delete v;
        }
    }
    vertexList.clear();
}

/**
 * @brief Weiler-Atherton多边形裁剪算法
 * @param polygon 待裁剪的多边形顶点序列
 * @param xmin 裁剪窗口左边界
 * @param ymin 裁剪窗口下边界
 * @param xmax 裁剪窗口右边界
 * @param ymax 裁剪窗口上边界
 * @return 裁剪后的多边形集合（可能产生多个多边形）
 * 
 * Weiler-Atherton算法是最强大的多边形裁剪算法之一，能够处理：
 * - 凸多边形和凹多边形
 * - 带孔的多边形
 * - 可能产生多个独立的裁剪结果
 * 
 * 算法步骤：
 * 1. 快速检测：如果多边形完全在窗口内或完全在窗口外，直接返回
 * 2. 构建主多边形和裁剪窗口的顶点链表
 * 3. 计算所有交点并插入到两个链表中
 * 4. 标记每个交点是"进入点"还是"离开点"
 * 5. 从每个进入点开始追踪，生成裁剪后的多边形
 * 6. 清理内存
 * 
 * 时间复杂度：O(n*m)，其中n和m分别为主多边形和裁剪窗口的顶点数
 * 空间复杂度：O(n+m+k)，其中k为交点数
 */
std::vector<std::vector<Point2D>> ClippingAlgorithms::ClipPolygonWeilerAtherton(
    const std::vector<Point2D>& polygon, int xmin, int ymin, int xmax, int ymax) {
    std::vector<std::vector<Point2D>> result;
    
    // 多边形至少需要3个顶点
    if (polygon.size() < 3) {
        return result;
    }
    
    // 快速检测：检查所有顶点是否都在裁剪窗口内或外
    bool allInside = true;
    bool allOutside = true;
    for (const Point2D& p : polygon) {
        if (IsPointInsideWindow(p, xmin, ymin, xmax, ymax)) {
            allOutside = false;
        } else {
            allInside = false;
        }
    }
    
    // 如果所有顶点都在窗口内，返回原多边形（无需裁剪）
    if (allInside) {
        result.push_back(polygon);
        return result;
    }
    
    // 构建主多边形和裁剪窗口的顶点链表
    std::vector<WAVertex*> polyList = BuildPolygonVertexList(polygon);
    std::vector<WAVertex*> clipList = BuildClipWindowVertexList(xmin, ymin, xmax, ymax);
    
    // 计算并插入所有交点
    InsertIntersections(polyList, clipList, xmin, ymin, xmax, ymax);
    
    // 检查是否有交点
    bool hasIntersections = false;
    for (WAVertex* v : polyList) {
        if (v->isIntersection) {
            hasIntersections = true;
            break;
        }
    }
    
    // 如果没有交点且不是全在内部，说明完全在外部
    if (!hasIntersections) {
        // 清理内存
        for (WAVertex* v : clipList) {
            if (!v->isIntersection) {
                delete v;
            }
        }
        clipList.clear();
        for (WAVertex* v : polyList) {
            delete v;
        }
        polyList.clear();
        return result; // 返回空结果
    }
    
    // 标记交点的进入/离开属性
    MarkEntryExit(polyList, xmin, ymin, xmax, ymax);
    
    // 追踪并生成裁剪后的多边形
    result = TraceClippedPolygons(polyList, xmin, ymin, xmax, ymax);
    
    // 清理内存：使用set避免重复删除（交点被两个链表引用）
    std::set<WAVertex*> toDelete;
    for (WAVertex* v : polyList) {
        toDelete.insert(v);
    }
    for (WAVertex* v : clipList) {
        toDelete.insert(v);
    }
    for (WAVertex* v : toDelete) {
        delete v;
    }
    polyList.clear();
    clipList.clear();
    
    return result;
}
