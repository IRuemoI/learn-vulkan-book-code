#pragma once
#include "Headers.h"
class VulkanRenderer;

class VulkanDrawable {
public:
    VulkanDrawable(VulkanRenderer *parent = 0);
    ~VulkanDrawable();

    void createVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture);
    void prepare();
    void render();
    void setPipeline(VkPipeline *vulkanPipeline) { pipeline = vulkanPipeline; }
    VkPipeline *getPipeline() { return pipeline; }
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

private:
    std::vector<VkCommandBuffer> vecCmdDraw;// Command buffer for drawing
    void recordCommandBuffer(int currentBuffer, VkCommandBuffer *cmdDraw);
    VulkanRenderer *rendererObj;
    VkPipeline *pipeline;
};