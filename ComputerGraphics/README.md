# 计算机图形学实验

## 项目结构
```
ComputerGraphics/
├── src/
│   ├── core/           # 核心数据结构
│   ├── algorithms/     # 图形算法
│   ├── engine/         # 图形引擎
│   ├── ui/             # 界面定义
│   └── main.cpp        # 程序入口
└── *.vcxproj           # VS项目文件
```

## 编译运行
1. 用 Visual Studio 打开 `ComputerGraphics.sln`
2. 选择 x64 平台
3. 编译运行

## 功能
- 直线绘制（DDA、Bresenham）
- 圆绘制（中点、Bresenham）
- 矩形、多段线、多边形
- 边界填充、扫描线填充
