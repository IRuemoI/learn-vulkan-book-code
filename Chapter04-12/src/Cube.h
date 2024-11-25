#pragma once
#include "ColorRect.h"
#include "DrawableObjectCommon.h"

#include "ColorRect.h"
#include "DrawableObjectCommon.h"

class Cube {
public:
    DrawableObjectCommon *colorRect;
    float unit_size;

    void drawSelf(VkCommandBuffer cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer);
    Cube(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties, float *vdata, float unit_sizeIn);
    ~Cube();
};