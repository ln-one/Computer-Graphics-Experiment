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
    
    // Weiler-Atherton
    static std::vector<std::vector<Point2D>> ClipPolygonWeilerAtherton(const std::vector<Point2D>& polygon,
                                                                        int xmin, int ymin, int xmax, int ymax);

private:
    // Weiler-Atherton helper structures
    struct WAVertex {
        Point2D point;
        bool isIntersection;
        bool isEntry;
        WAVertex* next;
        WAVertex* neighbor;
        bool visited;
        int id;
        
        WAVertex() : isIntersection(false), isEntry(false), next(nullptr), 
                     neighbor(nullptr), visited(false), id(0) {}
        WAVertex(Point2D p) : point(p), isIntersection(false), isEntry(false), 
                              next(nullptr), neighbor(nullptr), visited(false), id(0) {}
    };
    
    static bool SegmentIntersection(Point2D p1, Point2D p2, Point2D p3, Point2D p4, 
                                    Point2D& intersection, double& t1, double& t2);
    static std::vector<WAVertex*> BuildPolygonVertexList(const std::vector<Point2D>& polygon);
    static std::vector<WAVertex*> BuildClipWindowVertexList(int xmin, int ymin, int xmax, int ymax);
    static void InsertIntersections(std::vector<WAVertex*>& polyList, std::vector<WAVertex*>& clipList,
                                    int xmin, int ymin, int xmax, int ymax);
    static void MarkEntryExit(std::vector<WAVertex*>& polyList, int xmin, int ymin, int xmax, int ymax);
    static std::vector<std::vector<Point2D>> TraceClippedPolygons(std::vector<WAVertex*>& polyList);
    static void CleanupVertexList(std::vector<WAVertex*>& vertexList);
    static bool IsPointInsideWindow(Point2D point, int xmin, int ymin, int xmax, int ymax);
    
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
