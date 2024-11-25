#pragma once
#include <vector>
#include <vulkan/vulkan.h>


class ShaderQueueSuit_ComputeBD {
private:
    VkBuffer uniformBuf;
    VkDescriptorBufferInfo uniformBufferInfo;


    std::vector<VkDescriptorSetLayout> descLayouts;

    VkPipelineShaderStageCreateInfo shaderStages[1];

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
    void createPipeline(VkDevice &device);
    void destroyPipeline(VkDevice &device);

public:
    VkDeviceMemory memUniformBuf;
    int bufferByteCount;

    VkWriteDescriptorSet writes[3];

    std::vector<VkDescriptorSet> descSet;

    VkPipelineLayout pipelineLayout;

    VkPipeline pipeline;

    ShaderQueueSuit_ComputeBD(VkDevice *deviceIn, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~ShaderQueueSuit_ComputeBD();
};