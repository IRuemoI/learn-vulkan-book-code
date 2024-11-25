#pragma once
#include <string>
#include <vulkan/vulkan.h>

class DrawableObjectCommonLight {
public:
    int indirectDrawCount;                      //间接绘制信息数据组的数量
    int drawCmdbufbytes;                        //间接绘制信息数据所占总字节数
    VkBuffer drawCmdbuf;                        //间接绘制信息数据缓冲
    VkDeviceMemory drawCmdMem;                  //间接绘制信息数据缓冲对应设备内存
    float *pushConstantData;                    //需推送的常量数据数组的首地址指针
    VkDevice *devicePointer;                    //指向逻辑设备的指针
    float *vdata;                               //顶点数据数组首地址指针
    int vCount;                                 //顶点数量
    VkBuffer vertexDatabuf;                     //顶点数据缓冲
    VkDeviceMemory vertexDataMem;               //顶点数据所需设备内存
    VkDescriptorBufferInfo vertexDataBufferInfo;//顶点数据缓冲描述信息

    DrawableObjectCommonLight(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);//构造函数
    ~DrawableObjectCommonLight();                                                                                                                    //析构函数

    void drawSelf(VkCommandBuffer &secondary_cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer);
    void initDrawCmdbuf(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);//用于创建间接绘制信息数据缓冲的方法//绘制方法
};
