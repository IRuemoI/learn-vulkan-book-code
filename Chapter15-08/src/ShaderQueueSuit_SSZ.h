#pragma once
#include <vector>
#include <vulkan/vulkan.h>
class ShaderQueueSuit_SSZ {
private:
    VkBuffer uniformBuf;
    VkDescriptorBufferInfo uniformBufferInfo;
    std::vector<VkDescriptorSetLayout> descLayouts;
    VkPipelineShaderStageCreateInfo shaderStages[2];
    VkVertexInputBindingDescription vertexBinding;
    VkVertexInputAttributeDescription vertexAttributes[2];
    VkPipelineCache pipelineCache;
    VkDevice *devicePointer;
    VkDescriptorPool descPool;
    void createUniformBuffer(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    void destroyUniformBuffer(VkDevice &device);
    void createPipelineLayout(VkDevice &device);
    void destroyPipelineLayout(VkDevice &device);
    void initDescriptorSet(VkDevice &device);
    void createShaderModule(VkDevice &device);
    void destroyShaderModule(VkDevice &device);
    void initVertexAttributeInfo();
    void createPipeline(VkDevice &device, VkRenderPass &renderPass);
    void destroyPipeline(VkDevice &device);

public:
    int bufferByteCount;
    VkDeviceMemory memUniformBuf;
    VkWriteDescriptorSet writes[2];
    std::vector<VkDescriptorSet> descSet;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    ShaderQueueSuit_SSZ(VkDevice *deviceIn, VkRenderPass &renderPass, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~ShaderQueueSuit_SSZ();
};