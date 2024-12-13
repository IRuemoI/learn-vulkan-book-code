#include "VulkanInstance.h"

VkResult VulkanInstance::createInstance(std::vector<const char *> &layers, std::vector<const char *> &extensionNames, char const *const appName) {
    //设置实例相关的层和扩展信息
    layerExtension->appRequestedExtensionNames = extensionNames;
    layerExtension->appRequestedLayerNames = layers;

    // 定义Vulkan应用程序的结构体
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = appName;
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION_1_2;

    // 定义Vulkan实例创建的参数结构体
    VkInstanceCreateInfo instInfo = {};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pNext = VK_NULL_HANDLE;
    instInfo.flags = 0;
    instInfo.pApplicationInfo = &appInfo;

    // 设置需要启用层的名称列表
    instInfo.enabledLayerCount = (uint32_t) layers.size();
    instInfo.ppEnabledLayerNames = !layers.empty() ? layers.data() : nullptr;

    // 设置需要启用的扩展列表
    instInfo.enabledExtensionCount = (uint32_t) extensionNames.size();
    instInfo.ppEnabledExtensionNames = !extensionNames.empty() ? extensionNames.data() : nullptr;

    VkResult result = vkCreateInstance(&instInfo, nullptr, &instance);
    assert(result == VK_SUCCESS);

    return result;
}

void VulkanInstance::destroyInstance() {
    vkDestroyInstance(instance, nullptr);//销毁实例
}
VulkanInstance::VulkanInstance() {
    this->layerExtension = new VulkanLayerAndExtension();
}

VulkanInstance::~VulkanInstance() {
}
