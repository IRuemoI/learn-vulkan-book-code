#include "ObjectData.h"
#include <vector>

float *ObjectData::vdata;
int ObjectData::dataByteCount;
int ObjectData::vCount;


void ObjectData::genVertexData() {
    vCount = 5;
    dataByteCount = vCount * 6 * sizeof(float);
    vdata = new float[vCount * 6]{
            0, 0, 0, 1, 1, 0,
            60, 60, 0, 1, 1, 1,
            -60, 60, 0, 0, 1, 0,
            -60, -60, 0, 1, 1, 1,
            50, -60, 0, 1, 1, 0};
}