#pragma once

#include "Headers.h"
//#include "VulkanQueue.h"
#include "VulkanLED.h"

class VulkanApplication;


// Vulkan暴露一个或多个设备，每个设备都暴露一个或多个队列，这些队列可以彼此异步处理作业。
// 设备支持的队列被分为多个系列，每个系列支持一种或多种功能类型，并且可以包含具有相似特征的多个队列。
// 单个队列族中的队列被认为是相互兼容的，并且为队列族生成的作业可以在该队列族中的任何队列上执行
class VulkanDevice {
public:
    VulkanDevice(VkPhysicalDevice *gpu);
    ~VulkanDevice();

public:
    VkDevice device;// 逻辑设备
    VkPhysicalDevice *gpu;// 物理设备
    VkPhysicalDeviceProperties gpuProps;// 物理设备属性
    VkPhysicalDeviceMemoryProperties memoryProperties;//物理设备内存属性

public:
    // 队列
    VkQueue queue;// Vulkan队列对象
    std::vector<VkQueueFamilyProperties> queueFamilyProps;//存储物理设备暴露的所有队列族属性
    uint32_t graphicsQueueIndex;// 存储图形队列索引
    uint32_t graphicsQueueWithPresentIndex;// 设备暴露的队列族索引，支持图形和呈现
    uint32_t queueFamilyCount;// Device specific layer and extensions

    //层和扩展
    VulkanLayerAndExtension layerExtension;

public:
    VkResult createDevice(std::vector<const char *> &layers, std::vector<const char *> &extensions);
    void destroyDevice();

    bool memoryTypeFromProperties(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);

    // 获取物理设备公开的可用队列
    void getPhysicalDeviceQueuesAndProperties();

    // 查询物理设备获取队列属性
    uint32_t getGraphicsQueueHandle();

    // Queue related member functions.
    void getDeviceQueue();
};