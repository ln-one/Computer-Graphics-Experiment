#pragma once

/**
 * @file DrawMode.h
 * @brief 绘图模式枚举定义
 * @author 计算机图形学项目组
 */

/**
 * @enum DrawMode
 * @brief 绘图模式枚举
 * 
 * 定义了系统支持的所有绘图和操作模式，包括2D和3D功能
 * 用于控制用户界面的交互行为和图形绘制算法的选择
 */
enum DrawMode {
    // === 2D 基础模式 ===
    MODE_NONE = 0,                    ///< 无操作模式（默认状态）
    MODE_EXPR1,                       ///< 实验模式1
    
    // === 2D 直线绘制算法 ===
    MODE_LINE_DDA,                    ///< DDA直线绘制算法
    MODE_LINE_BRESENHAM,              ///< Bresenham直线绘制算法
    
    // === 2D 圆形绘制算法 ===
    MODE_CIRCLE_MIDPOINT,             ///< 中点圆绘制算法
    MODE_CIRCLE_BRESENHAM,            ///< Bresenham圆绘制算法
    
    // === 2D 基本图形 ===
    MODE_RECTANGLE,                   ///< 矩形绘制模式
    MODE_POLYLINE,                    ///< 折线绘制模式
    MODE_POLYGON,                     ///< 多边形绘制模式
    MODE_BSPLINE,                     ///< B样条曲线绘制模式
    
    // === 2D 填充算法 ===
    MODE_FILL_SCANLINE,               ///< 扫描线填充算法
    MODE_FILL_BOUNDARY,               ///< 边界填充算法
    
    // === 2D 几何变换 ===
    MODE_SELECT,                      ///< 图形选择模式
    MODE_TRANSLATE,                   ///< 平移变换模式
    MODE_SCALE,                       ///< 缩放变换模式
    MODE_ROTATE,                      ///< 旋转变换模式
    
    // === 2D 裁剪算法 ===
    MODE_CLIP_COHEN_SUTHERLAND,       ///< Cohen-Sutherland直线裁剪算法
    MODE_CLIP_MIDPOINT,               ///< 中点分割直线裁剪算法
    MODE_CLIP_SUTHERLAND_HODGMAN,     ///< Sutherland-Hodgman多边形裁剪算法
    MODE_CLIP_WEILER_ATHERTON,        ///< Weiler-Atherton多边形裁剪算法
    
    // === 3D 基本图形 ===
    MODE_3D_SPHERE,                   ///< 3D球体绘制模式
    MODE_3D_CYLINDER,                 ///< 3D圆柱体绘制模式
    MODE_3D_PLANE,                    ///< 3D平面绘制模式
    MODE_3D_CUBE,                     ///< 3D立方体绘制模式
    
    // === 3D 交互操作 ===
    MODE_3D_SELECT,                   ///< 3D图形选择模式
    MODE_3D_VIEW_CONTROL              ///< 3D视角控制模式
};
