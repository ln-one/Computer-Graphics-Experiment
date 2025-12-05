# PlantUML 图表说明

本目录包含实验三报告所需的所有PlantUML图表源代码。

## 图表列表

### 1. 系统架构图
- **文件**: `system-architecture.puml`
- **说明**: 展示系统的分层架构，包括UI层、引擎层、管理层、算法层和数据层
- **用途**: 第五章 - 功能模块结构及说明

### 2. 类结构图
- **文件**: `class-structure.puml` (详细版，横向)
- **文件**: `class-relationships.puml` (简洁版，竖向，推荐)
- **文件**: `shape-hierarchy.puml` (图形类继承层次，竖向)
- **说明**: 展示主要类的继承关系和依赖关系
- **用途**: 第五章 - 功能模块结构及说明
- **推荐**: 使用 `class-relationships.puml` 和 `shape-hierarchy.puml`

### 3. 菜单结构图
- **文件**: `menu-structure.puml` / `menu-tree.puml` / `menu-wbs.puml`
- **说明**: 三种不同风格的菜单结构展示
  - `menu-structure.puml`: 卡片式布局
  - `menu-tree.puml`: 思维导图式
  - `menu-wbs.puml`: WBS工作分解结构式（推荐）
- **用途**: 第四章 - 界面设计及交互方式

### 4. Cohen-Sutherland算法流程图
- **文件**: `cohen-sutherland-flowchart.puml`
- **说明**: Cohen-Sutherland直线裁剪算法的详细流程
- **用途**: 第六章 - 算法流程图及说明

### 5. 中点分割算法流程图
- **文件**: `midpoint-subdivision-flowchart.puml`
- **说明**: 中点分割直线裁剪算法的递归流程
- **用途**: 第六章 - 算法流程图及说明

### 6. Sutherland-Hodgman算法流程图
- **文件**: `sutherland-hodgman-flowchart.puml`
- **说明**: Sutherland-Hodgman多边形裁剪算法的流程
- **用途**: 第六章 - 算法流程图及说明

### 7. Weiler-Atherton算法流程图
- **文件**: `weiler-atherton-flowchart.puml`
- **说明**: Weiler-Atherton多边形裁剪算法的复杂流程
- **用途**: 第六章 - 算法流程图及说明

### 8. 区域编码示意图
- **文件**: `region-codes.puml`
- **说明**: Cohen-Sutherland算法使用的9区域编码
- **用途**: 第六章 - 算法流程图及说明

### 9. 几何变换流程图
- **文件**: `transform-process.puml`
- **说明**: 平移、缩放、旋转三种变换的交互流程
- **用途**: 第六章 - 算法流程图及说明

## 如何使用

### 方法1：在线渲染（推荐）

1. 访问 [PlantUML Online Server](http://www.plantuml.com/plantuml/uml/)
2. 复制 `.puml` 文件内容
3. 粘贴到在线编辑器
4. 点击"Submit"生成图片
5. 右键保存图片

### 方法2：VS Code插件

1. 安装 VS Code 插件：`PlantUML`
2. 打开 `.puml` 文件
3. 按 `Alt+D` 预览图表
4. 右键选择"Export Current Diagram"导出图片

### 方法3：命令行工具

```bash
# 安装 PlantUML
# 需要先安装 Java

# 生成PNG图片
java -jar plantuml.jar diagram.puml

# 生成SVG图片（矢量图，推荐用于报告）
java -jar plantuml.jar -tsvg diagram.puml
```

### 方法4：Markdown直接引用

在支持PlantUML的Markdown编辑器中（如Typora、VS Code with插件），可以直接引用：

```markdown
![系统架构图](diagrams/system-architecture.puml)
```

## 图表布局说明

所有流程图和架构图都已调整为**竖向布局**，更适合A4纸张的文档排版：

- ✅ 系统架构图：竖向分层
- ✅ 类关系图：竖向依赖
- ✅ 菜单结构图：竖向展开
- ✅ 算法流程图：从上到下
- ✅ 区域编码图：竖向排列

## 图片格式建议

- **Word文档**: 使用PNG格式（300dpi）
- **PDF报告**: 使用SVG格式（矢量图，缩放不失真）
- **在线文档**: 使用PNG或SVG格式
- **打印报告**: 建议PNG 600dpi，确保清晰度

## 自定义样式

所有图表都使用了统一的配色方案：
- 绿色：文件操作
- 蓝色：绘图功能
- 青色：填充功能
- 黄色：变换功能（实验三新增）
- 粉色：裁剪功能（实验三新增）
- 灰色：帮助功能

如需修改样式，请编辑各 `.puml` 文件开头的 `skinparam` 部分。

## 注意事项

1. 确保使用UTF-8编码保存文件，以正确显示中文
2. 某些复杂图表可能需要较长时间渲染
3. 如果图表显示不正确，检查PlantUML语法是否有误
4. 建议导出为高分辨率图片（至少300dpi）用于打印

## 更新日志

- 2025-12-05: 创建所有实验三所需的图表
  - 系统架构图
  - 类结构图
  - 菜单结构图（3种风格）
  - 4种裁剪算法流程图
  - 区域编码示意图
  - 几何变换流程图
