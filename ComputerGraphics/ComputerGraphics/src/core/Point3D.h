#pragma once

// 三维点结构
struct Point3D {
    float x, y, z;
    
    Point3D() : x(0), y(0), z(0) {}
    Point3D(float x, float y, float z) : x(x), y(y), z(z) {}
};
