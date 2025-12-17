#pragma once
#include "../core/Point2D.h"
#include <vector>

/**
 * @file ClippingAlgorithms.h
 * @brief 图形裁剪算法类定义
 * @author ln1.opensource@gmail.com
 */

/**
 * @class ClippingAlgorithms
 * @brief 图形裁剪算法实现类
 * 
 * 提供多种经典的图形裁剪算法，包括直线裁剪和多边形裁剪
 * 支持Cohen-Sutherland、中点分割、Sutherland-Hodgman和Weiler-Atherton算法
 */
class ClippingAlgorithms {
public:
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
     * 使用区域编码的方法快速判断和裁剪直线段
     */
    static bool ClipLineCohenSutherland(Point2D& p1, Point2D& p2, int xmin, int ymin, int xmax, int ymax);
    
    /**
     * @brief 中点分割直线裁剪算法
     * @param p1 直线起点
     * @param p2 直线终点
     * @param xmin 裁剪窗口左边界
     * @param ymin 裁剪窗口下边界
     * @param xmax 裁剪窗口右边界
     * @param ymax 裁剪窗口上边界
     * @param result 输出的裁剪后线段集合
     * 
     * 使用递归中点分割的方法进行直线裁剪
     */
    static void ClipLineMidpoint(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax,
                                 std::vector<std::pair<Point2D, Point2D>>& result);
    
    /**
     * @brief Sutherland-Hodgman多边形裁剪算法
     * @param polygon 待裁剪的多边形顶点序列
     * @param xmin 裁剪窗口左边界
     * @param ymin 裁剪窗口下边界
     * @param xmax 裁剪窗口右边界
     * @param ymax 裁剪窗口上边界
     * @return 裁剪后的多边形顶点序列
     * 
     * 逐边裁剪多边形，适用于凸多边形裁剪窗口
     */
    static std::vector<Point2D> ClipPolygonSutherlandHodgman(const std::vector<Point2D>& polygon,
                                                              int xmin, int ymin, int xmax, int ymax);
    
    /**
     * @brief Weiler-Atherton多边形裁剪算法
     * @param polygon 待裁剪的多边形顶点序列
     * @param xmin 裁剪窗口左边界
     * @param ymin 裁剪窗口下边界
     * @param xmax 裁剪窗口右边界
     * @param ymax 裁剪窗口上边界
     * @return 裁剪后的多边形集合（可能产生多个多边形）
     * 
     * 支持凹多边形裁剪，可能产生多个裁剪结果
     */
    static std::vector<std::vector<Point2D>> ClipPolygonWeilerAtherton(const std::vector<Point2D>& polygon,
                                                                        int xmin, int ymin, int xmax, int ymax);

private:
    /**
     * @struct WAVertex
     * @brief Weiler-Atherton算法的顶点结构
     * 
     * 用于构建多边形和裁剪窗口的顶点链表，支持交点标记和遍历
     */
    struct WAVertex {
        Point2D point;          ///< 顶点坐标
        bool isIntersection;    ///< 是否为交点
        bool isEntry;           ///< 是否为进入点（从外部进入裁剪窗口）
        WAVertex* next;         ///< 指向下一个顶点的指针
        WAVertex* neighbor;     ///< 指向对应交点的指针（用于在两个链表间跳转）
        bool visited;           ///< 遍历标记
        int id;                 ///< 顶点标识符
        
        /**
         * @brief 默认构造函数
         */
        WAVertex() : isIntersection(false), isEntry(false), next(nullptr), 
                     neighbor(nullptr), visited(false), id(0) {}
        
        /**
         * @brief 参数化构造函数
         * @param p 顶点坐标
         */
        WAVertex(Point2D p) : point(p), isIntersection(false), isEntry(false), 
                              next(nullptr), neighbor(nullptr), visited(false), id(0) {}
    };
    
    /**
     * @brief 计算两线段的交点
     * @param p1 第一条线段起点
     * @param p2 第一条线段终点
     * @param p3 第二条线段起点
     * @param p4 第二条线段终点
     * @param intersection 输出交点坐标
     * @param t1 第一条线段上交点的参数
     * @param t2 第二条线段上交点的参数
     * @return 如果线段相交返回true
     */
    static bool SegmentIntersection(Point2D p1, Point2D p2, Point2D p3, Point2D p4, 
                                    Point2D& intersection, double& t1, double& t2);
    
    /// @brief 构建多边形顶点链表
    static std::vector<WAVertex*> BuildPolygonVertexList(const std::vector<Point2D>& polygon);
    
    /// @brief 构建裁剪窗口顶点链表
    static std::vector<WAVertex*> BuildClipWindowVertexList(int xmin, int ymin, int xmax, int ymax);
    
    /// @brief 在顶点链表中插入交点
    static void InsertIntersections(std::vector<WAVertex*>& polyList, std::vector<WAVertex*>& clipList,
                                    int xmin, int ymin, int xmax, int ymax);
    
    /// @brief 标记交点的进入/退出属性
    static void MarkEntryExit(std::vector<WAVertex*>& polyList, int xmin, int ymin, int xmax, int ymax);
    
    /// @brief 追踪裁剪后的多边形
    static std::vector<std::vector<Point2D>> TraceClippedPolygons(std::vector<WAVertex*>& polyList,
                                                                   int xmin, int ymin, int xmax, int ymax);
    
    /// @brief 清理顶点链表内存
    static void CleanupVertexList(std::vector<WAVertex*>& vertexList);
    
    /// @brief 判断点是否在裁剪窗口内
    static bool IsPointInsideWindow(Point2D point, int xmin, int ymin, int xmax, int ymax);
    
private:
    /**
     * @enum OutCode
     * @brief Cohen-Sutherland算法的区域编码
     */
    enum OutCode { 
        INSIDE = 0,  ///< 内部区域
        LEFT = 1,    ///< 左侧区域
        RIGHT = 2,   ///< 右侧区域
        BOTTOM = 4,  ///< 下方区域
        TOP = 8      ///< 上方区域
    };
    
    /**
     * @enum ClipEdge
     * @brief 裁剪边枚举
     */
    enum ClipEdge { 
        CLIP_LEFT,    ///< 左边界
        CLIP_RIGHT,   ///< 右边界
        CLIP_BOTTOM,  ///< 下边界
        CLIP_TOP      ///< 上边界
    };
    
    /// @brief 计算点的区域编码
    static int ComputeOutCode(Point2D point, int xmin, int ymin, int xmax, int ymax);
    
    /// @brief 判断点是否在窗口内部
    static bool IsInsideWindow(Point2D point, int xmin, int ymin, int xmax, int ymax);
    
    /// @brief 判断两点是否在窗口同一侧外部
    static bool IsOutsideSameSide(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax);
    
    /// @brief 中点分割递归函数
    static void ClipLineMidpointRecursive(Point2D p1, Point2D p2, int xmin, int ymin, int xmax, int ymax,
                                          std::vector<std::pair<Point2D, Point2D>>& result, int depth);
    
    /// @brief 判断点是否在指定边的内侧
    static bool IsInsideEdge(Point2D point, ClipEdge edge, int xmin, int ymin, int xmax, int ymax);
    
    /// @brief 计算直线与裁剪边的交点
    static Point2D ComputeIntersection(Point2D p1, Point2D p2, ClipEdge edge, int xmin, int ymin, int xmax, int ymax);
    
    /// @brief 用指定边裁剪多边形
    static std::vector<Point2D> ClipPolygonAgainstEdge(const std::vector<Point2D>& polygon, ClipEdge edge,
                                                        int xmin, int ymin, int xmax, int ymax);
};
