#pragma once
#include "AABB.h"

using namespace std;

class SZTJCUtil {
private:
    static void calculateFrustumPlanes(float *mvp, float *g_frustumPlanes);//计算视锥体6 个面的方程
    static bool isPointInHalfSpace(float x, float y, float z, float a, float b, float c, float d);//判断指定点是否在指定半空间外
    static bool isAllOutHalfSpace(AABB *aabb, float a, float b, float c, float d);//判断包围盒是否完全在指定半空间外

public:
    static bool isAABBInSZT(float *mvpMatrix, AABB *aabb);//判断包围盒是否完全在视锥体外
};