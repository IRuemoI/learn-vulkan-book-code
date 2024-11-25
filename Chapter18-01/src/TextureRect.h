#pragma once
class TextureRect {
public:
    TextureRect(float width, float height);
    float *vdata;
    int dataByteCount;
    int vCount;
    ~TextureRect();
};