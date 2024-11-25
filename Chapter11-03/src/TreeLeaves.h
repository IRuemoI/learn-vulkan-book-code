#pragma once
class TreeLeaves {
public:
    static float *vdata;
    static int dataByteCount;
    static int vCount;
    static void genVertexData(float width, float height, float absolute_height);//生成树叶的方法
};
