#pragma once
#include <vector>
#include <vulkan/vulkan.h>

class ShaderQueueSuit_CommonTexLight
{
private:
    VkBuffer uniformBuf;
    VkDescriptorBufferInfo uniformBufferInfo;
    //DescriptorSetLayout列表
    std::vector<VkDescriptorSetLayout> descLayouts;
    //着色器阶段数组 长度为2 表示仅有 顶点着色器与片元着色器
    VkPipelineShaderStageCreateInfo shaderStages[2];
    //管线的顶点数据输入绑定描述--挂接着色器
    VkVertexInputBindingDescription vertexBinding;
    //管线的顶点数据输入属性描述--挂接着色器
    VkVertexInputAttributeDescription vertexAttributes[3];
    //管线缓冲
    VkPipelineCache pipelineCache;
    //设备
    VkDevice* devicePointer;
    //描述池
    VkDescriptorPool descPool;

    void createUniformBuffer(VkDevice& device,VkPhysicalDeviceMemoryProperties& memoryProperties);
    void destroyUniformBuffer(VkDevice& device);
    void createPipelineLayout(VkDevice& device);
    void destroyPipelineLayout(VkDevice& device);
    void initDescriptorSet(VkDevice& device);
    void createShaderModule(VkDevice& device);
    void destroyShaderModule(VkDevice& device);
    //初始化顶点着色器输入变量信息
    void initVertexAttributeInfo();
    void createPipeline(VkDevice& device,VkRenderPass& renderPass);
    void destroyPipeline(VkDevice& device);

  public:
    VkDeviceMemory memUniformBuf;
    int bufferByteCount;
    //Uniform写入用描述集
    VkWriteDescriptorSet writes[2];
    //描述集合列表
    std::vector<VkDescriptorSet> descSet;
    //管线布局
    VkPipelineLayout pipelineLayout;
    //管线
    VkPipeline pipeline;

    ShaderQueueSuit_CommonTexLight(VkDevice* deviceIn,VkRenderPass& renderPass,VkPhysicalDeviceMemoryProperties& memoryProperties);
    ~ShaderQueueSuit_CommonTexLight();
};
