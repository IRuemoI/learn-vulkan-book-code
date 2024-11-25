#pragma once
#include "Animation.h"
#include "VertexDataForDraw.h"
class Bnggdh {
private:
    VertexDataForDraw *vdfd;
    Animation *mAnimation;
    float maxKeytime;
    string sourceName;

public:
    int getVerNums();
    int getIndexNums();
    float *getMatrix(string id);
    float getMaxKeytime();
    float *getTextures();
    short *getIndices();
    float *getPosition();
    void updata(float time);
    void init();
    Bnggdh(string sourceName);
    ~Bnggdh();
};