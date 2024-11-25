#pragma once
#include "TextureManager.h"
#include <vulkan/vulkan.h>

class Dashboard2DObject {
public:
    VkDevice *devicePointer;
    float *vdata;
    int vCount;
    float *pushConstantData;

    //顶点数据缓冲
    VkBuffer vertexDatabuf;
    //顶点数据内存
    VkDeviceMemory vertexDataMem;
    //顶点数据Buffer信息
    VkDescriptorBufferInfo vertexDataBufferInfo;


    Dashboard2DObject(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~Dashboard2DObject();

    void drawSelf(VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer);
};
