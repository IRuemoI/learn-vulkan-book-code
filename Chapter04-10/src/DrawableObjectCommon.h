#pragma once
#include <string>
#include <vulkan/vulkan.h>

class DrawableObjectCommonLight {
public:
    float *pushConstantData;//推送常量数据指针

    VkDevice *devicePointer;//指向逻辑设备的指针
    float *vdata;//顶点数据数组首地址指针
    int vCount;//顶点数量
    uint16_t *idata;//索引数据数组首地址指针
    int iCount;//索引数量
    VkBuffer vertexDatabuf;//顶点数据缓冲
    VkDeviceMemory vertexDataMem;//顶点数据所需设备内存
    VkDescriptorBufferInfo vertexDataBufferInfo;//顶点数据缓冲描述信息
    VkBuffer indexDatabuf;//索引数据缓冲
    VkDeviceMemory indexDataMem;//索引数据所需设备内存
    VkDescriptorBufferInfo indexDataBufferInfo;//索引数据缓冲描述信息
    void drawSelf(VkCommandBuffer &secondary_cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer, uint32_t sIndex, uint32_t eIndex);//绘制方法

    DrawableObjectCommonLight(float *vdataIn, int dataByteCount, int vCountIn, uint16_t *idataIn, int indexByteCount, int iCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);//构造函数
    ~DrawableObjectCommonLight();//析构函数
private:
    void createVertexBuffer(int dataByteCount, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);//创建顶点数据缓冲的方法
    void createIndexBuffer(int indexByteCount, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);//创建索引数据缓冲的方法
};
