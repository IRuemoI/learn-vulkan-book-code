#include "VulkanApplication.h"
#include "VulkanDrawable.h"

std::unique_ptr<VulkanApplication> VulkanApplication::instance;
std::once_flag VulkanApplication::onlyOnce;

extern std::vector<const char *> instanceExtensionNames;
extern std::vector<const char *> layerNames;
extern std::vector<const char *> deviceExtensionNames;


// 负责枚举实例层的应用程序构造函数
VulkanApplication::VulkanApplication() {
    // 在应用程序启动时，枚举实例中的层
    instanceObj.layerExtension->getInstanceLayerProperties();

    deviceObj = nullptr;
    debugFlag = false;
    rendererObj = nullptr;
    isPrepared = false;
    isResizing = false;
}

VulkanApplication::~VulkanApplication() {
    delete rendererObj;
    rendererObj = nullptr;
}

// Returns the Single ton object of VulkanApplication
VulkanApplication *VulkanApplication::GetInstance() {
    std::call_once(onlyOnce, []() { instance.reset(new VulkanApplication()); });
    return instance.get();
}

VkResult VulkanApplication::createVulkanInstance(std::vector<const char *> &layers, std::vector<const char *> &extensionNames, const char *applicationName) {
    return instanceObj.createInstance(layers, extensionNames, applicationName);
}

// 设备握手函数负责创建逻辑设备。创建逻辑设备的流程如下：
// 1. 获取物理设备特定的层和相应的扩展[可选]
// 2. 创建用户定义的VulkanDevice对象
// 3. 提供需要在此物理设备中启用的层和扩展列表
// 4. 获取物理设备或GPU属性
// 5. 从物理设备或GPU获取内存属性
// 6. 查询物理设备公开队列及其相关属性
// 7. 获取图形队列的句柄
// 8. 创建逻辑设备，将其连接到图形队列

// 创建逻辑设备和队列的高层函数
VkResult VulkanApplication::handShakeWithDevice(VkPhysicalDevice *gpu, std::vector<const char *> &layers, std::vector<const char *> &extensions) {

    // 用户定义一了Vulkan设备对象，它负责管理物理和逻辑设备，也负责管理它包含的队列和属性
    deviceObj = new VulkanDevice(gpu);
    if (!deviceObj) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    // 输出设备级别可用的层以及他们的扩展信息
    deviceObj->layerExtension.getDeviceExtensionProperties(gpu);

    // 获取物理设备或者GPU属性
    vkGetPhysicalDeviceProperties(*gpu, &deviceObj->gpuProps);

    // 获取物理设备或者GPU的内存属性
    vkGetPhysicalDeviceMemoryProperties(*gpu, &deviceObj->memoryProperties);

    // 获取物理设备端可用的队列及其属性
    deviceObj->getPhysicalDeviceQueuesAndProperties();

    // 获取能够支持图形作业(或者说运算)流水线的队列
    deviceObj->getGraphicsQueueHandle();

    // 创建逻辑设备，确保设备已经关联到一个图形队列中
    return deviceObj->createDevice(layers, extensions);
}

VkResult VulkanApplication::enumeratePhysicalDevices(std::vector<VkPhysicalDevice> &gpuList) {
    // 记录GPU数量
    uint32_t gpuDeviceCount;

    // 获取GPU数量
    VkResult result = vkEnumeratePhysicalDevices(instanceObj.instance, &gpuDeviceCount, nullptr);
    assert(result == VK_SUCCESS);

    assert(gpuDeviceCount);

    // 给GPU对象列表分配空间
    gpuList.resize(gpuDeviceCount);

    // 获取物理设备对象
    result = vkEnumeratePhysicalDevices(instanceObj.instance, &gpuDeviceCount, gpuList.data());
    assert(result == VK_SUCCESS);

    return result;
}

void VulkanApplication::initialize() {
    char title[] = "Hello World!!!";

    // 检查所提供的层是否支持
    instanceObj.layerExtension->areLayersSupported(layerNames);

    // 使用指定的层和扩展名称创建Vulkan实例
    createVulkanInstance(layerNames, instanceExtensionNames, title);

    // 如果打开了调试开关，则创建调试报告
    if (debugFlag) {
        instanceObj.layerExtension->setupDebugMessenger();
    }

    // 获取物理设备列表
    enumeratePhysicalDevices(gpuList);

    // 在这个例子中我们仅仅使用第一个可用的设备
    if (!gpuList.empty()) {
        // 创建逻辑设备，绑定图形队列
        handShakeWithDevice(&gpuList[0], layerNames, deviceExtensionNames);
    }
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(gpuList[0], &deviceProperties);
    std::cout << "Vulkan使用的物理设备：" << deviceProperties.deviceName << std::endl;

    if (!rendererObj) {
        rendererObj = new VulkanRenderer(this, deviceObj);
        // 创建一个500x500的空白窗口
        rendererObj->createPresentationWindow(500, 500);
        // 初始化交换链
        rendererObj->getSwapChain()->initializeSwapChain();
    }
    rendererObj->initialize();
}

void VulkanApplication::resize() {
    // If prepared then only proceed for
    if (!isPrepared) {
        return;
    }

    isResizing = true;

    vkDeviceWaitIdle(deviceObj->device);
    rendererObj->destroyFramebuffers();
    rendererObj->destroyCommandPool();
    rendererObj->destroyPipeline();
    rendererObj->getPipelineObject()->destroyPipelineCache();
    for (VulkanDrawable *drawableObj: *rendererObj->getDrawingItems()) {
        drawableObj->destroyDescriptor();
    }
    rendererObj->destroyRenderpass();
    rendererObj->getSwapChain()->destroySwapChain();
    rendererObj->destroyDrawableVertexBuffer();
    rendererObj->destroyDrawableUniformBuffer();
    rendererObj->destroyTextureResource();
    rendererObj->destroyDepthBuffer();
    rendererObj->initialize();
    prepare();

    isResizing = false;
}

void VulkanApplication::deInitialize() {
    // Destroy all the pipeline objects
    rendererObj->destroyPipeline();

    // Destroy the associate pipeline cache
    rendererObj->getPipelineObject()->destroyPipelineCache();

    for (VulkanDrawable *drawableObj: *rendererObj->getDrawingItems()) {
        drawableObj->destroyDescriptor();
    }

    rendererObj->getShader()->destroyShaders();
    rendererObj->destroyFramebuffers();
    rendererObj->destroyRenderpass();
    rendererObj->destroyDrawableVertexBuffer();
    rendererObj->destroyDrawableUniformBuffer();

    rendererObj->destroyDrawableCommandBuffer();
    rendererObj->destroyDepthBuffer();
    rendererObj->getSwapChain()->destroySwapChain();
    rendererObj->destroyCommandBuffer();
    rendererObj->destroyDrawableSynchronizationObjects();
    rendererObj->destroyCommandPool();
    rendererObj->destroyPresentationWindow();
    rendererObj->destroyTextureResource();
    deviceObj->destroyDevice();
    if (debugFlag) {
        instanceObj.layerExtension->DestroyDebugUtilsMessengerEXT();
    }
    instanceObj.destroyInstance();
}

void VulkanApplication::prepare() {
    isPrepared = false;
    rendererObj->prepare();
    isPrepared = true;
}

void VulkanApplication::update() {
    rendererObj->update();
}

bool VulkanApplication::render() {
    if (!isPrepared)
        return false;

    return rendererObj->render();
}
