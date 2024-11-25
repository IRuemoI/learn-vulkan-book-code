#include "FlatData.h"
#include <vector>
float *FlatData::vdata;
int FlatData::dataByteCount;
int FlatData::vCount;
void FlatData::genVertexData() {
    int cols = 12;
    int rows = cols * 3 / 4;
    std::vector<float> alVertix;
    std::vector<float> alTexCoor;
    float widht = 12;
    float UNIT_SIZE = widht / cols;
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            float zsx = -UNIT_SIZE * cols / 2 + i * UNIT_SIZE;
            float zsy = UNIT_SIZE * rows / 2 - j * UNIT_SIZE;
            float zsz = 0;
            alVertix.push_back(zsx);
            alVertix.push_back(zsy);
            alVertix.push_back(zsz);
            alVertix.push_back(zsx);
            alVertix.push_back(zsy - UNIT_SIZE);
            alVertix.push_back(zsz);
            alVertix.push_back(zsx + UNIT_SIZE);
            alVertix.push_back(zsy);
            alVertix.push_back(zsz);
            alVertix.push_back(zsx + UNIT_SIZE);
            alVertix.push_back(zsy);
            alVertix.push_back(zsz);
            alVertix.push_back(zsx);
            alVertix.push_back(zsy - UNIT_SIZE);
            alVertix.push_back(zsz);
            alVertix.push_back(zsx + UNIT_SIZE);
            alVertix.push_back(zsy - UNIT_SIZE);
            alVertix.push_back(zsz);
        }
    }
    float sizew = 1.0f / cols;
    float sizeh = 1.0f / rows;
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            float s = i * sizew;
            float t = j * sizeh;
            alTexCoor.push_back(s);
            alTexCoor.push_back(t);
            alTexCoor.push_back(s);
            alTexCoor.push_back(t + sizeh);
            alTexCoor.push_back(s + sizew);
            alTexCoor.push_back(t);
            alTexCoor.push_back(s + sizew);
            alTexCoor.push_back(t);
            alTexCoor.push_back(s);
            alTexCoor.push_back(t + sizeh);
            alTexCoor.push_back(s + sizew);
            alTexCoor.push_back(t + sizeh);
        }
    }
    vCount = cols * rows * 6;
    dataByteCount = vCount * 5 * sizeof(float);
    vdata = new float[vCount * 5];
    int index = 0;
    for (int i = 0; i < vCount; i++) {
        vdata[index++] = alVertix[i * 3 + 0];
        vdata[index++] = alVertix[i * 3 + 1];
        vdata[index++] = alVertix[i * 3 + 2];
        vdata[index++] = alTexCoor[i * 2 + 0];
        vdata[index++] = alTexCoor[i * 2 + 1];
    }
}
