#include "ShaderQueueSuit_Light.h"
#include "FileUtil.h"
#include "HelpFunction.h"
#include "ShaderCompileUtil.h"
#include "Util.h"
#include "VulkanDemoApp.h"
#include <assert.h>
#include <shaderc/shaderc.hpp>
void ShaderQueueSuit_Light::createUniformBuffer(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties) {
    bufferByteCount = sizeof(float) * 68;
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
void ShaderQueueSuit_Light::destroyUniformBuffer(VkDevice &device) {
    vkDestroyBuffer(device, uniformBuf, nullptr);
    vkFreeMemory(device, memUniformBuf, nullptr);
}
void ShaderQueueSuit_Light::createPipelineLayout(VkDevice &device) {
    VkDescriptorSetLayoutBinding layout_bindings[4];
    layout_bindings[0].binding = 0;
    layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    layout_bindings[0].descriptorCount = 1;
    layout_bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layout_bindings[0].pImmutableSamplers = nullptr;
    layout_bindings[1].binding = 1;
    layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    layout_bindings[1].descriptorCount = 1;
    layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layout_bindings[1].pImmutableSamplers = nullptr;
    layout_bindings[2].binding = 2;
    layout_bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    layout_bindings[2].descriptorCount = 1;
    layout_bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layout_bindings[2].pImmutableSamplers = nullptr;
    layout_bindings[3].binding = 3;
    layout_bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_bindings[3].descriptorCount = 1;
    layout_bindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layout_bindings[3].pImmutableSamplers = nullptr;
    VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.pNext = nullptr;
    descriptor_layout.bindingCount = 4;
    descriptor_layout.pBindings = layout_bindings;
    descLayouts.resize(NUM_DESCRIPTOR_SETS);
    VkResult result = vkCreateDescriptorSetLayout(device, &descriptor_layout, nullptr, descLayouts.data());
    assert(result == VK_SUCCESS);
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = nullptr;
    pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pPipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pPipelineLayoutCreateInfo.pSetLayouts = descLayouts.data();
    result = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    assert(result == VK_SUCCESS);
}
void ShaderQueueSuit_Light::destroyPipelineLayout(VkDevice &device) {
    for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++) {
        vkDestroyDescriptorSetLayout(device, descLayouts[i], nullptr);
    }
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}
VkDescriptorImageInfo descriptorImageInfo(//获取图像描述信息结构体实例的方法
        VkSampler sampler,
        VkImageView imageView,
        VkImageLayout imageLayout) {
    VkDescriptorImageInfo descriptorImageInfo{};  //构建图像描述信息结构体实例
    descriptorImageInfo.sampler = sampler;        //指定采用的采样器
    descriptorImageInfo.imageView = imageView;    //指定采用的图像视图
    descriptorImageInfo.imageLayout = imageLayout;//指定采用的图像布局
    return descriptorImageInfo;                   //返回图像描述信息结构体实例
}
void ShaderQueueSuit_Light::initDescriptorSet(VkDevice &device) {//初始化描述集的方法
    VkDescriptorPoolSize type_count[4];                          //描述集池尺寸实例数组
    type_count[0].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;    //第1 个描述的类型
    type_count[0].descriptorCount = 1;                           //第1 个描述的数量
    type_count[1].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    type_count[1].descriptorCount = 1;
    type_count[2].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    type_count[2].descriptorCount = 1;
    type_count[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    type_count[3].descriptorCount = 1;
    VkDescriptorPoolCreateInfo descriptor_pool = {};
    descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool.pNext = nullptr;
    descriptor_pool.maxSets = 1;
    descriptor_pool.poolSizeCount = 4;
    descriptor_pool.pPoolSizes = type_count;
    VkResult result = vkCreateDescriptorPool(device, &descriptor_pool, nullptr, &descPool);
    assert(result == VK_SUCCESS);
    std::vector<VkDescriptorSetLayout> layouts;
    layouts.push_back(descLayouts[0]);
    VkDescriptorSetAllocateInfo alloc_info[1];
    alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info[0].pNext = nullptr;
    alloc_info[0].descriptorPool = descPool;
    alloc_info[0].descriptorSetCount = layouts.size();
    alloc_info[0].pSetLayouts = layouts.data();
    descSet.resize(1);
    result = vkAllocateDescriptorSets(device, alloc_info, descSet.data());
    assert(result == VK_SUCCESS);
    writes[0] = {};                                                //完善一致变量写入描述集实例数组元素0
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;      //结构体类型
    writes[0].dstBinding = 0;                                      //目标绑定编号
    writes[0].pNext = nullptr;                                     //自定义数据的指针
    writes[0].descriptorCount = 1;                                 //描述数量
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;//描述类型(输入附件)
    colorImageInfo = descriptorImageInfo(                          //调用方法获取图像描述信息结构体实例
            VK_NULL_HANDLE, VulkanDemoApp::colorImageView[0],
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    writes[0].pImageInfo = &colorImageInfo;//图像信息
    writes[0].dstSet = descSet[0];         //更新描述集对应的写入属性0(一致变量)
    writes[1] = {};
    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding = 1;
    writes[1].pNext = nullptr;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    positionImageInfo = descriptorImageInfo(VK_NULL_HANDLE, VulkanDemoApp::colorImageView[1], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    writes[1].pImageInfo = &positionImageInfo;
    writes[2] = {};
    writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[2].dstBinding = 2;
    writes[2].pNext = nullptr;
    writes[2].descriptorCount = 1;
    writes[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    normalImageInfo = descriptorImageInfo(VK_NULL_HANDLE, VulkanDemoApp::colorImageView[2], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    writes[2].pImageInfo = &normalImageInfo;
    writes[3] = {};
    writes[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[3].dstBinding = 3;
    writes[3].pNext = nullptr;
    writes[3].descriptorCount = 1;
    writes[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[3].pBufferInfo = &uniformBufferInfo;
    writes[3].dstArrayElement = 0;
    writes[0].dstSet = descSet[0];
    writes[1].dstSet = descSet[0];
    writes[2].dstSet = descSet[0];
    writes[3].dstSet = descSet[0];
}
void ShaderQueueSuit_Light::createShaderModule(VkDevice &device) {
    const char *vs = "./shaders/light.vert";
    const char *fs = "./shaders/light.frag";
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
    printf("顶点着色器加载成功！");
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
    printf("片元着色器加载成功！");
}
void ShaderQueueSuit_Light::destroyShaderModule(VkDevice &device) {
    vkDestroyShaderModule(device, shaderStages[0].module, nullptr);
    vkDestroyShaderModule(device, shaderStages[1].module, nullptr);
}
void ShaderQueueSuit_Light::createPipeline(VkDevice &device, VkRenderPass &renderPass) {
    VkDynamicState dynamicStateEnables[2];
    memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.pDynamicStates = dynamicStateEnables;
    dynamicState.dynamicStateCount = 0;
    VkPipelineVertexInputStateCreateInfo vi{};
    vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VkPipelineInputAssemblyStateCreateInfo ia;
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.pNext = nullptr;
    ia.flags = 0;
    ia.primitiveRestartEnable = VK_FALSE;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPipelineRasterizationStateCreateInfo rs;
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.pNext = nullptr;
    rs.flags = 0;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = VK_CULL_MODE_NONE;
    rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.depthClampEnable = VK_TRUE;
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
    viewports.width = VulkanDemoApp::screenWidth;
    viewports.height = VulkanDemoApp::screenHeight;
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
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.stageCount = 2;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 1;
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
void ShaderQueueSuit_Light::destroyPipeline(VkDevice &device) {
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineCache(device, pipelineCache, nullptr);
}
ShaderQueueSuit_Light::ShaderQueueSuit_Light(VkDevice *deviceIn, VkRenderPass &renderPass, VkPhysicalDeviceMemoryProperties &memoryProperties) {
    this->devicePointer = deviceIn;
    createUniformBuffer(*devicePointer, memoryProperties);
    createPipelineLayout(*devicePointer);
    initDescriptorSet(*devicePointer);
    createShaderModule(*devicePointer);
    createPipeline(*devicePointer, renderPass);
}
ShaderQueueSuit_Light::~ShaderQueueSuit_Light() {
    destroyPipeline(*devicePointer);
    destroyShaderModule(*devicePointer);
    destroyPipelineLayout(*devicePointer);
    destroyUniformBuffer(*devicePointer);
}