#pragma once
#define SPAN 4

#define CR 255

class VertData {
public:
    static float *vdata;

    static int dataByteCount;

    static int vCount;


    static int *indexData;

    static int indexDataByteCount;

    static int indexCount;

    static void genVertData();
};