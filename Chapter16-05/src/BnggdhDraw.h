#pragma once
#include "Bnggdh.h"
#include "HelpFunction.h"
#include "MatrixState3D.h"
#include <vulkan/vulkan.h>
class BnggdhDraw {
public:
    Bnggdh *bnggdh;
    float maxKeyTime;
    float *positionData;
    float *textureData;
    float *normalData;
    float *pushConstantData;
    VkDevice *devicePointer;
    VkPhysicalDeviceMemoryProperties *memoryPropertiesPointer;
    float *vDataTransfer;
    float *vdata;
    int vCount;
    int vdataByteCount;
    VkBuffer vertexDatabuf;
    VkDeviceMemory vertexDataMem;
    VkMemoryRequirements vmem_reqs;
    VkDescriptorBufferInfo vertexDataBufferInfo;
    uint16_t *idata;
    int iCount;
    int idataByteCount;
    VkBuffer indexDatabuf;
    VkDeviceMemory indexDataMem;
    VkMemoryRequirements imem_reqs;
    VkDescriptorBufferInfo indexDataBufferInfo;
    BnggdhDraw(string sourceName, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~BnggdhDraw();
    void initVertexData();
    void initIndexData();
    void initVertexBuffer();
    void initIndexBuffer();
    void referVertexBuffer();
    /*
	*@param time		动画时间
	*@param picName		纹理图资源路径名称字符串
	*/
    void drawSelf(string picName, VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline);
    void updateData(float time);
    void copyData();
};