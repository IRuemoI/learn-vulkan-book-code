#pragma once
#include "TextureManager.h"
#include <string>
#include <vulkan/vulkan.h>
class TexTrunkDrawableObject {
public:
    VkDevice *devicePointer;
    float *vdata;
    int vCount;
    float *pushConstantData;
    VkBuffer vertexDatabuf;
    VkDeviceMemory vertexDataMem;
    float centerx;
    float centery;
    float centerz;
    VkDescriptorBufferInfo vertexDataBufferInfo;
    TexTrunkDrawableObject(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties, float tx, float ty, float tz);
    ~TexTrunkDrawableObject();
    void drawSelf(VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer);
};