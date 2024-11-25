#pragma once
#include "Headers.h"

/***************指令缓存封装类***************/
class CommandBufferMgr {
public:
    // 从指令池分配内存到指令缓存
    static void allocCommandBuffer(const VkDevice *device, const VkCommandPool cmdPool, VkCommandBuffer *cmdBuf, const VkCommandBufferAllocateInfo *commandBufferInfo = nullptr);
    // 开始指令缓存的录制
    static void beginCommandBuffer(VkCommandBuffer cmdBuf, VkCommandBufferBeginInfo *inCmdBufInfo = nullptr);
    // 结束指令缓存的录制
    static void endCommandBuffer(VkCommandBuffer cmdBuf);
    // 将指令缓存提交到执行部分
    static void submitCommandBuffer(const VkQueue &queue, const VkCommandBuffer *cmdBufList, const VkSubmitInfo *submitInfo = nullptr, const VkFence &fence = VK_NULL_HANDLE);
};

void *readFile(const char *spvFileName, size_t *fileSize);
