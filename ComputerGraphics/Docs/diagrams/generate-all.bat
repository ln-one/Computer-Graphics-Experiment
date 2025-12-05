@echo off
REM 生成所有PlantUML图表为PNG格式 (Windows版本)
REM 使用方法: generate-all.bat

echo 开始生成所有图表...
echo.

REM 检查Java是否安装
java -version >nul 2>&1
if errorlevel 1 (
    echo 错误: 未找到Java
    echo 请先安装Java: https://www.java.com/download/
    pause
    exit /b 1
)

REM 检查plantuml.jar是否存在
if not exist plantuml.jar (
    echo 错误: 未找到plantuml.jar
    echo 请下载plantuml.jar到当前目录
    echo 下载地址: https://plantuml.com/download
    pause
    exit /b 1
)

REM 创建输出目录
if not exist output mkdir output

REM 生成所有.puml文件为PNG
for %%f in (*.puml) do (
    echo 生成: %%f
    java -jar plantuml.jar -tpng -o output "%%f"
)

echo.
echo 完成! 所有图片已保存到 output\ 目录
echo.
dir /b output\*.png

echo.
set /p choice="是否同时生成SVG格式? (Y/N): "
if /i "%choice%"=="Y" (
    echo 生成SVG格式...
    for %%f in (*.puml) do (
        java -jar plantuml.jar -tsvg -o output "%%f"
    )
    echo SVG文件已生成!
)

pause
