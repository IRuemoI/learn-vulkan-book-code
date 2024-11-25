#pragma once
#include "BodyPart.h"
#include <cfloat>
#include <math.h>
using namespace std;
class Robot {
public:
    BodyPart
            *bRoot,
            *bBody, *bHead,
            *bLeftTop, *bLeftBottom, *bRightTop, *bRightBottom,
            *bRightLegTop, *bRightLegBottom, *bLeftLegTop, *bLeftLegBottom,
            *bLeftFoot, *bRightFoot;
    std::vector<BodyPart *> bpVector;
    float lowest = FLT_MAX;
    Robot(vector<ObjObject *> objObject);
    ~Robot();
    void updateState();
    void backToInit();
    void calLowest();
    void drawSelf(VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer);
};
