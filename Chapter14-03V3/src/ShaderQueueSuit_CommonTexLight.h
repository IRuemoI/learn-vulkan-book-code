#pragma once
#include <vector>
#include <vulkan/vulkan.h>
class ShaderQueueSuit_CommonTexLight {
private:
    VkBuffer uniformBufVert;
    VkDescriptorBufferInfo uniformBufferInfoVert;
    VkBuffer uniformBufFrag;
    VkDescriptorBufferInfo uniformBufferInfoFrag;
    std::vector<VkDescriptorSetLayout> descLayouts;
    VkPipelineShaderStageCreateInfo shaderStages[2];
    VkVertexInputBindingDescription vertexBinding;
    VkVertexInputAttributeDescription vertexAttributes[3];
    VkPipelineCache pipelineCache;
    VkDevice *devicePointer;
    VkDescriptorPool descPool;
    void create_uniform_buffer_vert(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    void destroy_uniform_buffer_vert(VkDevice &device);
    void create_uniform_buffer_frag(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    void destroy_uniform_buffer_frag(VkDevice &device);
    void createPipelineLayout(VkDevice &device);
    void destroyPipelineLayout(VkDevice &device);
    void initDescriptorSet(VkDevice &device);
    void createShaderModule(VkDevice &device);
    void destroyShaderModule(VkDevice &device);
    void initVertexAttributeInfo();
    void createPipeline(VkDevice &device, VkRenderPass &renderPass);
    void destroyPipeline(VkDevice &device);

public:
    VkDeviceMemory memUniformBufVert;
    int bufferByteCountVert;
    VkDeviceMemory memUniformBufFrag;
    int bufferByteCountFrag;
    VkWriteDescriptorSet writes[4];
    std::vector<VkDescriptorSet> descSet;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    ShaderQueueSuit_CommonTexLight(VkDevice *deviceIn, VkRenderPass &renderPass, VkPhysicalDeviceMemoryProperties &memoryProperties);
    ~ShaderQueueSuit_CommonTexLight();
};