# 外部库安装指南 / External Libraries Setup Guide

本文档提供详细的外部库下载和安装步骤。

## 1. GLAD (OpenGL Loader)

### 下载步骤：
1. 访问 GLAD 在线生成器：https://glad.dav1d.de/
2. 配置选项：
   - **Language**: C/C++
   - **Specification**: OpenGL
   - **API gl**: Version 3.3 (或更高)
   - **Profile**: Core
3. 点击 "Generate" 生成文件
4. 下载生成的 ZIP 文件

### 安装步骤：
1. 解压下载的 ZIP 文件
2. 将 `include/glad/` 文件夹复制到 `external/glad/`
3. 将 `include/KHR/` 文件夹复制到 `external/KHR/`
4. 创建 `external/src/` 文件夹（如果不存在）
5. 将 `src/glad.c` 文件复制到 `external/src/glad.c`

### 预期结构：
```
external/
├── glad/
│   └── glad.h
├── KHR/
│   └── khrplatform.h
└── src/
    └── glad.c
```

---

## 2. GLM (OpenGL Mathematics)

### 下载步骤：
1. 访问 GLM GitHub 发布页：https://github.com/g-truc/glm/releases
2. 下载最新版本的源代码 ZIP 文件（例如：glm-0.9.9.8.zip）

### 安装步骤：
1. 解压下载的 ZIP 文件
2. 在解压的文件夹中找到 `glm/` 子文件夹
3. 将整个 `glm/` 文件夹复制到 `external/glm/`

### 预期结构：
```
external/
└── glm/
    └── glm/
        ├── glm.hpp
        ├── ext/
        ├── gtc/
        ├── gtx/
        └── ... (其他头文件)
```

**注意**: GLM 是纯头文件库，不需要编译。

---

## 3. stb_image (Image Loader)

### 下载步骤：
1. 访问 stb GitHub 仓库：https://github.com/nothings/stb
2. 找到 `stb_image.h` 文件
3. 点击文件，然后点击 "Raw" 按钮
4. 右键保存页面为 `stb_image.h`

### 安装步骤：
1. 创建 `external/stb/` 文件夹（如果不存在）
2. 将下载的 `stb_image.h` 文件放入 `external/stb/`

### 预期结构：
```
external/
└── stb/
    └── stb_image.h
```

**注意**: stb_image 是单头文件库，使用时需要在一个 .cpp 文件中定义 `STB_IMAGE_IMPLEMENTATION`。

---

## 验证安装

安装完成后，`external/` 目录应该具有以下结构：

```
external/
├── README.md
├── SETUP_INSTRUCTIONS.md (本文件)
├── glad/
│   └── glad.h
├── KHR/
│   └── khrplatform.h
├── glm/
│   └── glm/
│       ├── glm.hpp
│       └── ... (其他文件)
├── stb/
│   └── stb_image.h
└── src/
    └── glad.c
```

## 常见问题

### Q: 找不到 glad.c 文件？
A: 确保在 GLAD 网站上点击了 "Generate" 并下载了完整的 ZIP 包。glad.c 在 ZIP 包的 `src/` 目录中。

### Q: GLM 编译错误？
A: 确保复制了完整的 `glm/glm/` 目录结构（注意有两层 glm 文件夹）。

### Q: 如何测试库是否正确安装？
A: 在 Visual Studio 中打开项目，尝试编译。如果没有 "找不到文件" 的错误，说明安装成功。

## 项目配置说明

项目已配置以下内容：
- **包含目录**: `external/`, `external/glad/`, `external/glm/`, `external/stb/`
- **链接库**: `opengl32.lib`
- **预处理器定义**: 已在项目中配置

无需手动修改项目设置，只需按照上述步骤安装库文件即可。
