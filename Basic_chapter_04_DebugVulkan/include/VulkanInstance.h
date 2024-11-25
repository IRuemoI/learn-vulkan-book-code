#pragma once

#include "VulkanLED.h"

class VulkanInstance {
public:
    VulkanInstance();
    ~VulkanInstance();

public:
    // Vulkan实例成员变量
    VkInstance instance;

    // Vulkan实例相关的层和扩展
    VulkanLayerAndExtension *layerExtension;

public:// VulkanInstance类内公开函数
    VkResult createInstance(std::vector<const char *> &layers, std::vector<const char *> &extensions, const char *applicationName);
    void destroyInstance();
};