#pragma once
#include <utility>
class AABB {
public:
    float minX;
    float maxX;
    float minY;//X，Y，Z 轴坐标最小值
    float maxY;
    float minZ;
    float maxZ;//X，Y，Z 轴坐标最大值

    AABB(float minXIn, float maxXIn, float minYIn, float maxYIn, float minZIn, float maxZIn);//构造函数
    ~AABB();                                                                                 //析构函数
};