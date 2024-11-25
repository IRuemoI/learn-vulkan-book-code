#include "VulkanDrawable.h"

#include "VulkanApplication.h"
#include "VulkanRenderer.h"
#include "Wrappers.h"

VulkanDrawable::VulkanDrawable(VulkanRenderer *parent) {
    // 注意需要使用0来填充内存
    memset(&VertexBuffer, 0, sizeof(VertexBuffer));
    rendererObj = parent;

    VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo;
    presentCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    presentCompleteSemaphoreCreateInfo.pNext = nullptr;
    presentCompleteSemaphoreCreateInfo.flags = 0;

    VkSemaphoreCreateInfo drawingCompleteSemaphoreCreateInfo;
    drawingCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    drawingCompleteSemaphoreCreateInfo.pNext = nullptr;
    drawingCompleteSemaphoreCreateInfo.flags = 0;

    VulkanDevice *deviceObj = VulkanApplication::GetInstance()->deviceObj;

    vkCreateSemaphore(deviceObj->device, &presentCompleteSemaphoreCreateInfo, nullptr, &presentCompleteSemaphore);
    vkCreateSemaphore(deviceObj->device, &drawingCompleteSemaphoreCreateInfo, nullptr, &drawingCompleteSemaphore);
}

VulkanDrawable::~VulkanDrawable() {
}

void VulkanDrawable::destroyCommandBuffer() {
    VulkanApplication *appObj = VulkanApplication::GetInstance();
    VulkanDevice *deviceObj = appObj->deviceObj;
    for (int i = 0; i < vecCmdDraw.size(); i++) {
        vkFreeCommandBuffers(deviceObj->device, rendererObj->cmdPool, 1, &vecCmdDraw[i]);
    }
}

void VulkanDrawable::destroySynchronizationObjects() {
    VulkanApplication *appObj = VulkanApplication::GetInstance();
    VulkanDevice *deviceObj = appObj->deviceObj;
    vkDestroySemaphore(deviceObj->device, presentCompleteSemaphore, nullptr);
    vkDestroySemaphore(deviceObj->device, drawingCompleteSemaphore, nullptr);
}

void VulkanDrawable::createVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture) {
    VulkanApplication *appObj = VulkanApplication::GetInstance();
    VulkanDevice *deviceObj = appObj->deviceObj;

    VkResult result;
    bool pass;

    // Create the Buffer resourece metadata information
    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.pNext = nullptr;
    bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufInfo.size = dataSize;
    bufInfo.queueFamilyIndexCount = 0;
    bufInfo.pQueueFamilyIndices = nullptr;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufInfo.flags = 0;

    // Create the Buffer resource
    result = vkCreateBuffer(deviceObj->device, &bufInfo, nullptr, &VertexBuffer.buf);
    assert(result == VK_SUCCESS);

    // Get the Buffer resource requirements
    VkMemoryRequirements memRqrmnt;
    vkGetBufferMemoryRequirements(deviceObj->device, VertexBuffer.buf, &memRqrmnt);

    // 创建内存分配元数据信息
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.memoryTypeIndex = 0;
    allocInfo.allocationSize = memRqrmnt.size;

    // 获取兼容的内存类型
    pass = deviceObj->memoryTypeFromProperties(memRqrmnt.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocInfo.memoryTypeIndex);
    assert(pass);

    // 为缓存资源分配实际的物理设备内存
    result = vkAllocateMemory(deviceObj->device, &allocInfo, nullptr, &(VertexBuffer.mem));
    assert(result == VK_SUCCESS);
    VertexBuffer.bufferInfo.range = memRqrmnt.size;
    VertexBuffer.bufferInfo.offset = 0;

    // 将物理设备内存区域映射到主机上
    uint8_t *pData;
    result = vkMapMemory(deviceObj->device, VertexBuffer.mem, 0, memRqrmnt.size, 0, (void **) &pData);
    assert(result == VK_SUCCESS);

    // 将数据复制到映射的内存
    memcpy(pData, vertexData, dataSize);

    // 取消物理设备内存映射
    vkUnmapMemory(deviceObj->device, VertexBuffer.mem);

    // 将已分配的缓存资源绑定到物理内存
    result = vkBindBufferMemory(deviceObj->device, VertexBuffer.buf, VertexBuffer.mem, 0);
    assert(result == VK_SUCCESS);

    // 在viIpBind中保存顶点数据的遍历方式
    viIpBind.binding = 0;
    viIpBind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    viIpBind.stride = dataStride;

    // 存储顶点数据解析信息
    viIpAttrb[0].binding = 0;
    viIpAttrb[0].location = 0;
    viIpAttrb[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    viIpAttrb[0].offset = 0;
    viIpAttrb[1].binding = 0;
    viIpAttrb[1].location = 1;
    viIpAttrb[1].format = useTexture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
    viIpAttrb[1].offset = 16;// After, 4 components - RGBA  each of 4 bytes(32bits)
}

void VulkanDrawable::initViewports(VkCommandBuffer *cmd) {
    viewport.height = (float) rendererObj->height;
    viewport.width = (float) rendererObj->width;
    viewport.minDepth = (float) 0.0f;
    viewport.maxDepth = (float) 1.0f;
    viewport.x = 0;
    viewport.y = 0;
    vkCmdSetViewport(*cmd, 0, NUMBER_OF_VIEWPORTS, &viewport);
}

void VulkanDrawable::initScissors(VkCommandBuffer *cmd) {
    scissor.extent.width = rendererObj->width;
    scissor.extent.height = rendererObj->height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vkCmdSetScissor(*cmd, 0, NUMBER_OF_SCISSORS, &scissor);
}

void VulkanDrawable::destroyVertexBuffer() {
    vkDestroyBuffer(rendererObj->getDevice()->device, VertexBuffer.buf, nullptr);
    vkFreeMemory(rendererObj->getDevice()->device, VertexBuffer.mem, nullptr);
}

void VulkanDrawable::recordCommandBuffer(int currentImage, VkCommandBuffer *cmdDraw) {
    VulkanDevice *deviceObj = rendererObj->getDevice();
    // 设置清除颜色值
    VkClearValue clearValues[2];
    clearValues[0].color.float32[0] = 0.0f;
    clearValues[0].color.float32[1] = 0.0f;
    clearValues[0].color.float32[2] = 0.0f;
    clearValues[0].color.float32[3] = 0.0f;

    // 设置深度/蒙版的清理数值
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    // 定义VkRenderPassBeginInfo结构体
    VkRenderPassBeginInfo renderPassBegin;
    renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBegin.pNext = nullptr;
    renderPassBegin.renderPass = rendererObj->renderPass;
    renderPassBegin.framebuffer = rendererObj->framebuffers[currentImage];
    renderPassBegin.renderArea.offset.x = 0;
    renderPassBegin.renderArea.offset.y = 0;
    renderPassBegin.renderArea.extent.width = rendererObj->width;
    renderPassBegin.renderArea.extent.height = rendererObj->height;
    renderPassBegin.clearValueCount = 2;
    renderPassBegin.pClearValues = clearValues;

    // 开始录制渲染通道
    vkCmdBeginRenderPass(*cmdDraw, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

    // Bound the command buffer with the graphics pipeline
    vkCmdBindPipeline(*cmdDraw, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

    // Bound the command buffer with the graphics pipeline
    const VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(*cmdDraw, 0, 1, &VertexBuffer.buf, offsets);

    // Define the dynamic viewport here
    initViewports(cmdDraw);

    // Define the scissoring
    initScissors(cmdDraw);

    // Issue the draw command with 3 vertex, 1 instance starting
    // from first vertex
    vkCmdDraw(*cmdDraw, 3, 1, 0, 0);

    // 完成渲染通道录制
    vkCmdEndRenderPass(*cmdDraw);
}

void VulkanDrawable::prepare() {
    VulkanDevice *deviceObj = rendererObj->getDevice();
    vecCmdDraw.resize(rendererObj->getSwapChain()->scPublicVars.colorBuffer.size());
    // 对于每个交换链中的颜色图缓存，都要分配一个对应的指令缓存
    for (int i = 0; i < rendererObj->getSwapChain()->scPublicVars.colorBuffer.size(); i++) {
        // 分配，创建并启动该指令缓存的录制
        CommandBufferMgr::allocCommandBuffer(&deviceObj->device, *rendererObj->getCommandPool(), &vecCmdDraw[i]);
        CommandBufferMgr::beginCommandBuffer(vecCmdDraw[i]);

        // 创建渲染通道实例
        recordCommandBuffer(i, &vecCmdDraw[i]);

        // 完成指令缓存的录制
        CommandBufferMgr::endCommandBuffer(vecCmdDraw[i]);
    }
}

void VulkanDrawable::render() {
    VulkanDevice *deviceObj = rendererObj->getDevice();
    VulkanSwapChain *swapChainObj = rendererObj->getSwapChain();

    uint32_t &currentColorImage = swapChainObj->scPublicVars.currentColorBuffer;
    VkSwapchainKHR &swapChain = swapChainObj->scPublicVars.swapChain;

    VkFence nullFence = VK_NULL_HANDLE;

    // 获取下一个可用的交换链图像索引
    VkResult result = swapChainObj->fpAcquireNextImageKHR(deviceObj->device, swapChain,
                                                          UINT64_MAX, presentCompleteSemaphore, VK_NULL_HANDLE, &currentColorImage);

    VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &presentCompleteSemaphore;
    submitInfo.pWaitDstStageMask = &submitPipelineStages;
    submitInfo.commandBufferCount = (uint32_t) sizeof(&vecCmdDraw[currentColorImage]) / sizeof(VkCommandBuffer);
    submitInfo.pCommandBuffers = &vecCmdDraw[currentColorImage];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &drawingCompleteSemaphore;

    // 将指令缓存的内容提交到队列
    CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &vecCmdDraw[currentColorImage], &submitInfo);

    // 将图像呈现到窗口
    VkPresentInfoKHR present = {};
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = nullptr;
    present.swapchainCount = 1;
    present.pSwapchains = &swapChain;
    present.pImageIndices = &currentColorImage;
    present.pWaitSemaphores = &drawingCompleteSemaphore;
    present.waitSemaphoreCount = 1;
    present.pResults = nullptr;

    // 将图像以排队方式呈现
    result = swapChainObj->fpQueuePresentKHR(deviceObj->queue, &present);
    assert(result == VK_SUCCESS);
}
