#include "Wrappers.h"

void CommandBufferMgr::allocCommandBuffer(const VkDevice *device, const VkCommandPool cmdPool, VkCommandBuffer *cmdBuf, const VkCommandBufferAllocateInfo *commandBufferInfo) {
    // 需要先完成交换链和指令缓冲池的初始化
    VkResult result;

    // 如果调用函数时提供了指令缓存分配信息参数
    if (commandBufferInfo) {
        result = vkAllocateCommandBuffers(*device, commandBufferInfo, cmdBuf);
        assert(!result);
        return;
    }

    // 默认实现需要先创建一个指令缓存的分配对象，然后将参数填入其中
    VkCommandBufferAllocateInfo cmdInfo = {};
    cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdInfo.pNext = nullptr;
    cmdInfo.commandPool = cmdPool;
    cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdInfo.commandBufferCount = (uint32_t) sizeof(cmdBuf) / sizeof(VkCommandBuffer);

    result = vkAllocateCommandBuffers(*device, &cmdInfo, cmdBuf);
    assert(!result);
}

void CommandBufferMgr::beginCommandBuffer(VkCommandBuffer cmdBuf, VkCommandBufferBeginInfo *inCmdBufInfo) {
    // 需要先调用initializeCommandBuffer()
    VkResult result;
    // 如果用户设置了自定义指令缓存
    if (inCmdBufInfo) {
        result = vkBeginCommandBuffer(cmdBuf, inCmdBufInfo);
        assert(result == VK_SUCCESS);
        return;
    }

    // 否则，使用默认方案
    VkCommandBufferInheritanceInfo cmdBufInheritInfo = {};
    cmdBufInheritInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    cmdBufInheritInfo.pNext = nullptr;
    cmdBufInheritInfo.renderPass = VK_NULL_HANDLE;
    cmdBufInheritInfo.subpass = 0;
    cmdBufInheritInfo.framebuffer = VK_NULL_HANDLE;
    cmdBufInheritInfo.occlusionQueryEnable = VK_FALSE;
    cmdBufInheritInfo.queryFlags = 0;
    cmdBufInheritInfo.pipelineStatistics = 0;

    VkCommandBufferBeginInfo cmdBufInfo = {};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;
    cmdBufInfo.flags = 0;
    cmdBufInfo.pInheritanceInfo = &cmdBufInheritInfo;

    result = vkBeginCommandBuffer(cmdBuf, &cmdBufInfo);

    assert(result == VK_SUCCESS);
}

void CommandBufferMgr::endCommandBuffer(VkCommandBuffer commandBuffer) {
    VkResult result;
    result = vkEndCommandBuffer(commandBuffer);
    assert(result == VK_SUCCESS);
}

void CommandBufferMgr::submitCommandBuffer(const VkQueue &queue, const VkCommandBuffer *commandBuffer, const VkSubmitInfo *inSubmitInfo, const VkFence &fence) {
    VkResult result;

    // 如果提交过程的控制属性可用的话，那么指令缓存已经被设置在其中了。可以忽略输入的参数
    if (inSubmitInfo) {
        result = vkQueueSubmit(queue, 1, inSubmitInfo, fence);
        assert(!result);

        result = vkQueueWaitIdle(queue);
        assert(!result);
        return;
    }

    // 否则我们需要自己创建新的提交属性结构体，并传入指令缓存对象
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = (uint32_t) sizeof(commandBuffer) / sizeof(VkCommandBuffer);
    submitInfo.pCommandBuffers = commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    result = vkQueueSubmit(queue, 1, &submitInfo, fence);
    assert(!result);

    result = vkQueueWaitIdle(queue);
    assert(!result);

    // Double check this following code need to be implemented.
    //do {
    //	res = vkWaitForFences(appObj->deviceObj->device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
    //} while (res == VK_TIMEOUT);
    //assert(res == VK_SUCCESS);

    //vkDestroyFence(appObj->deviceObj->device, drawFence, nullptr);
}