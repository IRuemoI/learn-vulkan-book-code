#pragma once
#include <map>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#define SAMPLER_COUNT 1
class TextureManager {
private:
    static std::vector<std::string> texNames;

public:
    static std::vector<std::string> texNamesSingle;
    static std::vector<std::string> texNamesPair;
    static bool needStaging;
    static std::vector<VkSampler> samplerList;
    static std::map<std::string, VkImage> textureImageList;
    static std::map<std::string, VkDeviceMemory> textureMemoryList;
    static std::map<std::string, VkImage> textureImageListHelp;
    static std::map<std::string, VkDeviceMemory> textureMemoryListHelp;
    static std::map<std::string, VkImageView> viewTextureList;
    static std::map<std::string, VkDescriptorImageInfo> texImageInfoList;
    static void initTextures(VkDevice &device, VkPhysicalDevice &gpu, VkPhysicalDeviceMemoryProperties &memoryProperties, VkCommandBuffer &cmdBuffer, VkQueue &queueGraphics);
    static void destroyTextures(VkDevice &device);
    static int getVkDescriptorSetIndexForCommonTexLight(std::string texName);
};