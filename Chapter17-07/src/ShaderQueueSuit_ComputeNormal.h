#ifndef VULKANEXBASE_ShaderQueueSuit_ComputeB_H
#define VULKANEXBASE_ShaderQueueSuit_ComputeB_H

#include <vector>
#include <vulkan/vulkan.h>


class ShaderQueueSuit_ComputeNormal {
private:
    std::vector<VkDescriptorSetLayout> descLayouts;

    VkPipelineShaderStageCreateInfo shaderStages[1];

    VkPipelineCache pipelineCache;

    VkDevice *devicePointer;

    VkDescriptorPool descPool;

    void create_pipeline_layout(VkDevice &device);
    void destroy_pipeline_layout(VkDevice &device);
    void init_descriptor_set(VkDevice &device);
    void create_shader(VkDevice &device);
    void destroy_shader(VkDevice &device);
    void create_pipe_line(VkDevice &device);
    void destroy_pipe_line(VkDevice &device);

public:
    VkWriteDescriptorSet writes[1];

    std::vector<VkDescriptorSet> descSet;

    VkPipelineLayout pipelineLayout;

    VkPipeline pipeline;

    ShaderQueueSuit_ComputeNormal(VkDevice *deviceIn, VkPhysicalDeviceMemoryProperties &memoryroperties);
    ~ShaderQueueSuit_ComputeNormal();
};

#endif
