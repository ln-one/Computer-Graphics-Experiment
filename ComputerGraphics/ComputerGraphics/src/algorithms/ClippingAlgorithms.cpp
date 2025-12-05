#include "ClippingAlgorithms.h"
#include <cmath>
#include <set>

int ClippingAlgorithms::ComputeOutCode(Point2D point, int xmin, int ymin, int xmax, int ymax) {
    int code = INSIDE;
    if (point.x < xmin) code |= LEFT;
    else if (point.x > xmax) code |= RIGHT;
    if (point.y < ymin) code |= TOP;
    else if (point.y > ymax) code |= BOTTOM;
    return code;
}

bool ClippingAlgorithms::ClipLineCohenSutherland(Point2D& p1, Point2D& p2, int xmin, int ymin, int xmax, int ymax) {
    int outcode1 = ComputeOutCode(p1, xmin, ymin, xmax, ymax);
    int outcode2 = ComputeOutCode(p2, xmin, ymin, xmax, ymax);
    bool accept = false;

    while (true) {
        if ((outcode1 | outcode2) == 0) {
            accept = true;
            break;
        } else if ((outcode1 & outcode2) != 0) {
            break;
        } else {
            int outcodeOut = outcode1 ? outcode1 : outcode2;
            Point2D intersection;

            if (outcodeOut & TOP) {
                intersection.x = p1.x + (p2.x - p1.x) * (ymin - p1.y) / (p2.y - p1.y);
                intersection.y = ymin;
            } else if (outcodeOut & BOTTOM) {
                intersection.x = p1.x + (p2.x - p1.x) * (ymax - p1.y) / (p2.y - p1.y);
                intersection.y = ymax;
            } else if (outcodeOut & RIGHT) {
                intersection.y = p1.y + (p2.y - p1.y) * (xmax - p1.x) / (p2.x - p1.x);
                intersection.x = xmax;
            } else if (outcodeOut & LEFT) {
                intersection.y = p1.y + (p2.y - p1.y) * (xmin - p1.x) / (p2.x - p1.x);
                intersection.x = xmin;
            }

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

bool ClippingAlgorithms::IsInsideWindow(Point2D point, int xmin, int ymin, int xmax, int ymax) {
    return (point.x >= xmin && point.x <= xmax && point.y >= ymin && point.y <= ymax);
}

bool ClippingAlgorithms::IsOutsideSameSide(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax) {
    bool p1Left = (p1.x < xmin), p2Left = (p2.x < xmin);
    bool p1Right = (p1.x > xmax), p2Right = (p2.x > xmax);
    bool p1Top = (p1.y < ymin), p2Top = (p2.y < ymin);
    bool p1Bottom = (p1.y > ymax), p2Bottom = (p2.y > ymax);
    return ((p1Left && p2Left) || (p1Right && p2Right) || (p1Top && p2Top) || (p1Bottom && p2Bottom));
}

void ClippingAlgorithms::ClipLineMidpointRecursive(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax,
                                                    std::vector<std::pair<Point2D, Point2D>>& result, int depth) {
    const int MAX_DEPTH = 20;
    if (depth > MAX_DEPTH) return;

    bool p1Inside = IsInsideWindow(p1, xmin, ymin, xmax, ymax);
    bool p2Inside = IsInsideWindow(p2, xmin, ymin, xmax, ymax);

    if (p1Inside && p2Inside) {
        result.push_back(std::make_pair(p1, p2));
        return;
    }

    if (IsOutsideSameSide(p1, p2, xmin, ymin, xmax, ymax)) return;

    Point2D midpoint((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);

    if ((abs(midpoint.x - p1.x) <= 1 && abs(midpoint.y - p1.y) <= 1) ||
        (abs(midpoint.x - p2.x) <= 1 && abs(midpoint.y - p2.y) <= 1)) {
        if (p1Inside) result.push_back(std::make_pair(p1, midpoint));
        else if (p2Inside) result.push_back(std::make_pair(midpoint, p2));
        return;
    }

    ClipLineMidpointRecursive(p1, midpoint, xmin, ymin, xmax, ymax, result, depth + 1);
    ClipLineMidpointRecursive(midpoint, p2, xmin, ymin, xmax, ymax, result, depth + 1);
}

void ClippingAlgorithms::ClipLineMidpoint(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax,
                                          std::vector<std::pair<Point2D, Point2D>>& result) {
    ClipLineMidpointRecursive(p1, p2, xmin, ymin, xmax, ymax, result, 0);
}

bool ClippingAlgorithms::IsInsideEdge(Point2D point, ClipEdge edge, int xmin, int ymin, int xmax, int ymax) {
    switch (edge) {
        case CLIP_LEFT: return point.x >= xmin;
        case CLIP_RIGHT: return point.x <= xmax;
        case CLIP_BOTTOM: return point.y <= ymax;
        case CLIP_TOP: return point.y >= ymin;
        default: return false;
    }
}

Point2D ClippingAlgorithms::ComputeIntersection(Point2D p1, Point2D p2, ClipEdge edge, int xmin, int ymin, int xmax, int ymax) {
    Point2D intersection;
    switch (edge) {
        case CLIP_LEFT:
            intersection.x = xmin;
            intersection.y = (p2.x != p1.x) ? p1.y + (p2.y - p1.y) * (xmin - p1.x) / (p2.x - p1.x) : p1.y;
            break;
        case CLIP_RIGHT:
            intersection.x = xmax;
            intersection.y = (p2.x != p1.x) ? p1.y + (p2.y - p1.y) * (xmax - p1.x) / (p2.x - p1.x) : p1.y;
            break;
        case CLIP_BOTTOM:
            intersection.y = ymax;
            intersection.x = (p2.y != p1.y) ? p1.x + (p2.x - p1.x) * (ymax - p1.y) / (p2.y - p1.y) : p1.x;
            break;
        case CLIP_TOP:
            intersection.y = ymin;
            intersection.x = (p2.y != p1.y) ? p1.x + (p2.x - p1.x) * (ymin - p1.y) / (p2.y - p1.y) : p1.x;
            break;
    }
    return intersection;
}

std::vector<Point2D> ClippingAlgorithms::ClipPolygonAgainstEdge(const std::vector<Point2D>& polygon, ClipEdge edge,
                                                                 int xmin, int ymin, int xmax, int ymax) {
    std::vector<Point2D> outputList;
    if (polygon.empty()) return outputList;

    for (size_t i = 0; i < polygon.size(); i++) {
        Point2D currentVertex = polygon[i];
        Point2D previousVertex = polygon[(i + polygon.size() - 1) % polygon.size()];

        bool currentInside = IsInsideEdge(currentVertex, edge, xmin, ymin, xmax, ymax);
        bool previousInside = IsInsideEdge(previousVertex, edge, xmin, ymin, xmax, ymax);

        if (previousInside && currentInside) {
            outputList.push_back(currentVertex);
        } else if (previousInside && !currentInside) {
            Point2D intersection = ComputeIntersection(previousVertex, currentVertex, edge, xmin, ymin, xmax, ymax);
            outputList.push_back(intersection);
        } else if (!previousInside && currentInside) {
            Point2D intersection = ComputeIntersection(previousVertex, currentVertex, edge, xmin, ymin, xmax, ymax);
            outputList.push_back(intersection);
            outputList.push_back(currentVertex);
        }
    }
    return outputList;
}

std::vector<Point2D> ClippingAlgorithms::ClipPolygonSutherlandHodgman(const std::vector<Point2D>& polygon,
                                                                       int xmin, int ymin, int xmax, int ymax) {
    std::vector<Point2D> clipped = polygon;
    if (!clipped.empty()) clipped = ClipPolygonAgainstEdge(clipped, CLIP_LEFT, xmin, ymin, xmax, ymax);
    if (!clipped.empty()) clipped = ClipPolygonAgainstEdge(clipped, CLIP_RIGHT, xmin, ymin, xmax, ymax);
    if (!clipped.empty()) clipped = ClipPolygonAgainstEdge(clipped, CLIP_BOTTOM, xmin, ymin, xmax, ymax);
    if (!clipped.empty()) clipped = ClipPolygonAgainstEdge(clipped, CLIP_TOP, xmin, ymin, xmax, ymax);
    return clipped;
}


// Weiler-Atherton polygon clipping implementation
bool ClippingAlgorithms::SegmentIntersection(Point2D p1, Point2D p2, Point2D p3, Point2D p4,
                                              Point2D& intersection, double& t1, double& t2) {
    double dx1 = p2.x - p1.x;
    double dy1 = p2.y - p1.y;
    double dx2 = p4.x - p3.x;
    double dy2 = p4.y - p3.y;
    double denominator = dx1 * dy2 - dy1 * dx2;
    
    if (abs(denominator) < 1e-10) return false;
    
    double dx3 = p3.x - p1.x;
    double dy3 = p3.y - p1.y;
    t1 = (dx3 * dy2 - dy3 * dx2) / denominator;
    t2 = (dx1 * dy3 - dy1 * dx3) / denominator;
    
    if (t1 >= 0.0 && t1 <= 1.0 && t2 >= 0.0 && t2 <= 1.0) {
        intersection.x = static_cast<int>(p1.x + t1 * dx1 + 0.5);
        intersection.y = static_cast<int>(p1.y + t1 * dy1 + 0.5);
        return true;
    }
    return false;
}

bool ClippingAlgorithms::IsPointInsideWindow(Point2D point, int xmin, int ymin, int xmax, int ymax) {
    return (point.x >= xmin && point.x <= xmax && point.y >= ymin && point.y <= ymax);
}

std::vector<ClippingAlgorithms::WAVertex*> ClippingAlgorithms::BuildPolygonVertexList(
    const std::vector<Point2D>& polygon) {
    std::vector<WAVertex*> vertexList;
    for (size_t i = 0; i < polygon.size(); i++) {
        WAVertex* vertex = new WAVertex(polygon[i]);
        vertex->id = static_cast<int>(i);
        vertexList.push_back(vertex);
    }
    for (size_t i = 0; i < vertexList.size(); i++) {
        vertexList[i]->next = vertexList[(i + 1) % vertexList.size()];
    }
    return vertexList;
}

std::vector<ClippingAlgorithms::WAVertex*> ClippingAlgorithms::BuildClipWindowVertexList(
    int xmin, int ymin, int xmax, int ymax) {
    std::vector<WAVertex*> vertexList;
    // 顺时针顺序: 左上 -> 右上 -> 右下 -> 左下
    WAVertex* v1 = new WAVertex(Point2D(xmin, ymin));  // 左上
    WAVertex* v2 = new WAVertex(Point2D(xmax, ymin));  // 右上
    WAVertex* v3 = new WAVertex(Point2D(xmax, ymax));  // 右下
    WAVertex* v4 = new WAVertex(Point2D(xmin, ymax));  // 左下
    
    v1->id = 1000; v2->id = 1001; v3->id = 1002; v4->id = 1003;
    vertexList.push_back(v1); vertexList.push_back(v2);
    vertexList.push_back(v3); vertexList.push_back(v4);
    
    // 顺时针链接
    v1->next = v2; v2->next = v3; v3->next = v4; v4->next = v1;
    return vertexList;
}

void ClippingAlgorithms::InsertIntersections(std::vector<WAVertex*>& polyList, 
                                              std::vector<WAVertex*>& clipList,
                                              int xmin, int ymin, int xmax, int ymax) {
    struct IntersectionInfo {
        WAVertex* beforeVertex;
        double t;
        WAVertex* polyIntersect;
        WAVertex* clipIntersect;
    };
    
    std::vector<IntersectionInfo> polyIntersections;
    std::vector<IntersectionInfo> clipIntersections;
    
    WAVertex* polyStart = polyList[0];
    WAVertex* polyCurrent = polyStart;
    do {
        Point2D p1 = polyCurrent->point;
        Point2D p2 = polyCurrent->next->point;
        
        WAVertex* clipStart = clipList[0];
        WAVertex* clipCurrent = clipStart;
        do {
            Point2D p3 = clipCurrent->point;
            Point2D p4 = clipCurrent->next->point;
            Point2D intersection;
            double t1, t2;
            
            if (SegmentIntersection(p1, p2, p3, p4, intersection, t1, t2)) {
                if (t1 > 0.001 && t1 < 0.999 && t2 > 0.001 && t2 < 0.999) {
                    WAVertex* polyIntersect = new WAVertex(intersection);
                    polyIntersect->isIntersection = true;
                    WAVertex* clipIntersect = new WAVertex(intersection);
                    clipIntersect->isIntersection = true;
                    
                    polyIntersect->neighbor = clipIntersect;
                    clipIntersect->neighbor = polyIntersect;
                    polyIntersect->id = 2000 + static_cast<int>(polyIntersections.size());
                    clipIntersect->id = 3000 + static_cast<int>(clipIntersections.size());
                    
                    IntersectionInfo polyInfo;
                    polyInfo.beforeVertex = polyCurrent;
                    polyInfo.t = t1;
                    polyInfo.polyIntersect = polyIntersect;
                    polyInfo.clipIntersect = clipIntersect;
                    polyIntersections.push_back(polyInfo);
                    
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
    
    // Sort by beforeVertex and t value for polygon intersections
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
    
    // Group by beforeVertex and insert in reverse order of t (largest t first)
    // This way, inserting doesn't affect the positions of earlier intersections
    for (int i = static_cast<int>(polyIntersections.size()) - 1; i >= 0; i--) {
        WAVertex* before = polyIntersections[i].beforeVertex;
        WAVertex* intersect = polyIntersections[i].polyIntersect;
        intersect->next = before->next;
        before->next = intersect;
        polyList.push_back(intersect);
    }
    
    // Sort by beforeVertex and t value for clip intersections  
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
    
    for (int i = static_cast<int>(clipIntersections.size()) - 1; i >= 0; i--) {
        WAVertex* before = clipIntersections[i].beforeVertex;
        WAVertex* intersect = clipIntersections[i].clipIntersect;
        intersect->next = before->next;
        before->next = intersect;
        clipList.push_back(intersect);
    }
}

void ClippingAlgorithms::MarkEntryExit(std::vector<WAVertex*>& polyList, 
                                        int xmin, int ymin, int xmax, int ymax) {
    if (polyList.empty()) return;
    
    // 从第一个原始顶点开始遍历（不是交点）
    WAVertex* start = polyList[0];
    WAVertex* current = start;
    
    // 找到第一个非交点的顶点
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
            // 如果当前在外面，这个交点是入点；如果在里面，是出点
            current->isEntry = !inside;
            inside = !inside; // 穿过交点后状态翻转
        }
    } while (current != start);
}

std::vector<std::vector<Point2D>> ClippingAlgorithms::TraceClippedPolygons(
    std::vector<WAVertex*>& polyList) {
    std::vector<std::vector<Point2D>> resultPolygons;
    
    // Reset visited flags for all vertices
    for (WAVertex* v : polyList) {
        v->visited = false;
        if (v->neighbor) {
            v->neighbor->visited = false;
        }
    }
    
    // Find all entry points and trace polygons
    for (WAVertex* v : polyList) {
        if (v->isIntersection && v->isEntry && !v->visited) {
            std::vector<Point2D> polygon;
            WAVertex* start = v;
            WAVertex* current = v;
            int maxIterations = 1000;
            int iterations = 0;
            bool onSubjectPolygon = true; // 开始在主多边形上
            
            while (iterations < maxIterations) {
                // Add current point to polygon (avoid duplicates)
                if (polygon.empty() || 
                    polygon.back().x != current->point.x || 
                    polygon.back().y != current->point.y) {
                    polygon.push_back(current->point);
                }
                
                // Mark current intersection as visited
                if (current->isIntersection) {
                    current->visited = true;
                    if (current->neighbor) {
                        current->neighbor->visited = true;
                    }
                }
                
                // Move to next vertex
                current = current->next;
                iterations++;
                
                if (!current) break;
                
                // Check if we've returned to start
                if (current == start) {
                    break;
                }
                
                // 如果遇到交点
                if (current->isIntersection && current->neighbor) {
                    // 添加交点
                    if (polygon.empty() || 
                        polygon.back().x != current->point.x || 
                        polygon.back().y != current->point.y) {
                        polygon.push_back(current->point);
                    }
                    
                    // 标记为已访问
                    current->visited = true;
                    current->neighbor->visited = true;
                    
                    // 检查是否回到起点
                    if (current == start || current->neighbor == start) {
                        break;
                    }
                    
                    // 切换到另一个多边形
                    // 在主多边形上遇到出点(isEntry=false)时，切换到裁剪窗口
                    // 在裁剪窗口上时，遇到交点切换回主多边形
                    current = current->neighbor;
                    onSubjectPolygon = !onSubjectPolygon;
                }
            }
            
            if (polygon.size() >= 3) {
                resultPolygons.push_back(polygon);
            }
        }
    }
    return resultPolygons;
}

void ClippingAlgorithms::CleanupVertexList(std::vector<WAVertex*>& vertexList) {
    // 只删除非交点顶点，因为交点顶点会被两个列表引用
    // 交点的删除由 polyList 负责
    for (WAVertex* v : vertexList) {
        if (v) {
            delete v;
        }
    }
    vertexList.clear();
}

std::vector<std::vector<Point2D>> ClippingAlgorithms::ClipPolygonWeilerAtherton(
    const std::vector<Point2D>& polygon, int xmin, int ymin, int xmax, int ymax) {
    std::vector<std::vector<Point2D>> result;
    
    if (polygon.size() < 3) {
        return result;
    }
    
    // Check if all vertices are inside the clipping window
    bool allInside = true;
    bool allOutside = true;
    for (const Point2D& p : polygon) {
        if (IsPointInsideWindow(p, xmin, ymin, xmax, ymax)) {
            allOutside = false;
        } else {
            allInside = false;
        }
    }
    
    // If all vertices are inside, return the original polygon
    if (allInside) {
        result.push_back(polygon);
        return result;
    }
    
    std::vector<WAVertex*> polyList = BuildPolygonVertexList(polygon);
    std::vector<WAVertex*> clipList = BuildClipWindowVertexList(xmin, ymin, xmax, ymax);
    
    InsertIntersections(polyList, clipList, xmin, ymin, xmax, ymax);
    
    // If no intersections were found and all vertices are outside, polygon is outside
    bool hasIntersections = false;
    for (WAVertex* v : polyList) {
        if (v->isIntersection) {
            hasIntersections = true;
            break;
        }
    }
    
    if (!hasIntersections) {
        // No intersections and not all inside means completely outside
        // 先清理 clipList 中非交点的顶点
        for (WAVertex* v : clipList) {
            if (!v->isIntersection) {
                delete v;
            }
        }
        clipList.clear();
        // 再清理 polyList
        for (WAVertex* v : polyList) {
            delete v;
        }
        polyList.clear();
        return result; // Return empty
    }
    
    MarkEntryExit(polyList, xmin, ymin, xmax, ymax);
    result = TraceClippedPolygons(polyList);
    
    // 清理内存：先收集所有需要删除的顶点
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
