#pragma once
#include "vulkan/vulkan.h"
#include <shaderc/shaderc.hpp>//这个头文件位于VulkanSDK的Include文件夹内
#include <span>
#include <vector>

class ShaderQueueSuit_Common {
private:
    VkBuffer uniformBuf;// 一致变量缓冲
    VkDescriptorBufferInfo uniformBufferInfo;// 一致变量缓冲描述信息
    std::vector<VkDescriptorSetLayout> descLayouts;// 描述集布局列表
    VkPipelineShaderStageCreateInfo shaderStages[2];// 着色器阶段数组
    VkVertexInputBindingDescription vertexBinding;// 管线的顶点输入数据绑定描述
    VkVertexInputAttributeDescription vertexAttributes[2];// 管线的顶点输入属性描述
    VkDevice *devicePointer;// 逻辑设备指针
    void createUniformBuffer(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);// 创建一致变量缓冲
    void destroyUniformBuffer(VkDevice &device);// 销毁一致变量缓冲
    void createPipelineLayout(VkDevice &device);// 创建管线布局
    void destroyPipelineLayout(VkDevice &device);// 销毁管线布局
    void initDescriptorSet(VkDevice &device);// 初始化描述集
    void createShaderModule(VkDevice &device);// 创建着色器模块
    void destroyShaderModule(VkDevice &device);// 销毁着色器模块
    void initVertexAttributeInfo();// 初始化顶点输入属性信息
    void createPipeline(VkDevice &device, VkRenderPass &renderPass);// 创建管线
    void destroyPipeline(VkDevice &device) const;// 销毁管线
    std::span<const uint32_t> compileGLSLtoSPIRV(const char *shaderFilePath, shaderc_shader_kind shader_kind) const;

public:
    VkDescriptorPool descPool;// 描述池
    VkPipelineCache pipelineCache;// 管线缓冲
    int bufferByteCount;// 一致缓冲总字节数
    VkDeviceMemory memUniformBuf;// 一致变量缓冲内存
    VkWriteDescriptorSet writes[1];// 一致变量写入描述集
    std::vector<VkDescriptorSet> descSet;// 描述集列表
    VkPipelineLayout pipelineLayout;// 管线布局
    VkPipeline pipeline;// 管线
    shaderc::Compiler compiler;
    ShaderQueueSuit_Common(VkDevice *deviceIn, VkRenderPass &renderPass, VkPhysicalDeviceMemoryProperties &memoryProperties);// 构造函数
    ~ShaderQueueSuit_Common();// 析构函数
};