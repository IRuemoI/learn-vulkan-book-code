#pragma once

class TexDataObject {
public:
    int width;
    int height;
    unsigned char *data;
    int dataByteCount;
    TexDataObject(int width, int height, unsigned char *data, int dataByteCount);
    ~TexDataObject();
};