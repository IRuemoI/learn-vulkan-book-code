#include "VulkanDevice.h"
#include "VulkanApplication.h"
#include "VulkanInstance.h"

VulkanDevice::VulkanDevice(VkPhysicalDevice *physicalDevice) {
    gpu = physicalDevice;
}

VulkanDevice::~VulkanDevice() {
}

// 注：这个函数要求队列对象已经就绪
VkResult VulkanDevice::createDevice(std::vector<const char *> &layers, std::vector<const char *> &extensions) {
    layerExtension.appRequestedLayerNames = layers;
    layerExtension.appRequestedExtensionNames = extensions;

    // 使用可用的队列信息创建逻辑设备

    VkResult result;
    float queuePriorities[1] = {0.0};
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.queueFamilyIndex = graphicsQueueIndex;
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = nullptr;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = nullptr;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = nullptr;// 设备层被废弃
    deviceInfo.enabledExtensionCount = (uint32_t) extensions.size();
    deviceInfo.ppEnabledExtensionNames = !extensions.empty() ? extensions.data() : nullptr;
    deviceInfo.pEnabledFeatures = nullptr;

    result = vkCreateDevice(*gpu, &deviceInfo, nullptr, &device);
    assert(result == VK_SUCCESS);

    return result;
}

/// 检查请求的内存类型(参数一)并将其填入参数三
bool VulkanDevice::memoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t *typeIndex) {
    // 循环确定内存类型索引
    for (uint32_t i = 0; i < 32; i++) {
        // 若对应类型比特位为1
        if ((typeBits & 1) == 1) {
            // 类型可用，检查否匹配用户请求的属性
            if ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // 没有匹配的内存类型，返回为false
    return false;
}

void VulkanDevice::getPhysicalDeviceQueuesAndProperties() {
    // 第二个参数传入NULL，从而查询队列族的数量
    vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queueFamilyCount, nullptr);

    // 分配数组空间保存队列属性信息
    queueFamilyProps.resize(queueFamilyCount);

    // 获取队列族的属性
    vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queueFamilyCount, queueFamilyProps.data());
}

uint32_t VulkanDevice::getGraphicsQueueHandle() {

    bool found = false;
    // 1. 迭代查找物理设备所支持的所有队列
    for (unsigned int i = 0; i < queueFamilyCount; i++) {
        // 2. 获取图形类型的队列
        // 物理设备支持的四种类型的队列或队列族
        // 物理队列		- VK_QUEUE_GRAPHICS_BIT
        // 计算队列		- VK_QUEUE_COMPUTE_BIT
        // DMA/传输队列	- VK_QUEUE_TRANSFER_BIT
        // 稀疏内存		- VK_QUEUE_SPARSE_BINDING_BIT

        if (queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            // 3. 获取图像队列族的句柄/索引
            found = true;
            graphicsQueueIndex = i;
            break;
        }
    }

    // 如果没有找到队列，则使用断言结束程序
    assert(found);

    return 0;
}

void VulkanDevice::destroyDevice() {
    vkDestroyDevice(device, nullptr);
}


//Queue related functions
void VulkanDevice::getDeviceQueue() {
    //this depends on initialiing the SwapChain to get the graphics queue with presentation support
    vkGetDeviceQueue(device, graphicsQueueWithPresentIndex, 0, &queue);
}
