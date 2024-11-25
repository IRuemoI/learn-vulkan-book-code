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

/***************TEXTURE WRAPPERS***************/
struct TextureData
{
    VkSampler sampler;
    VkImage image;
    VkImageLayout imageLayout;
    VkMemoryAllocateInfo memoryAlloc;
    VkDeviceMemory mem;
    VkImageView view;
    uint32_t mipMapLevels;
    uint32_t layerCount;
    uint32_t textureWidth, textureHeight;
    VkDescriptorImageInfo descsImgInfo;
};

/***************PPM PARSER CLASS***************/
#include "Headers.h"

class PpmParser {
public:
    PpmParser();
    ~PpmParser();
    bool getHeaderInfo(const char *filename);
    bool loadImageData(int rowPitch, uint8_t *rgba_data);
    int32_t getImageWidth();
    int32_t getImageHeight();
    const char *filename() { return ppmFile.c_str(); }

private:
    bool isValid;
    int32_t imageWidth;
    int32_t imageHeight;
    int32_t dataPosition;
    std::string ppmFile;
    gli::texture2D *tex2D;
};