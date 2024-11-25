#pragma once
#include <string>
#include <vulkan/vulkan.h>
class ObjObject {
public:
    float *pushConstantData;
    VkDevice *devicePointer;
    float *vdata;
    int vCount;
    VkBuffer vertexDatabuf;
    VkDeviceMemory vertexDataMem;
    VkDescriptorBufferInfo vertexDataBufferInfo;
    ObjObject(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~ObjObject();
    void drawSelf(VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer);
};