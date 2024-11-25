#include "TextureManager.h"
#include "FileUtil.h"
#include "HelpFunction.h"
#include <assert.h>
std::vector<std::string> TextureManager::texNames = {"../textures/1706_haimian.bntex"};

bool TextureManager::needStaging;
std::vector<VkSampler> TextureManager::samplerList;
std::map<std::string, VkImage> TextureManager::textureImageList;
std::map<std::string, VkDeviceMemory> TextureManager::textureMemoryList;
std::map<std::string, VkImageView> TextureManager::viewTextureList;
std::map<std::string, VkDescriptorImageInfo> TextureManager::texImageInfoList;

std::map<std::string, VkImage> TextureManager::textureImageListHelp;
std::map<std::string, VkDeviceMemory> TextureManager::textureMemoryListHelp;

void TextureManager::initTextures(VkDevice &device, VkPhysicalDevice &gpu, VkPhysicalDeviceMemoryProperties &memoryProperties, VkCommandBuffer &cmdBuffer, VkQueue &queueGraphics) {

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
    samplerCreateInfo.maxAnisotropy = 0;
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


    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(gpu, VK_FORMAT_R8G8B8A8_UNORM, &formatProps);


    needStaging = (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)) ? true : false;
    printf("TextureManager %s\n", (needStaging ? "不能使用线性瓦片纹理" : "能使用线性瓦片纹理"));


    for (int i = 0; i < texNames.size(); i++) {

        CommonTexDataObject *ctdo = FileUtil::loadCommonTexData(texNames[i]);
        printf("%s w %d h %d\n", texNames[i].c_str(), ctdo->width, ctdo->height);


        VkImageCreateInfo image_create_info = {};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.pNext = nullptr;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
        image_create_info.extent.width = ctdo->width;
        image_create_info.extent.height = ctdo->height;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = 1;
        image_create_info.arrayLayers = 1;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        image_create_info.usage = needStaging ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : VK_IMAGE_USAGE_SAMPLED_BIT;
        image_create_info.queueFamilyIndexCount = 0;
        image_create_info.pQueueFamilyIndices = nullptr;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_create_info.flags = 0;


        VkMemoryAllocateInfo mem_alloc = {};
        mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mem_alloc.pNext = nullptr;
        mem_alloc.allocationSize = 0;
        mem_alloc.memoryTypeIndex = 0;


        VkImage textureImage;
        VkResult result = vkCreateImage(device, &image_create_info, nullptr, &textureImage);
        if (needStaging) {
            textureImageListHelp[texNames[i]] = textureImage;
        } else {
            textureImageList[texNames[i]] = textureImage;
        }


        VkMemoryRequirements mem_reqs;
        vkGetImageMemoryRequirements(device, textureImage, &mem_reqs);
        mem_alloc.allocationSize = mem_reqs.size;

        bool flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &mem_alloc.memoryTypeIndex);


        VkDeviceMemory textureMemory;
        result = vkAllocateMemory(device, &mem_alloc, nullptr, &(textureMemory));
        if (needStaging) {
            textureMemoryListHelp[texNames[i]] = textureMemory;
        } else {
            textureMemoryList[texNames[i]] = textureMemory;
        }


        result = vkBindImageMemory(device, textureImage, textureMemory, 0);


        uint8_t *pData;
        vkMapMemory(device, textureMemory, 0, mem_reqs.size, 0, (void **) (&pData));

        memcpy(pData, ctdo->data, mem_reqs.size);

        vkUnmapMemory(device, textureMemory);

        if (needStaging) {
            image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            VkImage textureImageStage;
            result = vkCreateImage(device, &image_create_info, nullptr, &textureImageStage);
            textureImageList[texNames[i]] = textureImageStage;
            assert(result == VK_SUCCESS);
            vkGetImageMemoryRequirements(device, textureImageStage, &mem_reqs);
            mem_alloc.allocationSize = mem_reqs.size;
            flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex);
            assert(flag);
            VkDeviceMemory textureMemoryStage;
            result = vkAllocateMemory(device, &mem_alloc, nullptr, &textureMemoryStage);
            textureMemoryList[texNames[i]] = textureMemoryStage;
            assert(result == VK_SUCCESS);
            result = vkBindImageMemory(device, textureImageStage, textureMemoryStage, 0);
            assert(result == VK_SUCCESS);

            VkImageCopy copy_region;
            copy_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy_region.srcSubresource.mipLevel = 0;
            copy_region.srcSubresource.baseArrayLayer = 0;
            copy_region.srcSubresource.layerCount = 1;
            copy_region.srcOffset.x = 0;
            copy_region.srcOffset.y = 0;
            copy_region.srcOffset.z = 0;
            copy_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy_region.dstSubresource.mipLevel = 0;
            copy_region.dstSubresource.baseArrayLayer = 0;
            copy_region.dstSubresource.layerCount = 1;
            copy_region.dstOffset.x = 0;
            copy_region.dstOffset.y = 0;
            copy_region.dstOffset.z = 0;
            copy_region.extent.width = ctdo->width;
            copy_region.extent.height = ctdo->height;
            copy_region.extent.depth = 1;


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
            VkFence drawFence;
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.pNext = nullptr;
            fenceInfo.flags = 0;
            vkCreateFence(device, &fenceInfo, nullptr, &drawFence);


            vkResetCommandBuffer(cmdBuffer, 0);

            result = vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);

            vkCmdCopyImage(cmdBuffer, textureImageListHelp[texNames[i]], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, textureImageList[texNames[i]], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

            result = vkEndCommandBuffer(cmdBuffer);

            result = vkQueueSubmit(queueGraphics, 1, submit_info, drawFence);

            do {
                result = vkWaitForFences(device, 1, &drawFence, VK_TRUE, 100000000);
            } while (result == VK_TIMEOUT);
        }


        VkImageViewCreateInfo view_info = {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.pNext = nullptr;
        view_info.image = VK_NULL_HANDLE;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
        view_info.components.r = VK_COMPONENT_SWIZZLE_R;
        view_info.components.g = VK_COMPONENT_SWIZZLE_G;
        view_info.components.b = VK_COMPONENT_SWIZZLE_B;
        view_info.components.a = VK_COMPONENT_SWIZZLE_A;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;
        view_info.image = textureImageList[texNames[i]];

        VkImageView viewTexture;
        result = vkCreateImageView(device, &view_info, nullptr, &viewTexture);
        viewTextureList[texNames[i]] = viewTexture;


        VkDescriptorImageInfo texImageInfo;
        texImageInfo.imageView = viewTexture;
        texImageInfo.sampler = samplerList[0];
        texImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        texImageInfoList[texNames[i]] = texImageInfo;


        delete ctdo;
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
        if (needStaging) {
            vkDestroyImage(device, textureImageListHelp[texNames[i]], nullptr);
            vkFreeMemory(device, textureMemoryListHelp[texNames[i]], nullptr);
        }
    }
}

int TextureManager::getVkDescriptorSetIndexForCommonTexLight(std::string texName) {
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
