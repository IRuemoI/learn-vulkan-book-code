#pragma once
#include "ColorRect.h"
#include "DrawableObjectCommon.h"

class Cube {
public:
    void drawSelf(VkCommandBuffer cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer);//绘制方法

    Cube(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);//构造函数
    ~Cube();//析构函数
};