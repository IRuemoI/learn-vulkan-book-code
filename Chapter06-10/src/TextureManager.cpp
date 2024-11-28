#include "TextureManager.h"
#include "FileUtil.h"
#include "HelpFunction.h"
#include <cassert>
void setImageLayout(VkCommandBuffer cmd, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout, VkImageLayout new_image_layout, int32_t layerCount)//设置图像布局的方法
{
    VkImageMemoryBarrier image_memory_barrier = {};//构建图像内存屏障结构体实例
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.pNext = nullptr;
    image_memory_barrier.srcAccessMask = 0;
    image_memory_barrier.dstAccessMask = 0;
    image_memory_barrier.oldLayout = old_image_layout;
    image_memory_barrier.newLayout = new_image_layout;
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange.aspectMask = aspectMask;
    image_memory_barrier.subresourceRange.baseMipLevel = 0;
    image_memory_barrier.subresourceRange.levelCount = 1;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount = layerCount;//数组层的数量
    if (old_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.srcAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    }
    if (old_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    if (old_image_layout == VK_IMAGE_LAYOUT_PREINITIALIZED) {
        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
    }
    if (new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        image_memory_barrier.srcAccessMask =
                VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    }
    if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.dstAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.dstAccessMask =
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    vkCmdPipelineBarrier(cmd, src_stages, dest_stages, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
}
std::vector<std::string> TextureManager::texNames = {"../textures/0610_vulkan.bntexa"};
std::vector<VkSampler> TextureManager::samplerList;
std::map<std::string, VkImage> TextureManager::textureImageList;
std::map<std::string, VkDeviceMemory> TextureManager::textureMemoryList;
std::map<std::string, VkImageView> TextureManager::viewTextureList;
std::map<std::string, VkDescriptorImageInfo> TextureManager::texImageInfoList;
void TextureManager::initSampler(VkDevice &device, VkPhysicalDevice &gpu) {
    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.mipLodBias = 0.0;
    samplerCreateInfo.anisotropyEnable = VK_FALSE;
    samplerCreateInfo.maxAnisotropy = 1;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerCreateInfo.minLod = 0.0;
    samplerCreateInfo.maxLod = 0.0;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    for (int i = 0; i < SAMPLER_COUNT; i++) {
        VkSampler samplerTexture;
        VkResult result = vkCreateSampler(device, &samplerCreateInfo, nullptr, &samplerTexture);
        samplerList.push_back(samplerTexture);
    }
}
void TextureManager::initTextures(VkDevice &device, VkPhysicalDevice &gpu, VkPhysicalDeviceMemoryProperties &memoryProperties, VkCommandBuffer &cmdBuffer, VkQueue &queueGraphics) {
    initSampler(device, gpu);
    for (int i = 0; i < texNames.size(); i++) {
        TexArrayDataObject *ctdo = FileUtil::load2DArrayTexData(texNames[i]);
        printf("%s w %d h %d l %d\n", texNames[i].c_str(), ctdo->width, ctdo->height, ctdo->length);
        init_SPEC_2DArray_Textures(texNames[i], device, gpu, memoryProperties, cmdBuffer, queueGraphics, VK_FORMAT_R8G8B8A8_UNORM, ctdo);
    }
}
void TextureManager::init_SPEC_2DArray_Textures(std::string texName, VkDevice &device, VkPhysicalDevice &gpu, VkPhysicalDeviceMemoryProperties &memoryProperties, VkCommandBuffer &cmdBuffer, VkQueue &queueGraphics, VkFormat format, TexArrayDataObject *ctdo) {
    /*创建缓冲，将纹理数据首先搞进缓冲，然后传输进纹理*/
    VkBuffer stagingBuffer;
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCreateInfo.size = ctdo->dataByteCount;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.flags = 0;
    VkResult result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &stagingBuffer);
    assert(result == VK_SUCCESS);
    VkMemoryRequirements memReqs = {};
    vkGetBufferMemoryRequirements(device, stagingBuffer, &memReqs);
    VkMemoryAllocateInfo memAllocInfo = {};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.pNext = nullptr;
    memAllocInfo.memoryTypeIndex = 0;
    memAllocInfo.allocationSize = memReqs.size;
    memoryTypeFromProperties(memoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memAllocInfo.memoryTypeIndex);
    VkDeviceMemory stagingMemory;
    result = vkAllocateMemory(device, &memAllocInfo, nullptr, &stagingMemory);
    assert(result == VK_SUCCESS);
    result = vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0);
    assert(result == VK_SUCCESS);
    uint8_t *pData;
    vkMapMemory(device, stagingMemory, 0, memReqs.size, 0, (void **) (&pData));
    memcpy(pData, ctdo->data, memReqs.size);
    vkUnmapMemory(device, stagingMemory);
    VkImageCreateInfo image_create_info = {};//构建图像创建信息结构体实例
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;//结构体的类型
    image_create_info.pNext = nullptr;//自定义数据的指针
    image_create_info.imageType = VK_IMAGE_TYPE_2D;//图像类型
    image_create_info.format = format;//图像像素格式
    image_create_info.extent.width = ctdo->width;//图像宽度
    image_create_info.extent.height = ctdo->height;//图像高度
    image_create_info.extent.depth = 1;//图像深度
    image_create_info.mipLevels = 1;//图像 mipmap 级数
    image_create_info.arrayLayers = ctdo->length;//图像数组层数量
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_create_info.queueFamilyIndexCount = 0;
    image_create_info.pQueueFamilyIndices = nullptr;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_create_info.flags = 0;
    VkImage textureImage;
    result = vkCreateImage(device, &image_create_info, nullptr, &textureImage);
    assert(result == VK_SUCCESS);
    textureImageList[texName] = textureImage;
    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = nullptr;
    mem_alloc.allocationSize = 0;
    mem_alloc.memoryTypeIndex = 0;
    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(device, textureImage, &mem_reqs);
    mem_alloc.allocationSize = mem_reqs.size;
    printf("mem_reqs.size=%d\n", (int) (mem_reqs.size));
    bool flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits, 0, &mem_alloc.memoryTypeIndex);
    VkDeviceMemory textureMemory;
    result = vkAllocateMemory(device, &mem_alloc, nullptr, &textureMemory);
    textureMemoryList[texName] = textureMemory;
    result = vkBindImageMemory(device, textureImage, textureMemory, 0);
    assert(result == VK_SUCCESS);
    VkBufferImageCopy bufferCopyRegion = {};//构建缓冲图像拷贝结构体实例
    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;//使用方面
    bufferCopyRegion.imageSubresource.mipLevel = 0;//mipmap 级别
    bufferCopyRegion.imageSubresource.baseArrayLayer = 0;//基础数组层
    bufferCopyRegion.imageSubresource.layerCount = ctdo->length;//数组层的数量
    bufferCopyRegion.imageExtent.width = ctdo->width;
    bufferCopyRegion.imageExtent.height = ctdo->height;
    bufferCopyRegion.imageExtent.depth = 1;
    VkCommandBufferBeginInfo cmd_buf_info = {};
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buf_info.pNext = nullptr;
    cmd_buf_info.flags = 0;
    cmd_buf_info.pInheritanceInfo = nullptr;
    const VkCommandBuffer cmd_buffers[] = {cmdBuffer};
    VkSubmitInfo submit_info[1] = {};
    submit_info[0].pNext = nullptr;
    submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info[0].waitSemaphoreCount = 0;
    submit_info[0].pWaitSemaphores = VK_NULL_HANDLE;
    submit_info[0].pWaitDstStageMask = VK_NULL_HANDLE;
    submit_info[0].commandBufferCount = 1;
    submit_info[0].pCommandBuffers = cmd_buffers;
    submit_info[0].signalSemaphoreCount = 0;
    submit_info[0].pSignalSemaphores = nullptr;
    VkFenceCreateInfo fenceInfo;
    VkFence copyFence;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;
    vkCreateFence(device, &fenceInfo, nullptr, &copyFence);
    vkResetCommandBuffer(cmdBuffer, 0);
    result = vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
    setImageLayout(cmdBuffer, textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, ctdo->length);
    vkCmdCopyBufferToImage(cmdBuffer, stagingBuffer, textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);
    setImageLayout(cmdBuffer, textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, ctdo->length);
    result = vkEndCommandBuffer(cmdBuffer);
    result = vkQueueSubmit(queueGraphics, 1, submit_info, copyFence);
    do {
        result = vkWaitForFences(device, 1, &copyFence, VK_TRUE, 100000000);
    } while (result == VK_TIMEOUT);
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingMemory, nullptr);
    vkDestroyFence(device, copyFence, nullptr);
    VkImageViewCreateInfo view_info = {};//构建图像视图创建信息结构体实例
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;//结构的类型
    view_info.pNext = nullptr;//自定义数据的指针
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;//图像视图的类型
    view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    view_info.components.r = VK_COMPONENT_SWIZZLE_R;
    view_info.components.g = VK_COMPONENT_SWIZZLE_G;
    view_info.components.b = VK_COMPONENT_SWIZZLE_B;
    view_info.components.a = VK_COMPONENT_SWIZZLE_A;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = ctdo->length;
    view_info.image = textureImageList[texName];
    VkImageView viewTexture;
    result = vkCreateImageView(device, &view_info, nullptr, &viewTexture);
    assert(result == VK_SUCCESS);
    viewTextureList[texName] = viewTexture;
    VkDescriptorImageInfo texImageInfo;
    texImageInfo.imageView = viewTexture;
    texImageInfo.sampler = samplerList[0];
    texImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    texImageInfoList[texName] = texImageInfo;
    delete ctdo;
}
void TextureManager::destroyTextures(VkDevice &device) {
    for (int i = 0; i < SAMPLER_COUNT; i++) {
        vkDestroySampler(device, samplerList[i], nullptr);
    }
    for (int i = 0; i < texNames.size(); i++) {
        vkDestroyImageView(device, viewTextureList[texNames[i]], nullptr);
        vkDestroyImage(device, textureImageList[texNames[i]], nullptr);
        vkFreeMemory(device, textureMemoryList[texNames[i]], nullptr);
    }
}
int TextureManager::getVkDescriptorSetIndex(std::string texName) {
    int result = -1;
    for (int i = 0; i < texNames.size(); i++) {
        if (texNames[i].compare(texName.c_str()) == 0) {
            result = i;
            break;
        }
    }
    assert(result != -1);
    return result;
}