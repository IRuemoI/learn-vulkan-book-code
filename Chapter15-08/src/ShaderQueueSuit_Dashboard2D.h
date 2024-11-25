#pragma once
#include <vector>
#include <vulkan/vulkan.h>
class ShaderQueueSuit_Dashboard2D {
private:
    std::vector<VkDescriptorSetLayout> descLayouts;
    VkPipelineShaderStageCreateInfo shaderStages[2];
    VkVertexInputBindingDescription vertexBinding;
    VkVertexInputAttributeDescription vertexAttributes[2];
    VkPipelineCache pipelineCache;
    VkDevice *devicePointer;
    VkDescriptorPool descPool;
    void createPipelineLayout(VkDevice &device);
    void destroyPipelineLayout(VkDevice &device);
    void initDescriptorSet(VkDevice &device);
    void createShaderModule(VkDevice &device);
    void destroyShaderModule(VkDevice &device);
    void initVertexAttributeInfo();
    void createPipeline(VkDevice &device, VkRenderPass &renderPass);
    void destroyPipeline(VkDevice &device);

public:
    VkWriteDescriptorSet writes[4];
    std::vector<VkDescriptorSet> descSet;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    ShaderQueueSuit_Dashboard2D(VkDevice *deviceIn, VkRenderPass &renderPass, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~ShaderQueueSuit_Dashboard2D();
};