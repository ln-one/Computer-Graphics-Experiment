#!/bin/bash
# 生成所有PlantUML图表为PNG格式
# 使用方法: ./generate-all.sh

echo "开始生成所有图表..."

# 检查是否安装了plantuml
if ! command -v plantuml &> /dev/null
then
    echo "错误: 未找到plantuml命令"
    echo "请先安装PlantUML: "
    echo "  macOS: brew install plantuml"
    echo "  或下载jar: https://plantuml.com/download"
    exit 1
fi

# 创建输出目录
mkdir -p output

# 生成所有.puml文件为PNG
for file in *.puml; do
    if [ -f "$file" ]; then
        echo "生成: $file"
        plantuml -tpng -o output "$file"
    fi
done

echo "完成! 所有图片已保存到 output/ 目录"
echo ""
echo "生成的图片列表:"
ls -lh output/*.png

# 可选：同时生成SVG格式（矢量图）
read -p "是否同时生成SVG格式? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]
then
    echo "生成SVG格式..."
    for file in *.puml; do
        if [ -f "$file" ]; then
            plantuml -tsvg -o output "$file"
        fi
    done
    echo "SVG文件已生成!"
fi
