#pragma once
#include "Headers.h"
#include "VulkanDescriptor.h"
#include "Wrappers.h"

class VulkanRenderer;
class VulkanDrawable : public VulkanDescriptor {
public:
    VulkanDrawable(VulkanRenderer *parent = 0);
    ~VulkanDrawable();

    void createVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture);
    void prepare();
    void render();
    void update();

    void setPipeline(VkPipeline *vulkanPipeline) { pipeline = vulkanPipeline; }
    VkPipeline *getPipeline() { return pipeline; }

    void createUniformBuffer();
    void createDescriptorPool(bool useTexture);
    void createDescriptorResources();
    void createDescriptorSet(bool useTexture);
    void createDescriptorSetLayout(bool useTexture);
    void createPipelineLayout();

    void initViewports(VkCommandBuffer *cmd);
    void initScissors(VkCommandBuffer *cmd);

    void destroyVertexBuffer();
    void destroyCommandBuffer();
    void destroySynchronizationObjects();
    void destroyUniformBuffer();

public:
    struct
    {
        VkBuffer buffer;// Buffer resource object
        VkDeviceMemory memory;// Buffer resourece object's allocated device memory
        VkDescriptorBufferInfo bufferInfo;// Buffer info that need to supplied into write descriptor set (VkWriteDescriptorSet)
        VkMemoryRequirements memRqrmnt;// Store the queried memory requirement of the uniform buffer
        std::vector<VkMappedMemoryRange> mappedRange;// Metadata of memory mapped objects
        uint8_t *pData;// Host pointer containing the mapped device address which is used to write data into.
    } UniformData;

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
    void recordCommandBuffer(int currentImage, VkCommandBuffer *cmdDraw);
    VkViewport viewport;
    VkRect2D scissor;
    VkSemaphore presentCompleteSemaphore;
    VkSemaphore drawingCompleteSemaphore;

    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 Model;
    glm::mat4 MVP;

    VulkanRenderer *rendererObj;
    VkPipeline *pipeline;
};