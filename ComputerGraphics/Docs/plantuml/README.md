# PlantUML 图表集合

本目录包含计算机图形学实验报告中所有的PlantUML图表代码。

## 文件列表

### 系统设计图

1. **menu-structure.puml** - 菜单结构图
   - 展示程序的菜单层次结构
   - 包含所有菜单项和快捷键

2. **system-architecture.puml** - 系统架构图
   - 展示系统的模块划分
   - 显示模块之间的调用关系

3. **class-structure.puml** - 类结构图
   - GraphicsEngine核心类设计
   - DrawMode枚举和Point2D结构体

### 算法流程图

4. **algorithm-dda-line.puml** - DDA直线算法流程图
   - 数字微分分析器算法
   - 浮点增量计算

5. **algorithm-bresenham-line.puml** - Bresenham直线算法流程图
   - 整数误差累积算法
   - 高效直线绘制

6. **algorithm-midpoint-circle.puml** - 中点圆算法流程图
   - 中点判别法
   - 8对称性优化

7. **algorithm-bresenham-circle.puml** - Bresenham圆算法流程图
   - Bresenham圆绘制
   - 整数运算优化

8. **algorithm-bspline.puml** - B样条曲线算法流程图
   - 参数曲线计算
   - 基函数求值

9. **algorithm-scanline-fill.puml** - 扫描线填充算法流程图
   - 多边形填充
   - 交点排序和配对

10. **algorithm-boundary-fill.puml** - 边界填充算法流程图
    - 种子填充法
    - 栈式扩散算法

## 使用方法

### 在线渲染
访问 [PlantUML Online](http://www.plantuml.com/plantuml/uml/) 并粘贴代码

### 本地渲染
```bash
# 安装PlantUML
brew install plantuml  # macOS
# 或下载jar文件

# 渲染单个文件
plantuml menu-structure.puml

# 渲染所有文件
plantuml *.puml
```

### VS Code插件
安装 "PlantUML" 插件，可以实时预览

### IDEA/PyCharm插件
安装 "PlantUML integration" 插件

## 输出格式

PlantUML支持多种输出格式：
- PNG (默认)
- SVG (矢量图)
- PDF
- LaTeX

```bash
# 生成SVG格式
plantuml -tsvg *.puml

# 生成PDF格式
plantuml -tpdf *.puml
```

## 图表说明

所有图表均采用中文标注，符合实验报告要求。图表清晰展示了：
- 系统的整体架构和模块划分
- 核心类的设计和关系
- 各算法的详细执行流程
- 决策点和循环结构

## 注意事项

- 确保使用支持中文的字体
- 某些在线工具可能不支持中文，建议使用本地渲染
- 流程图使用活动图语法，结构图使用组件图和类图语法
