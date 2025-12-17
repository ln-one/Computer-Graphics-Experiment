#pragma once

/**
 * @file MenuIDs.h
 * @brief 菜单项ID定义
 * @author ln1.opensource@gmail.com
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

// === 3D对话框ID ===
#define IDD_TRANSFORM3D 70001                ///< 3D变换对话框

// === 3D变换对话框控件ID ===
// 位置输入框
#define IDC_EDIT_POS_X 70101                 ///< 位置X输入框
#define IDC_EDIT_POS_Y 70102                 ///< 位置Y输入框
#define IDC_EDIT_POS_Z 70103                 ///< 位置Z输入框

// 旋转输入框
#define IDC_EDIT_ROT_X 70104                 ///< 旋转X输入框
#define IDC_EDIT_ROT_Y 70105                 ///< 旋转Y输入框
#define IDC_EDIT_ROT_Z 70106                 ///< 旋转Z输入框

// 缩放输入框
#define IDC_EDIT_SCALE_X 70107               ///< 缩放X输入框
#define IDC_EDIT_SCALE_Y 70108               ///< 缩放Y输入框
#define IDC_EDIT_SCALE_Z 70109               ///< 缩放Z输入框

// 分组框
#define IDC_GROUP_POSITION 70201             ///< 位置分组框
#define IDC_GROUP_ROTATION 70202             ///< 旋转分组框
#define IDC_GROUP_SCALE 70203                ///< 缩放分组框

// === 3D光照对话框ID ===
#define IDD_LIGHTING3D 70002                 ///< 3D光照设置对话框

// === 3D光照对话框控件ID ===
// 光源位置输入框
#define IDC_EDIT_LIGHT_POS_X 70301           ///< 光源位置X输入框
#define IDC_EDIT_LIGHT_POS_Y 70302           ///< 光源位置Y输入框
#define IDC_EDIT_LIGHT_POS_Z 70303           ///< 光源位置Z输入框

// 光照强度输入框
#define IDC_EDIT_AMBIENT 70304               ///< 环境光强度输入框
#define IDC_EDIT_DIFFUSE 70305               ///< 漫反射强度输入框
#define IDC_EDIT_SPECULAR 70306              ///< 镜面反射强度输入框

// 光源颜色输入框
#define IDC_EDIT_LIGHT_R 70307               ///< 光源颜色R输入框
#define IDC_EDIT_LIGHT_G 70308               ///< 光源颜色G输入框
#define IDC_EDIT_LIGHT_B 70309               ///< 光源颜色B输入框

// 光照对话框分组框
#define IDC_GROUP_LIGHT_POSITION 70401       ///< 光源位置分组框
#define IDC_GROUP_LIGHT_INTENSITY 70402      ///< 光照强度分组框
#define IDC_GROUP_LIGHT_COLOR 70403          ///< 光源颜色分组框

// === 3D材质对话框ID ===
#define IDD_MATERIAL3D 70003                 ///< 3D材质编辑对话框

// === 3D材质对话框控件ID ===
// 环境光反射系数输入框
#define IDC_EDIT_MAT_AMBIENT_R 70501         ///< 环境光R输入框
#define IDC_EDIT_MAT_AMBIENT_G 70502         ///< 环境光G输入框
#define IDC_EDIT_MAT_AMBIENT_B 70503         ///< 环境光B输入框

// 漫反射系数输入框
#define IDC_EDIT_MAT_DIFFUSE_R 70504         ///< 漫反射R输入框
#define IDC_EDIT_MAT_DIFFUSE_G 70505         ///< 漫反射G输入框
#define IDC_EDIT_MAT_DIFFUSE_B 70506         ///< 漫反射B输入框

// 镜面反射系数输入框
#define IDC_EDIT_MAT_SPECULAR_R 70507        ///< 镜面反射R输入框
#define IDC_EDIT_MAT_SPECULAR_G 70508        ///< 镜面反射G输入框
#define IDC_EDIT_MAT_SPECULAR_B 70509        ///< 镜面反射B输入框

// 光泽度输入框
#define IDC_EDIT_MAT_SHININESS 70510         ///< 光泽度输入框

// 材质对话框分组框
#define IDC_GROUP_MAT_AMBIENT 70601          ///< 环境光分组框
#define IDC_GROUP_MAT_DIFFUSE 70602          ///< 漫反射分组框
#define IDC_GROUP_MAT_SPECULAR 70603         ///< 镜面反射分组框
#define IDC_GROUP_MAT_SHININESS 70604        ///< 光泽度分组框

// 材质按钮（在变换对话框中）
#define IDC_BTN_MATERIAL 70701               ///< 材质编辑按钮
#define IDC_BTN_TEXTURE 70702                ///< 纹理设置按钮

// === 3D纹理对话框ID ===
#define IDD_TEXTURE3D 70004                  ///< 3D纹理设置对话框

// === 3D纹理对话框控件ID ===
#define IDC_EDIT_TEXTURE_PATH 70801          ///< 纹理文件路径输入框
#define IDC_BTN_BROWSE_TEXTURE 70802         ///< 浏览纹理文件按钮
#define IDC_COMBO_MAPPING_TYPE 70803         ///< 纹理映射方式下拉框
#define IDC_EDIT_TEX_SCALE_U 70804           ///< 纹理缩放U输入框
#define IDC_EDIT_TEX_SCALE_V 70805           ///< 纹理缩放V输入框
#define IDC_EDIT_TEX_OFFSET_U 70806          ///< 纹理偏移U输入框
#define IDC_EDIT_TEX_OFFSET_V 70807          ///< 纹理偏移V输入框
#define IDC_BTN_REMOVE_TEXTURE 70808         ///< 移除纹理按钮
#define IDC_STATIC_TEXTURE_PREVIEW 70809     ///< 纹理预览静态控件

// 纹理对话框分组框
#define IDC_GROUP_TEXTURE_FILE 70901         ///< 纹理文件分组框
#define IDC_GROUP_TEXTURE_MAPPING 70902      ///< 纹理映射分组框
#define IDC_GROUP_TEXTURE_PARAMS 70903       ///< 纹理参数分组框
