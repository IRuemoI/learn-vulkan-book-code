#pragma once
#include "TextureManager.h"
#include <string>
#include <vulkan/vulkan.h>
class DrawableObject {
public:
    VkDevice *devicePointer;
    float *vdata;
    int vCount;
    float *pushConstantData;
    VkBuffer vertexDatabuf;
    VkDeviceMemory vertexDataMem;
    VkDescriptorBufferInfo vertexDataBufferInfo;
    DrawableObject(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~DrawableObject();
    void drawSelf(VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer);
};