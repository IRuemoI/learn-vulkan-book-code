#pragma once
#include "TexDataObject.h"
#include <map>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#define SAMPLER_COUNT 2
class TextureManager {
public:
    static std::vector<std::string> texNames;
    static std::vector<std::string> texNamesForWater;
    static std::vector<std::string> texNamesForObj;
    static std::vector<VkSampler> samplerList;
    static std::map<std::string, VkImage> textureImageList;
    static std::map<std::string, VkDeviceMemory> textureMemoryList;
    static std::map<std::string, VkImageView> viewTextureList;
    static std::map<std::string, VkDescriptorImageInfo> texImageInfoList;
    static void initTextures(VkDevice &device, VkPhysicalDevice &gpu, VkPhysicalDeviceMemoryProperties &memoryProperties, VkCommandBuffer &cmdBuffer, VkQueue &queueGraphics);
    static void destroyTextures(VkDevice &device);
    static int getVkDescriptorSetIndex(std::string texName);

private:
    static void initSampler(VkDevice &device, VkPhysicalDevice &gpu);
    static void init_SPEC_2D_Textures(std::string texName, VkDevice &device, VkPhysicalDevice &gpu, VkPhysicalDeviceMemoryProperties &memoryProperties, VkCommandBuffer &cmdBuffer, VkQueue &queueGraphics, VkFormat format, TexDataObject *ctdo);
};