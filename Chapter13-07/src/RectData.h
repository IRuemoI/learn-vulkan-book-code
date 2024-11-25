#pragma once
#define SKY_BOX_HALF_EDGE 28
class RectData {
public:
    static float span;
    static float *vData;
    static int dataByteCount;
    static int vCount;
    static void getVertexData();
};