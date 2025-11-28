#pragma once
#include "../core/Point2D.h"
#include <vector>

class ClippingAlgorithms {
public:
    // Cohen-Sutherland
    static bool ClipLineCohenSutherland(Point2D& p1, Point2D& p2, int xmin, int ymin, int xmax, int ymax);
    
    // Midpoint Subdivision
    static void ClipLineMidpoint(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax,
                                 std::vector<std::pair<Point2D, Point2D>>& result);
    
    // Sutherland-Hodgman
    static std::vector<Point2D> ClipPolygonSutherlandHodgman(const std::vector<Point2D>& polygon,
                                                              int xmin, int ymin, int xmax, int ymax);

private:
    enum OutCode { INSIDE = 0, LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8 };
    enum ClipEdge { CLIP_LEFT, CLIP_RIGHT, CLIP_BOTTOM, CLIP_TOP };
    
    static int ComputeOutCode(Point2D point, int xmin, int ymin, int xmax, int ymax);
    static bool IsInsideWindow(Point2D point, int xmin, int ymin, int xmax, int ymax);
    static bool IsOutsideSameSide(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax);
    static void ClipLineMidpointRecursive(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax,
                                          std::vector<std::pair<Point2D, Point2D>>& result, int depth);
    static bool IsInsideEdge(Point2D point, ClipEdge edge, int xmin, int ymin, int xmax, int ymax);
    static Point2D ComputeIntersection(Point2D p1, Point2D p2, ClipEdge edge, int xmin, int ymin, int xmax, int ymax);
    static std::vector<Point2D> ClipPolygonAgainstEdge(const std::vector<Point2D>& polygon, ClipEdge edge,
                                                        int xmin, int ymin, int xmax, int ymax);
};
