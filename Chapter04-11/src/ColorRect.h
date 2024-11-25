#pragma once
#include <cstdint>

class ColorRect {
public:
    static float *vdata;
    static int dataByteCount;
    static int vCount;

    static void genVertexData();
};
