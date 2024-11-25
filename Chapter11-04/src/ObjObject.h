#pragma once
#include <string>
#include <vulkan/vulkan.h>
class ObjObject {
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


    ObjObject(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~ObjObject();

    void drawSelf(VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer);
};