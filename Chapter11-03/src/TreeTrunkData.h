#pragma once
class TreeTrunkData {
public:
    static float *vdata;
    static int dataByteCount;
    static int vCount;
    static void genVertexData(float bottom_radius, float joint_Height, int jointNum, int availableNum);//生成树的方法
};