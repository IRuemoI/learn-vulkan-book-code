#include "TextureManager.h"
#include "FileUtil.h"
#include "HelpFunction.h"
#include <assert.h>
void setImageLayoutCube(VkCommandBuffer cmd, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout, VkImageLayout new_image_layout) {

    VkImageMemoryBarrier image_memory_barrier = {};
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
    image_memory_barrier.subresourceRange.layerCount = 6;

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
void setImageLayout(VkCommandBuffer cmd, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout, VkImageLayout new_image_layout) {
    VkImageMemoryBarrier image_memory_barrier = {};
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
    image_memory_barrier.subresourceRange.layerCount = 1;
    if (old_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
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
        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;

        image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    }
    if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    vkCmdPipelineBarrier(cmd, src_stages, dest_stages, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
}
std::vector<std::string> TextureManager::texNames = {
        "../textures/1307_skycubemap_back.bntex",
        "../textures/1307_skycubemap_down.bntex",
        "../textures/1307_skycubemap_front.bntex",
        "../textures/1307_skycubemap_right.bntex",
        "../textures/1307_skycubemap_left.bntex",
        "../textures/1307_skycubemap_up.bntex",
        "../textures/1307_flare1.bntex",
        "../textures/1307_flare2.bntex",
        "../textures/1307_flare3.bntex"};
std::vector<std::string> TextureManager::texNamesForCubeMap = {"../textures/1307_skycubemap_left.bntex"};
std::vector<std::string> TextureManager::texNamesForSkyBox = {
        "../textures/1307_skycubemap_back.bntex",
        "../textures/1307_skycubemap_down.bntex",
        "../textures/1307_skycubemap_front.bntex",
        "../textures/1307_skycubemap_right.bntex",
        "../textures/1307_skycubemap_left.bntex",
        "../textures/1307_skycubemap_up.bntex"};
std::vector<std::string> TextureManager::texNamesForGuang = {
        "../textures/1307_flare1.bntex",
        "../textures/1307_flare2.bntex",
        "../textures/1307_flare3.bntex"};
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
    samplerCreateInfo.borderColor =
            VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    for (int i = 0; i < SAMPLER_COUNT; i++) {
        VkSampler samplerTexture;
        VkResult result = vkCreateSampler(device,
                                          &samplerCreateInfo, nullptr, &samplerTexture);
        samplerList.push_back(samplerTexture);
    }
}
void TextureManager::init_SPEC_2D_Textures(std::string texName, VkDevice &device, VkPhysicalDevice &gpu, VkPhysicalDeviceMemoryProperties &memoryProperties, VkCommandBuffer &cmdBuffer, VkQueue &queueGraphics, VkFormat format, TexDataObject *ctdo) {
    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &formatProps);
    bool needStaging = (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)) ? true : false;
    printf("TextureManager %s", (needStaging ? "不能使用线性瓦片纹理" : "能使用线性瓦片纹理"));

    if (needStaging) {
        VkBuffer tempBuf;
        VkBufferCreateInfo buf_info = {};
        buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buf_info.pNext = nullptr;
        buf_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        buf_info.size = ctdo->dataByteCount;
        buf_info.queueFamilyIndexCount = 0;
        buf_info.pQueueFamilyIndices = nullptr;
        buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        buf_info.flags = 0;
        VkResult result = vkCreateBuffer(device, &buf_info, nullptr, &tempBuf);
        assert(result == VK_SUCCESS);
        VkMemoryRequirements mem_reqs;
        vkGetBufferMemoryRequirements(device, tempBuf, &mem_reqs);
        assert(ctdo->dataByteCount <= mem_reqs.size);
        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.pNext = nullptr;
        alloc_info.memoryTypeIndex = 0;
        alloc_info.allocationSize = mem_reqs.size;
        VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        bool flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits,
                                             requirements_mask, &alloc_info.memoryTypeIndex);
        if (flag) {
            printf("确定内存类型成功 类型索引为%d", alloc_info.memoryTypeIndex);
        } else {
            printf("确定内存类型失败!");
        }
        VkDeviceMemory memTemp;
        result = vkAllocateMemory(device, &alloc_info, nullptr, &memTemp);
        assert(result == VK_SUCCESS);
        uint8_t *pData;
        result = vkMapMemory(device, memTemp,
                             0, mem_reqs.size, 0, (void **) &pData);
        assert(result == VK_SUCCESS);
        memcpy(pData, ctdo->data, ctdo->dataByteCount);
        vkUnmapMemory(device, memTemp);
        result = vkBindBufferMemory(device, tempBuf, memTemp, 0);
        assert(result == VK_SUCCESS);

        VkImageCreateInfo image_create_info = {};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.pNext = nullptr;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.format = format;
        image_create_info.extent.width = ctdo->width;
        image_create_info.extent.height = ctdo->height;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = 1;
        image_create_info.arrayLayers = 1;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VK_IMAGE_USAGE_TRANSFER_DST_BIT;
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
        vkGetImageMemoryRequirements(device, textureImage, &mem_reqs);
        mem_alloc.allocationSize = mem_reqs.size;
        flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        &mem_alloc.memoryTypeIndex);
        VkDeviceMemory textureMemory;
        result = vkAllocateMemory(device, &mem_alloc, nullptr, &(textureMemory));
        textureMemoryList[texName] = textureMemory;
        result = vkBindImageMemory(device, textureImage, textureMemory, 0);

        VkBufferImageCopy bufferCopyRegion = {};
        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferCopyRegion.imageSubresource.mipLevel = 0;
        bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageExtent.width = ctdo->width;
        bufferCopyRegion.imageExtent.height = ctdo->height;
        bufferCopyRegion.imageExtent.depth = 1;
        bufferCopyRegion.bufferOffset = 0;
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
        setImageLayout(cmdBuffer, textureImage, VK_IMAGE_ASPECT_COLOR_BIT,
                       VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        vkCmdCopyBufferToImage(cmdBuffer, tempBuf,
                               textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);
        setImageLayout(cmdBuffer, textureImage,
                       VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        result = vkEndCommandBuffer(cmdBuffer);
        result = vkQueueSubmit(queueGraphics, 1, submit_info, copyFence);
        do {
            result = vkWaitForFences(device, 1, &copyFence, VK_TRUE, 100000000);
        } while (result == VK_TIMEOUT);
        vkDestroyBuffer(device, tempBuf, nullptr);
        vkFreeMemory(device, memTemp, nullptr);
        vkDestroyFence(device, copyFence, nullptr);
    } else {
        VkImageCreateInfo image_create_info = {};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.pNext = nullptr;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.format = format;
        image_create_info.extent.width = ctdo->width;
        image_create_info.extent.height = ctdo->height;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = 1;
        image_create_info.arrayLayers = 1;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        image_create_info.queueFamilyIndexCount = 0;
        image_create_info.pQueueFamilyIndices = nullptr;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_create_info.flags = 0;
        VkImage textureImage;
        VkResult result = vkCreateImage(device, &image_create_info, nullptr, &textureImage);
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
        bool flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                             &mem_alloc.memoryTypeIndex);
        VkDeviceMemory textureMemory;
        result = vkAllocateMemory(device, &mem_alloc, nullptr, &(textureMemory));
        textureMemoryList[texName] = textureMemory;
        result = vkBindImageMemory(device, textureImage, textureMemory, 0);
        uint8_t *pData;
        vkMapMemory(device, textureMemory, 0, mem_reqs.size, 0, (void **) (&pData));
        memcpy(pData, ctdo->data, mem_reqs.size);
        vkUnmapMemory(device, textureMemory);
    }
    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.pNext = nullptr;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = format;
    view_info.components.r = VK_COMPONENT_SWIZZLE_R;
    view_info.components.g = VK_COMPONENT_SWIZZLE_G;
    view_info.components.b = VK_COMPONENT_SWIZZLE_B;
    view_info.components.a = VK_COMPONENT_SWIZZLE_A;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    view_info.image = textureImageList[texName];
    VkImageView viewTexture;
    VkResult result = vkCreateImageView(device, &view_info, nullptr, &viewTexture);
    viewTextureList[texName] = viewTexture;
    VkDescriptorImageInfo texImageInfo;
    texImageInfo.imageView = viewTexture;
    texImageInfo.sampler = samplerList[0];
    texImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    texImageInfoList[texName] = texImageInfo;
    delete ctdo;
}
void TextureManager::initCubemapTextures(std::string texName, VkDevice &device, VkPhysicalDevice &gpu, VkPhysicalDeviceMemoryProperties &memoryProperties, VkCommandBuffer &cmdBuffer, VkQueue &queueGraphics, VkFormat format, TexDataObject *ctdo) {//立方图纹理不能使用VK_IMAGE_TILING_LINEAR

    /*创建缓冲，将纹理数据首先搞进缓冲，然后传输进纹理*/
    //创建辅助缓冲
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

    //创建辅助缓冲对应的内存
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

    //将缓冲和内存绑定
    result = vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0);
    assert(result == VK_SUCCESS);

    //映射的内存地址
    uint8_t *pData;
    vkMapMemory(device, stagingMemory, 0, memReqs.size, 0, (void **) (&pData));
    //将纹理数据拷贝进显存
    memcpy(pData, ctdo->data, memReqs.size);
    //解除内存映射
    vkUnmapMemory(device, stagingMemory);

    //创建VkImageCreateInfo实例
    VkImageCreateInfo image_create_info = {};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.pNext = nullptr;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = format;
    image_create_info.extent.width = ctdo->width;
    image_create_info.extent.height = ctdo->height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 6;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_create_info.queueFamilyIndexCount = 0;
    image_create_info.pQueueFamilyIndices = nullptr;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_create_info.flags = 0;

    //创建纹理对应的Image
    VkImage textureImage;
    result = vkCreateImage(device, &image_create_info, nullptr, &textureImage);
    assert(result == VK_SUCCESS);
    textureImageList[texName] = textureImage;

    //创建VkMemoryAllocateInfo实例
    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = nullptr;
    mem_alloc.allocationSize = 0;
    mem_alloc.memoryTypeIndex = 0;

    //获取纹理Image内存需求(这部分不一定要 可以自己根据像素格式和纹理图宽度高度计算)
    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(device, textureImage, &mem_reqs);
    mem_alloc.allocationSize = mem_reqs.size;
    //获取内存类型索引
    bool flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits, 0, &mem_alloc.memoryTypeIndex);

    //分配内存
    VkDeviceMemory textureMemory;
    result = vkAllocateMemory(device, &mem_alloc, nullptr, &(textureMemory));
    assert(result == VK_SUCCESS);
    textureMemoryList[texName] = textureMemory;

    //将纹理的Image和内存绑定
    result = vkBindImageMemory(device, textureImage, textureMemory, 0);

    //设置拷贝区间
    VkBufferImageCopy bufferCopyRegion = {};
    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferCopyRegion.imageSubresource.mipLevel = 0;
    bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
    bufferCopyRegion.imageSubresource.layerCount = 6;
    bufferCopyRegion.imageExtent.width = ctdo->width;
    bufferCopyRegion.imageExtent.height = ctdo->height;
    bufferCopyRegion.imageExtent.depth = 1;

    //命令缓冲启动信息
    VkCommandBufferBeginInfo cmd_buf_info = {};
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buf_info.pNext = nullptr;
    cmd_buf_info.flags = 0;
    cmd_buf_info.pInheritanceInfo = nullptr;

    const VkCommandBuffer cmd_buffers[] = {cmdBuffer};
    //提交信息
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

    //创建执行用Fence
    VkFenceCreateInfo fenceInfo;
    VkFence copyFence;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;
    vkCreateFence(device, &fenceInfo, nullptr, &copyFence);

    //清除一级命令缓冲
    vkResetCommandBuffer(cmdBuffer, 0);
    //启动一级命令缓冲，开始接收命令
    result = vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
    //设置目标Image的布局为VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    setImageLayoutCube(cmdBuffer, textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    //拷贝纹理
    vkCmdCopyBufferToImage(cmdBuffer, stagingBuffer, textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);
    //设置目标Image的布局为VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    setImageLayoutCube(cmdBuffer, textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    //结束一级命令缓冲
    result = vkEndCommandBuffer(cmdBuffer);
    //提交给队列执行
    result = vkQueueSubmit(queueGraphics, 1, submit_info, copyFence);
    //等待执行完毕
    do {
        result = vkWaitForFences(device, 1, &copyFence, VK_TRUE, 100000000);
    } while (result == VK_TIMEOUT);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingMemory, nullptr);
    vkDestroyFence(device, copyFence, nullptr);

    //为创建纹理所需的ImageView做准备
    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.pNext = nullptr;
    view_info.image = VK_NULL_HANDLE;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    view_info.components.r = VK_COMPONENT_SWIZZLE_R;
    view_info.components.g = VK_COMPONENT_SWIZZLE_G;
    view_info.components.b = VK_COMPONENT_SWIZZLE_B;
    view_info.components.a = VK_COMPONENT_SWIZZLE_A;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 6;
    view_info.image = textureImageList[texName];
    //创建纹理所需的ImageView
    VkImageView viewTexture;
    result = vkCreateImageView(device, &view_info, nullptr, &viewTexture);
    assert(result == VK_SUCCESS);
    viewTextureList[texName] = viewTexture;

    //组织ImageInfo实例，为未来的使用做准备
    VkDescriptorImageInfo texImageInfo;
    texImageInfo.imageView = viewTexture;
    texImageInfo.sampler = samplerList[0];
    texImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    texImageInfoList[texName] = texImageInfo;

    //删除内存中的纹理数据
    delete ctdo;
}

void TextureManager::initTextures(VkDevice &device, VkPhysicalDevice &gpu, VkPhysicalDeviceMemoryProperties &memoryProperties, VkCommandBuffer &cmdBuffer, VkQueue &queueGraphics) {
    initSampler(device, gpu);
    for (int i = 0; i < texNames.size(); i++) {
        std::string texName = texNames[i];
        std::vector<string> sa;
        splitString(texName, ".", sa);
        if (sa[1].compare("bntex") == 0) {//若为非压缩RGBA8纹理格式
            TexDataObject *ctdo = FileUtil::loadCommonTexData(texName);
            if (ctdo->data == nullptr)
                printf("读取到的纹理数据为空!!!!!!!!!!!!!!!!!");
            init_SPEC_2D_Textures(texName, device, gpu, memoryProperties, cmdBuffer, queueGraphics, VK_FORMAT_R8G8B8A8_UNORM, ctdo);
        } else if (sa[1].compare("bntexcube") == 0) {//若为立方图纹理
            TexDataObject *ctdo = FileUtil::loadCubemapTexData(texName);
            printf("%s w %d h %d c %d", texName.c_str(), ctdo->width, ctdo->height, ctdo->dataByteCount);
            initCubemapTextures(texName, device, gpu, memoryProperties, cmdBuffer, queueGraphics, VK_FORMAT_R8G8B8A8_UNORM, ctdo);
        }
    }
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
int getVkDescriptorSetIndexGeneral(std::string texName, std::vector<std::string> &texNames) {
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
int TextureManager::getVkDescriptorSetIndexForGuang(std::string texName) {
    return getVkDescriptorSetIndexGeneral(texName, texNamesForGuang);
}

int TextureManager::getVkDescriptorSetIndexForCubemap(std::string texName) {
    return getVkDescriptorSetIndexGeneral(texName, texNamesForCubeMap);
}
int TextureManager::getVkDescriptorSetIndexForSkyBox(std::string texName) {
    return getVkDescriptorSetIndexGeneral(texName, texNamesForSkyBox);
}
