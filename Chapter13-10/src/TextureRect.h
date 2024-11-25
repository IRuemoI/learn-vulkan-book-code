#pragma once
#include "TextureManager.h"
#include <string>
#include <vulkan/vulkan.h>
class TextureRect {
public:
    VkDevice *devicePointer;
    float *vdata;
    int vCount;
    float *pushConstantData;
    VkBuffer vertexDatabuf;
    VkDeviceMemory vertexDataMem;
    VkDescriptorBufferInfo vertexDataBufferInfo;
    TextureRect(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~TextureRect();
    void drawSelf(VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer);
};