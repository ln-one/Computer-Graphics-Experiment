# View Zoom Test Instructions (Task 15)

## 视角缩放测试说明

This document provides instructions for testing the view zoom functionality in View Control mode.

## Prerequisites

1. Build and run the ComputerGraphics application
2. Switch to 3D mode: Menu → 模式(Mode) → 3D模式(3D Mode)

## Test Steps

### Test 1: View Zoom in View Control Mode

1. Create some 3D shapes:
   - Menu → 3D图形(3D Shapes) → 立方体(Cube)
   - Click on the canvas to place a cube
   - Repeat to add more shapes if desired

2. Enter View Control mode:
   - Menu → 3D控制(3D Control) → 视角控制(View Control)

3. Test zoom functionality:
   - Scroll mouse wheel UP → Scene should zoom IN (camera gets closer)
   - Scroll mouse wheel DOWN → Scene should zoom OUT (camera gets farther)

4. Expected behavior:
   - Camera distance should change smoothly
   - Minimum distance: 1.0 units
   - Maximum distance: 50.0 units
   - Debug output should show: "View zoom: delta=X, camera distance=Y.YY"

### Test 2: Ctrl + Wheel Zoom (Any Mode)

1. While in any 3D mode (e.g., Cube drawing mode):
   - Hold Ctrl key
   - Scroll mouse wheel

2. Expected behavior:
   - View should zoom in/out even when not in View Control mode
   - This provides a convenient shortcut for zooming

### Test 3: Combined View Control

1. In View Control mode:
   - Drag mouse to rotate the view
   - Scroll wheel to zoom in/out

2. Expected behavior:
   - Both rotation and zoom should work together
   - Scene should update in real-time

## Requirements Verified

- **Requirement 4.3**: WHEN 处于视角控制模式 AND 用户滚动鼠标滚轮 THEN 系统 SHALL 调整摄像机与场景的距离（缩放）
- **Requirement 4.4**: WHEN 视角改变 THEN 系统 SHALL 实时更新场景渲染以反映新视角

## Notes

- The zoom speed is set to 0.5 units per wheel notch (120 delta)
- Camera distance is clamped between 1.0 and 50.0 units to prevent extreme zoom levels
- Debug messages are output to help verify the functionality during development
