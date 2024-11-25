#pragma once
#include "TextureManager.h"
#include <string>
#include <vulkan/vulkan.h>
class ParticleForDraw {
public:
    VkDevice *devicePointer;
    float *vdata;
    int vCount;
    float *pushConstantData;
    VkBuffer vertexDatabuf;
    VkDeviceMemory vertexDataMem;
    VkDescriptorBufferInfo vertexDataBufferInfo;
    ParticleForDraw(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~ParticleForDraw();
    void drawSelf(VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer, int id);
    void updateVertexData(float *vdataIn, int dataByteCount, VkDevice &device);
};
