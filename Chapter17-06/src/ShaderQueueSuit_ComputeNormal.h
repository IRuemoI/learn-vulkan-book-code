#pragma once
#include <vector>
#include <vulkan/vulkan.h>


class ShaderQueueSuit_ComputeNormal {
private:
    std::vector<VkDescriptorSetLayout> descLayouts;

    VkPipelineShaderStageCreateInfo shaderStages[1];

    VkPipelineCache pipelineCache;

    VkDevice *devicePointer;

    VkDescriptorPool descPool;

    void createPipelineLayout(VkDevice &device);
    void destroyPipelineLayout(VkDevice &device);
    void initDescriptorSet(VkDevice &device);
    void createShaderModule(VkDevice &device);
    void destroyShaderModule(VkDevice &device);
    void createPipeline(VkDevice &device);
    void destroyPipeline(VkDevice &device);

public:
    VkWriteDescriptorSet writes[1];

    std::vector<VkDescriptorSet> descSet;

    VkPipelineLayout pipelineLayout;

    VkPipeline pipeline;

    ShaderQueueSuit_ComputeNormal(VkDevice *deviceIn, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~ShaderQueueSuit_ComputeNormal();
};