#pragma once

/**
 * @file MenuIDs.h
 * @brief 菜单项ID定义
 * @author 计算机图形学项目组
 * 
 * 定义了应用程序中所有菜单项的唯一标识符
 * 按功能模块分组，便于管理和维护
 */

// === 文件操作菜单ID ===
#define ID_FILE_NEW 40001                    ///< 新建文件
#define ID_FILE_EXIT 40002                   ///< 退出程序

// === 实验功能菜单ID ===
#define ID_EXPR_EXPR1 40101                  ///< 实验功能1

// === 2D图形绘制菜单ID ===
// 直线绘制算法
#define ID_DRAW_LINE_DDA 40201               ///< DDA直线绘制算法
#define ID_DRAW_LINE_BRES 40202              ///< Bresenham直线绘制算法

// 圆形绘制算法
#define ID_DRAW_CIRCLE_MID 40203             ///< 中点圆绘制算法
#define ID_DRAW_CIRCLE_BRES 40204            ///< Bresenham圆绘制算法

// 基本图形
#define ID_DRAW_RECTANGLE 40205              ///< 矩形绘制
#define ID_DRAW_POLYLINE 40206               ///< 折线绘制
#define ID_DRAW_POLYGON 40208                ///< 多边形绘制
#define ID_DRAW_BSPLINE 40207                ///< B样条曲线绘制

// === 2D填充算法菜单ID ===
#define ID_FILL_SCANLINE 40301               ///< 扫描线填充算法
#define ID_FILL_BOUNDARY 40302               ///< 边界填充算法

// === 2D几何变换菜单ID ===
#define ID_TRANSFORM_SELECT 40501            ///< 图形选择模式
#define ID_TRANSFORM_TRANSLATE 40502         ///< 平移变换
#define ID_TRANSFORM_SCALE 40503             ///< 缩放变换
#define ID_TRANSFORM_ROTATE 40504            ///< 旋转变换

// === 2D裁剪算法菜单ID ===
#define ID_CLIP_COHEN_SUTHERLAND 40601       ///< Cohen-Sutherland直线裁剪
#define ID_CLIP_MIDPOINT 40602               ///< 中点分割直线裁剪
#define ID_CLIP_SUTHERLAND_HODGMAN 40603     ///< Sutherland-Hodgman多边形裁剪
#define ID_CLIP_WEILER_ATHERTON 40604        ///< Weiler-Atherton多边形裁剪

// === 帮助菜单ID ===
#define ID_HELP_ABOUT 40401                  ///< 关于对话框

// === 模式切换菜单ID ===
#define ID_MODE_2D 50001                     ///< 切换到2D模式
#define ID_MODE_3D 50002                     ///< 切换到3D模式

// === 3D图形绘制菜单ID ===
#define ID_3D_SPHERE 60001                   ///< 3D球体绘制
#define ID_3D_CYLINDER 60002                 ///< 3D圆柱体绘制
#define ID_3D_PLANE 60003                    ///< 3D平面绘制
#define ID_3D_CUBE 60004                     ///< 3D立方体绘制

// === 3D交互控制菜单ID ===
#define ID_3D_SELECT 61001                   ///< 3D图形选择模式
#define ID_3D_VIEW_CONTROL 61002             ///< 3D视角控制模式
#define ID_3D_LIGHTING 61003                 ///< 3D光照设置
