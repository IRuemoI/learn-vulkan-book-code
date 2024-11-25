#pragma once

#include <string>
#include <vulkan/vulkan.h>
class ColorObject {
public:
    VkDevice *devicePointer;
    float *vdata;
    int vCount;
    float *pushConstantData;
    VkBuffer vertexDatabuf;
    VkDeviceMemory vertexDataMem;
    VkDescriptorBufferInfo vertexDataBufferInfo;
    ColorObject(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~ColorObject();
    void drawSelf(VkCommandBuffer &secondary_cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline);
};