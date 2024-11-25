#include "Wrappers.h"
#include "VulkanApplication.h"

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
}

// ppm格式解析的实现
PpmParser::PpmParser() {
    isValid = false;
    imageWidth = 0;
    imageHeight = 0;
    ppmFile = "invalid file name";
    dataPosition = 0;
}

PpmParser::~PpmParser() {
}

int32_t PpmParser::getImageWidth() {
    return imageWidth;
}

int32_t PpmParser::getImageHeight() {
    return imageHeight;
}

bool PpmParser::getHeaderInfo(const char *filename) {
    tex2D = new gli::texture2D(gli::load(filename));
    imageHeight = static_cast<uint32_t>(tex2D[0].dimensions().x);
    imageWidth = static_cast<uint32_t>(tex2D[0].dimensions().y);
    return true;
}

bool PpmParser::loadImageData(int rowPitch, uint8_t *data) {
    uint8_t *dataTemp = (uint8_t *) tex2D->data();
    for (int y = 0; y < imageHeight; y++) {
        size_t imageSize = imageWidth * 4;
        memcpy(data, dataTemp, imageSize);
        dataTemp += imageSize;

        // 一行行的读取数据
        data += rowPitch;
    }

    return true;
}

void *readFile(const char *spvFileName, size_t *fileSize) {

    FILE *fp = fopen(spvFileName, "rb");
    if (!fp) {
        return nullptr;
    }

    size_t retval;
    long int size;

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);

    void *spvShader = malloc(size + 1);// 需要为`\0`多分配一个
    memset(spvShader, 0, size + 1);

    retval = fread(spvShader, size, 1, fp);
    assert(retval == 1);

    *fileSize = size;
    fclose(fp);
    return spvShader;
}
