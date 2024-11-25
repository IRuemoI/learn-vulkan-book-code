#pragma once
#include "Headers.h"
class VulkanRenderer;

class VulkanDrawable {
public:
    VulkanDrawable(VulkanRenderer *parent = 0);
    ~VulkanDrawable();

    void createVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture);
    void destroyVertexBuffer();

public:
    // 顶点缓存元数据结构体
    struct
    {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo bufferInfo;
    } VertexBuffer;

    // 保存定点输入速率
    VkVertexInputBindingDescription viIpBind;
    // 保存数据解释过程中有用的元数据
    VkVertexInputAttributeDescription viIpAttrb[2];
    VulkanRenderer *rendererObj;
};