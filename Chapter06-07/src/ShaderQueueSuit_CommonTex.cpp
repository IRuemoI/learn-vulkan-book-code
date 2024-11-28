#include "ShaderQueueSuit_CommonTex.h"
#include "FileUtil.h"
#include "HelpFunction.h"
#include "ShaderCompileUtil.h"
#include "TextureManager.h"
#include "Util.h"
#include "VulkanDemoApp.h"
#include <cassert>
#include <shaderc/shaderc.hpp>

void ShaderQueueSuit_CommonTex::createUniformBuffer(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties) {
    bufferByteCount = sizeof(float);
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
        printf("确定内存类型成功 类型索引为%d\n", alloc_info.memoryTypeIndex);
    } else {
        printf("确定内存类型失败!\n");
    }
    result = vkAllocateMemory(device, &alloc_info, nullptr, &memUniformBuf);
    assert(result == VK_SUCCESS);
    result = vkBindBufferMemory(device, uniformBuf, memUniformBuf, 0);
    assert(result == VK_SUCCESS);
    uniformBufferInfo.buffer = uniformBuf;
    uniformBufferInfo.offset = 0;
    uniformBufferInfo.range = bufferByteCount;
}
void ShaderQueueSuit_CommonTex::destroyUniformBuffer(VkDevice &device) {
    vkDestroyBuffer(device, uniformBuf, nullptr);
    vkFreeMemory(device, memUniformBuf, nullptr);
}
void ShaderQueueSuit_CommonTex::createPipelineLayout(VkDevice &device) {//创建管线布局的方法
    VkDescriptorSetLayoutBinding layout_bindings[2];//描述集布局绑定数组
    layout_bindings[0].binding = 0;//此绑定的绑定点编号为 0
    layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//描述类型
    layout_bindings[0].descriptorCount = 1;//描述数量
    layout_bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;//目标着色器阶段
    layout_bindings[0].pImmutableSamplers = nullptr;
    layout_bindings[1].binding = 1;//此绑定的绑定点编号为 1
    layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layout_bindings[1].descriptorCount = 1;//描述数量
    layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;//目标着色器阶段
    layout_bindings[1].pImmutableSamplers = nullptr;
    VkDescriptorSetLayoutCreateInfo descriptor_layout = {};//构建描述集布局创建信息结构体实例
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;//结构体类型
    descriptor_layout.pNext = nullptr;//自定义数据的指针
    descriptor_layout.bindingCount = 2;//描述集布局绑定的数量
    descriptor_layout.pBindings = layout_bindings;//描述集布局绑定数组
    descLayouts.resize(NUM_DESCRIPTOR_SETS);//调整描述集布局列表尺寸
    VkResult result = vkCreateDescriptorSetLayout(device, &descriptor_layout, nullptr, descLayouts.data());//创建描述集布局
    assert(result == VK_SUCCESS);//检查描述集布局创建是否成功
    const unsigned push_constant_range_count = 1;//推送常量范围实例数量
    VkPushConstantRange push_constant_ranges[push_constant_range_count] = {};//推送常量范围数组
    push_constant_ranges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;//着色器阶段
    push_constant_ranges[0].offset = 0;//起始偏移量
    push_constant_ranges[0].size = sizeof(float) * 16;//数据字节数
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};//构建管线布局创建信息结构体实例
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;//结构体类型
    pPipelineLayoutCreateInfo.pNext = nullptr;//自定义数据的指针
    pPipelineLayoutCreateInfo.pushConstantRangeCount = push_constant_range_count;//推送常量范围的数量
    pPipelineLayoutCreateInfo.pPushConstantRanges = push_constant_ranges;//推送常量范围的列表
    pPipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;//描述集布局的数量
    pPipelineLayoutCreateInfo.pSetLayouts = descLayouts.data();//描述集布局列表
    result = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout);//创建管线布局
    assert(result == VK_SUCCESS);//检查创建是否成功
}
void ShaderQueueSuit_CommonTex::destroyPipelineLayout(VkDevice &device) {
    for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++) {
        vkDestroyDescriptorSetLayout(device, descLayouts[i], nullptr);
    }
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}
void ShaderQueueSuit_CommonTex::initDescriptorSet(VkDevice &device) {//初始化描述集的方法
    VkDescriptorPoolSize type_count[2];//描述集池尺寸实例数组
    type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//第 1 个描述类型
    type_count[0].descriptorCount = TextureManager::texNames.size();//第 1 个描述数量
    type_count[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;//第 2 个描述类型
    type_count[1].descriptorCount = TextureManager::texNames.size();//第 2 个描述数量
    VkDescriptorPoolCreateInfo descriptor_pool = {};//构建描述集池创建信息结构体实例
    descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;//结构体类型
    descriptor_pool.pNext = nullptr;//自定义数据的指针
    descriptor_pool.maxSets = TextureManager::texNames.size();//描述集最大数量
    descriptor_pool.poolSizeCount = 2;//描述集池尺寸实例数量
    descriptor_pool.pPoolSizes = type_count;//描述集池尺寸实例数组
    VkResult result = vkCreateDescriptorPool(device, &descriptor_pool, nullptr, &descPool);//创建描述集池
    assert(result == VK_SUCCESS);//检查描述集池创建是否成功
    std::vector<VkDescriptorSetLayout> layouts;//描述集布局列表
    for (int i = 0; i < TextureManager::texNames.size(); i++) {//遍历所有纹理
        layouts.push_back(descLayouts[0]);//向列表中添加指定描述集布局
    }
    VkDescriptorSetAllocateInfo alloc_info[1];//构建描述集分配信息结构体实例数组
    alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;//结构体类型
    alloc_info[0].pNext = nullptr;//自定义数据的指针
    alloc_info[0].descriptorPool = descPool;//指定描述集池
    alloc_info[0].descriptorSetCount = TextureManager::texNames.size();//描述集数量
    alloc_info[0].pSetLayouts = layouts.data();//描述集布局列表
    descSet.resize(TextureManager::texNames.size());//调整描述集列表尺寸
    result = vkAllocateDescriptorSets(device, alloc_info, descSet.data());//分配指定数量的描述集
    assert(result == VK_SUCCESS);//检查描述集分配是否成功
    writes[0] = {};//完善一致变量写入描述集实例数组元素 0
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;//结构体类型
    writes[0].pNext = nullptr;//自定义数据的指针
    writes[0].descriptorCount = 1;//描述数量
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//描述类型(一致变量缓冲)
    writes[0].pBufferInfo = &uniformBufferInfo;//对应一致变量缓冲的信息
    writes[0].dstArrayElement = 0;//目标数组起始元素
    writes[0].dstBinding = 0;//目标绑定编号
    writes[1] = {};//完善一致变量写入描述集实例数组元素 1
    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;//结构体类型
    writes[1].dstBinding = 1;//目标绑定编号
    writes[1].descriptorCount = 1;//描述数量
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;//描述类型（采样用纹理）
    writes[1].dstArrayElement = 0;//目标数组起始元素
}
void ShaderQueueSuit_CommonTex::createShaderModule(VkDevice &device) {
    const char *vs = "./shaders/shader.vert";
    const char *fs = "./shaders/shader.frag";

    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].pNext = nullptr;// 自定义数据的指针
    shaderStages[0].pSpecializationInfo = nullptr;// 特殊信息
    shaderStages[0].flags = 0;// 供将来使用的标志
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;// 着色器阶段为顶点
    shaderStages[0].pName = "main";// 入口函数为main

    std::vector<unsigned int> vtx_spv;//将顶点着色器脚本编译为SPV
    bool retVal = compileGLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, FileUtil::loadAssetStr(vs).c_str(), vtx_spv);
    assert(retVal);//检查编译是否成功
    printf("顶点着色器脚本编译SPV成功！\n");

    VkShaderModuleCreateInfo moduleCreateInfo;// 准备顶点着色器模块创建信息
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;// 自定义数据的指针
    moduleCreateInfo.flags = 0;// 供将来使用的标志
    moduleCreateInfo.codeSize = vtx_spv.size() * sizeof(uint32_t);// 顶点着色器SPV数据总字节数
    moduleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(vtx_spv.data());// 顶点着色器SPV数据

    VkResult result = vkCreateShaderModule(device, &moduleCreateInfo, nullptr,
                                           &shaderStages[0].module);// 创建顶点着色器模块
    assert(result == VK_SUCCESS);// 检查顶点着色器模块创建是否成功

    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;// 结构体类型
    shaderStages[1].pNext = nullptr;// 自定义数据的指针
    shaderStages[1].pSpecializationInfo = nullptr;// 特殊信息
    shaderStages[1].flags = 0;// 供将来使用的标志
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;// 着色器阶段为片元
    shaderStages[1].pName = "main";// 入口函数为main

    std::vector<unsigned int> frag_spv;
    retVal = compileGLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, FileUtil::loadAssetStr(fs).c_str(), frag_spv);//将片元着色器脚本编译为SPV
    assert(retVal);//检查编译是否成功
    printf("片元着色器脚本编译SPV成功！\n");

    // 准备片元着色器模块创建信息
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;// 设置结构体类型
    moduleCreateInfo.pNext = nullptr;// 自定义数据的指针
    moduleCreateInfo.flags = 0;// 供将来使用的标志
    moduleCreateInfo.codeSize = frag_spv.size() * sizeof(uint32_t);// 片元着色器SPV数据总字节数
    moduleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(frag_spv.data());// 片元着色器SPV数据
    // 创建片元着色器模块
    result = vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderStages[1].module);
    assert(result == VK_SUCCESS);// 检查片元着色器模块创建是否成功
}
void ShaderQueueSuit_CommonTex::destroyShaderModule(VkDevice &device) {
    vkDestroyShaderModule(device, shaderStages[0].module, nullptr);
    vkDestroyShaderModule(device, shaderStages[1].module, nullptr);
}
void ShaderQueueSuit_CommonTex::initVertexAttributeInfo() {//设置顶点着色器输入属性信息
    vertexBinding.binding = 0;//对应绑定点
    vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;//数据输入频率为每顶点
    vertexBinding.stride = sizeof(float) * 5;//每组数据的跨度字节数
    vertexAttributes[0].binding = 0;//第 1 个顶点输入属性的绑定点
    vertexAttributes[0].location = 0;//第 1 个顶点输入属性的位置索引
    vertexAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;//第 1 个顶点输入属性的数据格式
    vertexAttributes[0].offset = 0;//第 1 个顶点输入属性的偏移量
    vertexAttributes[1].binding = 0;//第 2 个顶点输入属性的绑定点
    vertexAttributes[1].location = 1;//第 2 个顶点输入属性的位置索引
    vertexAttributes[1].format = VK_FORMAT_R32G32_SFLOAT;//第 2 个顶点输入属性的数据格式
    vertexAttributes[1].offset = 12;//第 2 个顶点输入属性的偏移量
}
void ShaderQueueSuit_CommonTex::createPipeLine(VkDevice &device, VkRenderPass &renderPass) {
    //关于这里的修改参阅：https://github.com/LunarG/VulkanSamples/pull/294/commits/2667330a95f42a38e44567393a10f64ac9914fee
    //TODO:但是具体为啥这么改没搞懂
    VkDynamicState dynamicStateEnables[2];//Viewport + Scissor
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
    vi.vertexAttributeDescriptionCount = 2;
    vi.pVertexAttributeDescriptions = vertexAttributes;
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
    att_state[0].blendEnable = VK_TRUE;
    att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;
    att_state[0].colorBlendOp = VK_BLEND_OP_ADD;
    att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
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
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.stageCount = 2;
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
void ShaderQueueSuit_CommonTex::destroyPipeLine(VkDevice &device) {
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineCache(device, pipelineCache, nullptr);
}
ShaderQueueSuit_CommonTex::ShaderQueueSuit_CommonTex(VkDevice *deviceIn, VkRenderPass &renderPass, VkPhysicalDeviceMemoryProperties &memoryProperties) {
    this->devicePointer = deviceIn;
    createUniformBuffer(*devicePointer, memoryProperties);
    createPipelineLayout(*devicePointer);
    initDescriptorSet(*devicePointer);
    createShaderModule(*devicePointer);
    initVertexAttributeInfo();
    createPipeLine(*devicePointer, renderPass);
}
ShaderQueueSuit_CommonTex::~ShaderQueueSuit_CommonTex() {
    destroyPipeLine(*devicePointer);
    destroyShaderModule(*devicePointer);
    destroyPipelineLayout(*devicePointer);
    destroyUniformBuffer(*devicePointer);
}