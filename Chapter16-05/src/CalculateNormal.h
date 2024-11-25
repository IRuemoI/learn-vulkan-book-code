#pragma once
class CalculateNormal {
public:
    static void calNormal(float *mVectors, short *mIndices, int indicesCount, int verNum, float *nXYZ, float *vNormal);//计算顶点法向量数据的方法
    static void getCrossProduct(float x1, float y1, float z1, float x2, float y2, float z2, float *result);            //计算向量叉积的方法
    static void vectorNormal(float *vector);                                                                           //规格化向量的方法
};