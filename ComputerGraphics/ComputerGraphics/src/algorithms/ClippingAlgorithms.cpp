#include "ClippingAlgorithms.h"
#include <cmath>

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
