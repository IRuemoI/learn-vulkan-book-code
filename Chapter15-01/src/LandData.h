#pragma once
class LandData {
public:
    float *vData;
    int vCount;
    LandData(int width, int height, unsigned char *data);
};