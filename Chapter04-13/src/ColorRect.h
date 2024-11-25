#pragma once
#include <cstdint>

class ColorRect {
public:
    static float *vdataG;//ÇàÂÌÉ«
    static float *vdataR;//·ÛºìÉ«
    static int dataByteCount;
    static int vCount;

    static void genVertexData();
};