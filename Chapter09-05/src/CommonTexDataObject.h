#pragma once

class CommonTexDataObject {
public:
    int width;
    int height;
    int dataByteCount;
    unsigned char *data;
    CommonTexDataObject(int width, int height, unsigned char *data, int dataByteCount);
    ~CommonTexDataObject();
};