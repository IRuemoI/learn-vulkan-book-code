#pragma once
#include "Headers.h"

struct LayerProperties
{
    VkLayerProperties properties;
    std::vector<VkExtensionProperties> extensions;
};

class VulkanLayerAndExtension {
public:
    VulkanLayerAndExtension();
    ~VulkanLayerAndExtension();

    /******* 层和扩展相关的成员函数和变量 *******/

    // 应用程序请求的层名称列表
    std::vector<const char *> appRequestedLayerNames;

    // 应用程序请求的扩展名称列表
    std::vector<const char *> appRequestedExtensionNames;

    // 本机(实例级别和设备级别)拥有的层和其中相关的扩展列表
    std::vector<LayerProperties> layerPropertyList;

    // 获取实例(全局)级别的层属性
    VkResult getInstanceLayerProperties();

    // 全局扩展
    VkResult getExtensionProperties(LayerProperties &layerProps, VkPhysicalDevice *gpu = nullptr);

    // 设备级别的扩展
    VkResult getDeviceExtensionProperties(VkPhysicalDevice *gpu);

    /******* Vukan调试相关的成员函数和变量 *******/
    VkBool32 areLayersSupported(std::vector<const char *> &layerNames);
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

private:
    VkDebugUtilsMessengerEXT debugUtilsMessengerCallback{};

public:
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {};
    void setupDebugMessenger();
};