#include "ShaderQueueSuit_ComputeNormal.h"
#include "FileUtil.h"
#include "HelpFunction.h"
#include "TextureManager.h"
#include "Util.h"
#include "VulkanDemoApp.h"
#include <assert.h>
#include <iostream>

void ShaderQueueSuit_ComputeNormal::create_pipeline_layout(VkDevice &device) {


    VkDescriptorSetLayoutBinding layout_bindings[1];

    layout_bindings[0].binding = 0;
    layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layout_bindings[0].descriptorCount = 1;
    layout_bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    layout_bindings[0].pImmutableSamplers = nullptr;


    VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.pNext = nullptr;
    descriptor_layout.bindingCount = 1;
    descriptor_layout.pBindings = layout_bindings;


    descLayouts.resize(NUM_DESCRIPTOR_SETS);
    VkResult result = vkCreateDescriptorSetLayout(device, &descriptor_layout, nullptr, descLayouts.data());
    assert(result == VK_SUCCESS);


    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = nullptr;
    pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges = VK_NULL_HANDLE;
    pPipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pPipelineLayoutCreateInfo.pSetLayouts = descLayouts.data();
    result = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    assert(result == VK_SUCCESS);
}


void ShaderQueueSuit_ComputeNormal::destroy_pipeline_layout(VkDevice &device) {
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}


void ShaderQueueSuit_ComputeNormal::init_descriptor_set(VkDevice &device) {

    VkDescriptorPoolSize type_count[1];
    type_count[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    type_count[0].descriptorCount = 1;


    VkDescriptorPoolCreateInfo descriptor_pool = {};
    descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool.pNext = nullptr;
    descriptor_pool.maxSets = 1;
    descriptor_pool.poolSizeCount = 1;
    descriptor_pool.pPoolSizes = type_count;


    VkResult result = vkCreateDescriptorPool(device, &descriptor_pool, nullptr, &descPool);
    assert(result == VK_SUCCESS);


    std::vector<VkDescriptorSetLayout> layouts;
    for (int i = 0; i < 1; i++) {
        layouts.push_back(descLayouts[0]);
    }


    VkDescriptorSetAllocateInfo alloc_info[1];
    alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info[0].pNext = nullptr;
    alloc_info[0].descriptorPool = descPool;
    alloc_info[0].descriptorSetCount = 1;
    alloc_info[0].pSetLayouts = layouts.data();


    descSet.resize(1);

    result = vkAllocateDescriptorSets(device, alloc_info, descSet.data());
    assert(result == VK_SUCCESS);


    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = nullptr;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;
}

void ShaderQueueSuit_ComputeNormal::create_shader(VkDevice &device) {
    SpvData spvCompData = FileUtil::loadSPV("./shaders/computeNormal.comp.spv");
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].pNext = nullptr;
    shaderStages[0].pSpecializationInfo = nullptr;
    shaderStages[0].flags = 0;
    shaderStages[0].stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStages[0].pName = "main";

    VkShaderModuleCreateInfo moduleCreateInfo;
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = spvCompData.size;
    moduleCreateInfo.pCode = spvCompData.data;

    VkResult result = vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderStages[0].module);
    assert(result == VK_SUCCESS);
}

void ShaderQueueSuit_ComputeNormal::destroy_shader(VkDevice &device) {
    vkDestroyShaderModule(device, shaderStages[0].module, nullptr);
}

void ShaderQueueSuit_ComputeNormal::create_pipe_line(VkDevice &device) {
    VkComputePipelineCreateInfo pipelineInfo;
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.flags = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = 0;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.stage = shaderStages[0];


    VkPipelineCacheCreateInfo pipelineCacheInfo;
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCacheInfo.pNext = nullptr;
    pipelineCacheInfo.initialDataSize = 0;
    pipelineCacheInfo.pInitialData = nullptr;
    pipelineCacheInfo.flags = 0;
    VkResult result = vkCreatePipelineCache(device, &pipelineCacheInfo, nullptr, &pipelineCache);
    assert(result == VK_SUCCESS);

    result = vkCreateComputePipelines(device, pipelineCache, 1, &pipelineInfo, nullptr, &pipeline);
    assert(result == VK_SUCCESS);
}

void ShaderQueueSuit_ComputeNormal::destroy_pipe_line(VkDevice &device) {

    vkDestroyPipeline(device, pipeline, nullptr);

    vkDestroyPipelineCache(device, pipelineCache, nullptr);
}

ShaderQueueSuit_ComputeNormal::ShaderQueueSuit_ComputeNormal(VkDevice *deviceIn, VkPhysicalDeviceMemoryProperties &memoryroperties) {
    this->devicePointer = deviceIn;
    create_shader(*devicePointer);
    create_pipeline_layout(*devicePointer);
    init_descriptor_set(*devicePointer);
    create_pipe_line(*devicePointer);
}

ShaderQueueSuit_ComputeNormal::~ShaderQueueSuit_ComputeNormal() {
    destroy_pipe_line(*devicePointer);
    destroy_shader(*devicePointer);
    destroy_pipeline_layout(*devicePointer);
}
