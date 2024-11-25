#include "ShaderQueueSuit_Common.h"
#include "FileUtil.h"
#include "HelpFunction.h"
#include "ShaderCompileUtil.h"
#include "TextureManager.h"
#include "Util.h"
#include "VulkanDemoApp.h"
#include <assert.h>
void ShaderQueueSuit_Common::createUniformBuffer(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties) {
    bufferByteCount = sizeof(float) * 20;
    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.pNext = nullptr;
    buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buf_info.size = bufferByteCount;
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = nullptr;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buf_info.flags = 0;
    VkResult result = vkCreateBuffer(device, &buf_info, nullptr, &uniformBuf);
    assert(result == VK_SUCCESS);
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(device, uniformBuf, &mem_reqs);
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.memoryTypeIndex = 0;
    alloc_info.allocationSize = mem_reqs.size;
    VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bool flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
    if (flag) {
        printf("确定内存类型成功 类型索引为%d", alloc_info.memoryTypeIndex);
    } else {
        printf("确定内存类型失败!");
    }
    result = vkAllocateMemory(device, &alloc_info, nullptr, &memUniformBuf);
    assert(result == VK_SUCCESS);
    result = vkBindBufferMemory(device, uniformBuf, memUniformBuf, 0);
    assert(result == VK_SUCCESS);
    uniformBufferInfo.buffer = uniformBuf;
    uniformBufferInfo.offset = 0;
    uniformBufferInfo.range = bufferByteCount;
}
void ShaderQueueSuit_Common::destroyUniformBuffer(VkDevice &device) {
    vkDestroyBuffer(device, uniformBuf, nullptr);
    vkFreeMemory(device, memUniformBuf, nullptr);
}
void ShaderQueueSuit_Common::createPipelineLayout(VkDevice &device) {
    VkDescriptorSetLayoutBinding layout_bindings[2];
    layout_bindings[0].binding = 0;
    layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_bindings[0].descriptorCount = 1;
    layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_bindings[0].pImmutableSamplers = nullptr;
    layout_bindings[1].binding = 1;
    layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layout_bindings[1].descriptorCount = 1;
    layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layout_bindings[1].pImmutableSamplers = nullptr;
    VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.pNext = nullptr;
    descriptor_layout.bindingCount = 2;
    descriptor_layout.pBindings = layout_bindings;
    descLayouts.resize(NUM_DESCRIPTOR_SETS);
    VkResult result = vkCreateDescriptorSetLayout(device, &descriptor_layout, nullptr, descLayouts.data());
    assert(result == VK_SUCCESS);
    const unsigned push_constant_range_count = 1;
    VkPushConstantRange push_constant_ranges[push_constant_range_count] = {};
    push_constant_ranges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_ranges[0].offset = 0;
    push_constant_ranges[0].size = sizeof(float) * 32;
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = nullptr;
    pPipelineLayoutCreateInfo.pushConstantRangeCount = push_constant_range_count;
    pPipelineLayoutCreateInfo.pPushConstantRanges = push_constant_ranges;
    pPipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pPipelineLayoutCreateInfo.pSetLayouts = descLayouts.data();
    result = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    assert(result == VK_SUCCESS);
}
void ShaderQueueSuit_Common::destroyPipelineLayout(VkDevice &device) {
    for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++) {
        vkDestroyDescriptorSetLayout(device, descLayouts[i], nullptr);
    }
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}
void ShaderQueueSuit_Common::initDescriptorSet(VkDevice &device) {
    VkDescriptorPoolSize type_count[2];
    type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    type_count[0].descriptorCount = TextureManager::texNames.size();
    type_count[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    type_count[1].descriptorCount = TextureManager::texNames.size();
    VkDescriptorPoolCreateInfo descriptor_pool = {};
    descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool.pNext = nullptr;
    descriptor_pool.maxSets = TextureManager::texNames.size();
    descriptor_pool.poolSizeCount = 2;
    descriptor_pool.pPoolSizes = type_count;
    VkResult result = vkCreateDescriptorPool(device, &descriptor_pool, nullptr, &descPool);
    assert(result == VK_SUCCESS);
    std::vector<VkDescriptorSetLayout> layouts;
    for (int i = 0; i < TextureManager::texNames.size(); i++) {
        layouts.push_back(descLayouts[0]);
    }
    VkDescriptorSetAllocateInfo alloc_info[1];
    alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info[0].pNext = nullptr;
    alloc_info[0].descriptorPool = descPool;
    alloc_info[0].descriptorSetCount = TextureManager::texNames.size();
    alloc_info[0].pSetLayouts = layouts.data();
    descSet.resize(TextureManager::texNames.size());
    result = vkAllocateDescriptorSets(device, alloc_info, descSet.data());
    assert(result == VK_SUCCESS);
    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = nullptr;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &uniformBufferInfo;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;
    writes[1] = {};
    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding = 1;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].dstArrayElement = 0;
}
void ShaderQueueSuit_Common::createShaderModule(VkDevice &device) {
    const char *vs = "./shaders/shader.vert";
    const char *fs = "./shaders/shader.frag";
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].pNext = nullptr;
    shaderStages[0].pSpecializationInfo = nullptr;
    shaderStages[0].flags = 0;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].pName = "main";
    std::vector<unsigned int> vtx_spv;
    bool retVal = compileGLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, FileUtil::loadAssetStr(vs).c_str(), vtx_spv);
    assert(retVal);
    printf("顶点着色器脚本编译SPV成功！\n");
    VkShaderModuleCreateInfo moduleCreateInfo;
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = vtx_spv.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = vtx_spv.data();
    VkResult result = vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderStages[0].module);
    assert(result == VK_SUCCESS);
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].pNext = nullptr;
    shaderStages[1].pSpecializationInfo = nullptr;
    shaderStages[1].flags = 0;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].pName = "main";
    std::vector<unsigned int> frag_spv;
    retVal = compileGLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, FileUtil::loadAssetStr(fs).c_str(), frag_spv);
    assert(retVal);
    printf("片元着色器脚本编译SPV成功！\n");
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = frag_spv.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = frag_spv.data();
    result = vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderStages[1].module);
    assert(result == VK_SUCCESS);
}
void ShaderQueueSuit_Common::destroyShaderModule(VkDevice &device) {
    vkDestroyShaderModule(device, shaderStages[0].module, nullptr);
    vkDestroyShaderModule(device, shaderStages[1].module, nullptr);
}
void ShaderQueueSuit_Common::initVertexAttributeInfo() {
    vertexBinding.binding = 0;
    vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertexBinding.stride = sizeof(float) * 8;
    vertexAttributes[0].binding = 0;
    vertexAttributes[0].location = 0;
    vertexAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexAttributes[0].offset = 0;
    vertexAttributes[1].binding = 0;
    vertexAttributes[1].location = 1;
    vertexAttributes[1].format = VK_FORMAT_R32G32_SFLOAT;
    vertexAttributes[1].offset = 12;
    vertexAttributes[2].binding = 0;
    vertexAttributes[2].location = 2;
    vertexAttributes[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexAttributes[2].offset = 20;
}
void ShaderQueueSuit_Common::createPipeline(VkDevice &device, VkRenderPass &renderPass) {
    VkDynamicState dynamicStateEnables[2];
    memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.pDynamicStates = dynamicStateEnables;
    dynamicState.dynamicStateCount = 0;
    VkPipelineVertexInputStateCreateInfo vi;
    vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vi.pNext = nullptr;
    vi.flags = 0;
    vi.vertexBindingDescriptionCount = 1;
    vi.pVertexBindingDescriptions = &vertexBinding;
    vi.vertexAttributeDescriptionCount = 3;
    vi.pVertexAttributeDescriptions = vertexAttributes;
    VkPipelineInputAssemblyStateCreateInfo ia;
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.pNext = nullptr;
    ia.flags = 0;
    ia.primitiveRestartEnable = VK_FALSE;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;//TODO：可能是这里的问题
    VkPipelineRasterizationStateCreateInfo rs;
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.pNext = nullptr;
    rs.flags = 0;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = VK_CULL_MODE_BACK_BIT;
    rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.depthClampEnable = VK_FALSE;
    rs.rasterizerDiscardEnable = VK_FALSE;
    rs.depthBiasEnable = VK_FALSE;
    rs.depthBiasConstantFactor = 0;
    rs.depthBiasClamp = 0;
    rs.depthBiasSlopeFactor = 0;
    rs.lineWidth = 1.0f;
    VkPipelineColorBlendAttachmentState att_state[1];
    att_state[0].colorWriteMask = 0xf;
    att_state[0].blendEnable = VK_FALSE;
    att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;
    att_state[0].colorBlendOp = VK_BLEND_OP_ADD;
    att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    VkPipelineColorBlendStateCreateInfo cb;
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.pNext = nullptr;
    cb.flags = 0;
    cb.attachmentCount = 1;
    cb.pAttachments = att_state;
    cb.logicOpEnable = VK_FALSE;
    cb.logicOp = VK_LOGIC_OP_NO_OP;
    cb.blendConstants[0] = 1.0f;
    cb.blendConstants[1] = 1.0f;
    cb.blendConstants[2] = 1.0f;
    cb.blendConstants[3] = 1.0f;
    VkViewport viewports;
    viewports.minDepth = 0.0f;
    viewports.maxDepth = 1.0f;
    viewports.x = 0;
    viewports.y = 0;
    viewports.width = (float) VulkanDemoApp::screenWidth;
    viewports.height = (float) VulkanDemoApp::screenHeight;
    VkRect2D scissor;
    scissor.extent.width = VulkanDemoApp::screenWidth;
    scissor.extent.height = VulkanDemoApp::screenHeight;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    VkPipelineViewportStateCreateInfo vp = {};
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.pNext = nullptr;
    vp.flags = 0;
    vp.viewportCount = 1;
    vp.scissorCount = 1;
    vp.pScissors = &scissor;
    vp.pViewports = &viewports;
    VkPipelineDepthStencilStateCreateInfo ds;
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.pNext = nullptr;
    ds.flags = 0;
    ds.depthTestEnable = VK_TRUE;
    ds.depthWriteEnable = VK_TRUE;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    ds.depthBoundsTestEnable = VK_FALSE;
    ds.minDepthBounds = 0;
    ds.maxDepthBounds = 0;
    ds.stencilTestEnable = VK_FALSE;
    ds.back.failOp = VK_STENCIL_OP_KEEP;
    ds.back.passOp = VK_STENCIL_OP_KEEP;
    ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
    ds.back.compareMask = 0;
    ds.back.reference = 0;
    ds.back.depthFailOp = VK_STENCIL_OP_KEEP;
    ds.back.writeMask = 0;
    ds.front = ds.back;
    VkPipelineMultisampleStateCreateInfo ms;
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.pNext = nullptr;
    ms.flags = 0;
    ms.pSampleMask = nullptr;
    ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    ms.sampleShadingEnable = VK_FALSE;
    ms.alphaToCoverageEnable = VK_FALSE;
    ms.alphaToOneEnable = VK_FALSE;
    ms.minSampleShading = 0.0;
    VkGraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = 0;
    pipelineInfo.flags = 0;
    pipelineInfo.pVertexInputState = &vi;
    pipelineInfo.pInputAssemblyState = &ia;
    pipelineInfo.pRasterizationState = &rs;
    pipelineInfo.pColorBlendState = &cb;
    pipelineInfo.pTessellationState = nullptr;
    pipelineInfo.pMultisampleState = &ms;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.pViewportState = &vp;
    pipelineInfo.pDepthStencilState = &ds;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    VkPipelineCacheCreateInfo pipelineCacheInfo;
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCacheInfo.pNext = nullptr;
    pipelineCacheInfo.initialDataSize = 0;
    pipelineCacheInfo.pInitialData = nullptr;
    pipelineCacheInfo.flags = 0;
    VkResult result = vkCreatePipelineCache(device, &pipelineCacheInfo, nullptr, &pipelineCache);
    assert(result == VK_SUCCESS);
    result = vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo, nullptr, &pipeline);
    assert(result == VK_SUCCESS);
}
void ShaderQueueSuit_Common::destroyPipeline(VkDevice &device) {
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineCache(device, pipelineCache, nullptr);
}
ShaderQueueSuit_Common::ShaderQueueSuit_Common(VkDevice *deviceIn, VkRenderPass &renderPass, VkPhysicalDeviceMemoryProperties &memoryProperties) {
    this->devicePointer = deviceIn;
    createUniformBuffer(*devicePointer, memoryProperties);
    createPipelineLayout(*devicePointer);
    initDescriptorSet(*devicePointer);
    createShaderModule(*devicePointer);
    initVertexAttributeInfo();
    createPipeline(*devicePointer, renderPass);
}
ShaderQueueSuit_Common::~ShaderQueueSuit_Common() {
    destroyPipeline(*devicePointer);
    destroyShaderModule(*devicePointer);
    destroyPipelineLayout(*devicePointer);
    destroyUniformBuffer(*devicePointer);
}