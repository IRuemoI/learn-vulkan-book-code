#include "VulkanSwapChain.h"

#include "VulkanApplication.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanRenderer.h"

#define INSTANCE_FUNC_PTR(instance, entrypoint)                                                  \
    {                                                                                            \
        fp##entrypoint = (PFN_vk##entrypoint) vkGetInstanceProcAddr(instance, "vk" #entrypoint); \
        if (fp##entrypoint == nullptr) {                                                         \
            std::cout << "Unable to locate the vkGetDeviceProcAddr: vk" #entrypoint;             \
            exit(-1);                                                                            \
        }                                                                                        \
    }

#define DEVICE_FUNC_PTR(dev, entrypoint)                                                  \
    {                                                                                     \
        fp##entrypoint = (PFN_vk##entrypoint) vkGetDeviceProcAddr(dev, "vk" #entrypoint); \
        if (fp##entrypoint == nullptr) {                                                  \
            std::cout << "Unable to locate the vkGetDeviceProcAddr: vk" #entrypoint;      \
            exit(-1);                                                                     \
        }                                                                                 \
    }

VulkanSwapChain::VulkanSwapChain(VulkanRenderer *renderer) {
    rendererObj = renderer;
    appObj = VulkanApplication::GetInstance();
}

VulkanSwapChain::~VulkanSwapChain() {
    scPrivateVars.swapchainImages.clear();
    scPrivateVars.surfFormats.clear();
    scPrivateVars.presentModes.clear();
}

VkResult VulkanSwapChain::getSwapChainExtensions() {
    // 依赖于createPresentationWindow()
    VkInstance &instance = appObj->instanceObj.instance;
    VkDevice &device = appObj->deviceObj->device;

    // 获取实例级别的交换链扩展函数指针
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceSupportKHR);
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
    INSTANCE_FUNC_PTR(instance, DestroySurfaceKHR);

    // 获取设备级别的交换链扩展函数指针
    DEVICE_FUNC_PTR(device, CreateSwapchainKHR);
    DEVICE_FUNC_PTR(device, DestroySwapchainKHR);
    DEVICE_FUNC_PTR(device, GetSwapchainImagesKHR);
    DEVICE_FUNC_PTR(device, AcquireNextImageKHR);
    DEVICE_FUNC_PTR(device, QueuePresentKHR);

    return VK_SUCCESS;
}

VkResult VulkanSwapChain::createSurface() {
    VkResult result;
    // 依赖于createPresentationWindow()，需要一个空窗口句柄
    VkInstance &instance = appObj->instanceObj.instance;

    // 窗口表面描述结构
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.hinstance = rendererObj->connection;
    createInfo.hwnd = rendererObj->window;

    result = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &scPublicVars.surface);

#else// _WIN32

    VkXcbSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.connection = rendererObj->connection;
    createInfo.window = rendererObj->window;

    result = vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &surface);
#endif// _WIN32

    assert(result == VK_SUCCESS);
    return result;
}

uint32_t VulkanSwapChain::getGraphicsQueueWithPresentationSupport() {
    VulkanDevice *device = appObj->deviceObj;
    uint32_t queueCount = device->queueFamilyCount;
    VkPhysicalDevice gpu = *device->gpu;
    std::vector<VkQueueFamilyProperties> &queueProps = device->queueFamilyProps;

    // 迭代检查查每个队列，判断呈现属性状态
    VkBool32 *supportsPresent = (VkBool32 *) malloc(queueCount * sizeof(VkBool32));
    for (uint32_t i = 0; i < queueCount; i++) {
        fpGetPhysicalDeviceSurfaceSupportKHR(gpu, i, scPublicVars.surface, &supportsPresent[i]);
    }

    // 在队列族数组中搜索图形队列和呈现队列，尝试找到同时支持这两种队列的队列
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t presentQueueNodeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueCount; i++) {
        if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (graphicsQueueNodeIndex == UINT32_MAX) {
                graphicsQueueNodeIndex = i;
            }

            if (supportsPresent[i] == VK_TRUE) {
                graphicsQueueNodeIndex = i;
                presentQueueNodeIndex = i;
                break;
            }
        }
    }

    if (presentQueueNodeIndex == UINT32_MAX) {
        // 如果找不到既支持图形又支持呈现的队列，那么单独找一个呈现队列
        for (uint32_t i = 0; i < queueCount; ++i) {
            if (supportsPresent[i] == VK_TRUE) {
                presentQueueNodeIndex = i;
                break;
            }
        }
    }

    free(supportsPresent);

    // 如果无法同时找到图形和呈现队列，提示出错
    if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) {
        return UINT32_MAX;
    }

    return graphicsQueueNodeIndex;
}

void VulkanSwapChain::getSupportedFormats() {
    VkPhysicalDevice gpu = *rendererObj->getDevice()->gpu;
    VkResult result;

    // 获取支持的vkformat列表
    uint32_t formatCount;
    result = fpGetPhysicalDeviceSurfaceFormatsKHR(gpu, scPublicVars.surface, &formatCount, nullptr);
    assert(result == VK_SUCCESS);
    scPrivateVars.surfFormats.clear();
    scPrivateVars.surfFormats.resize(formatCount);

    // 从分配的对象中获取VkFormats
    result = fpGetPhysicalDeviceSurfaceFormatsKHR(gpu, scPublicVars.surface, &formatCount, &scPrivateVars.surfFormats[0]);
    assert(result == VK_SUCCESS);

    // 如果是VK_FORMAT_UNDEFINED，则surface没有首选格式。我们使用BGRA 32位格式
    if (formatCount == 1 && scPrivateVars.surfFormats[0].format == VK_FORMAT_UNDEFINED) {
        scPublicVars.format = VK_FORMAT_B8G8R8A8_UNORM;
    } else {
        assert(formatCount >= 1);
        scPublicVars.format = scPrivateVars.surfFormats[0].format;
    }
}

void VulkanSwapChain::initializeSwapChain() {
    // 查询交换链扩展
    getSwapChainExtensions();

    // 创建表面并与窗口关联
    createSurface();

    // 获取具有表示支持的图形队列
    uint32_t index = getGraphicsQueueWithPresentationSupport();
    if (index == UINT32_MAX) {
        std::cout << "无法找到一个图像和一个呈现队列\n";
        exit(-1);
    }
    rendererObj->getDevice()->graphicsQueueWithPresentIndex = index;

    // 获取支持的格式列表
    getSupportedFormats();
}

void VulkanSwapChain::createSwapChain(const VkCommandBuffer &cmd) {
    /* This function retreive swapchain image and create those images- image view */

    // 使用扩展并获得表面功能，呈现模式
    getSurfaceCapabilitiesAndPresentMode();

    // 管理交换链呈现模式
    managePresentMode();

    // 创建交换链图像
    createSwapChainColorImages();

    // 获取创建颜色图绘制表面
    createColorImageView(cmd);
}

void VulkanSwapChain::getSurfaceCapabilitiesAndPresentMode() {
    VkResult result;
    VkPhysicalDevice gpu = *appObj->deviceObj->gpu;
    result = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, scPublicVars.surface, &scPrivateVars.surfCapabilities);
    assert(result == VK_SUCCESS);

    result = fpGetPhysicalDeviceSurfacePresentModesKHR(gpu, scPublicVars.surface, &scPrivateVars.presentModeCount, nullptr);
    assert(result == VK_SUCCESS);

    scPrivateVars.presentModes.clear();
    scPrivateVars.presentModes.resize(scPrivateVars.presentModeCount);
    assert(scPrivateVars.presentModes.size() >= 1);

    result = fpGetPhysicalDeviceSurfacePresentModesKHR(gpu, scPublicVars.surface, &scPrivateVars.presentModeCount, &scPrivateVars.presentModes[0]);
    assert(result == VK_SUCCESS);

    if (scPrivateVars.surfCapabilities.currentExtent.width == (uint32_t) -1) {
        // 如果未定义表面宽度和高度，则将其设置为与图像大小相等。
        scPrivateVars.swapChainExtent.width = rendererObj->width;
        scPrivateVars.swapChainExtent.height = rendererObj->height;
    } else {
        // 如果定义了表面大小，则它必须与交换链大小匹配
        scPrivateVars.swapChainExtent = scPrivateVars.surfCapabilities.currentExtent;
    }
}

void VulkanSwapChain::managePresentMode() {
    // 优先使用邮箱模式，最低延迟并且不会产生撕裂模式。如果没有，试试IMMEDIATE，它通常是可用的，而且是最快的（尽管它会产生撕裂）。如果没有，则退回到总是可用的FIFO
    scPrivateVars.swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (size_t i = 0; i < scPrivateVars.presentModeCount; i++) {
        if (scPrivateVars.presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            scPrivateVars.swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if ((scPrivateVars.swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
            (scPrivateVars.presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
            scPrivateVars.swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    // 确定要在交换链中使用的VkImage的数量（除了正在呈现和排队等待呈现的图像外，我们希望一次只拥有一个图像）
    scPrivateVars.desiredNumberOfSwapChainImages = scPrivateVars.surfCapabilities.minImageCount + 1;
    if ((scPrivateVars.surfCapabilities.maxImageCount > 0) &&
        (scPrivateVars.desiredNumberOfSwapChainImages > scPrivateVars.surfCapabilities.maxImageCount)) {
        // 应用程序必须满足比期望更少的图像
        scPrivateVars.desiredNumberOfSwapChainImages = scPrivateVars.surfCapabilities.maxImageCount;
    }

    if (scPrivateVars.surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        scPrivateVars.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        scPrivateVars.preTransform = scPrivateVars.surfCapabilities.currentTransform;
    }
}

void VulkanSwapChain::createSwapChainColorImages() {
    VkResult result;

    VkSwapchainCreateInfoKHR swapChainInfo = {};
    swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainInfo.pNext = nullptr;
    swapChainInfo.surface = scPublicVars.surface;
    swapChainInfo.minImageCount = scPrivateVars.desiredNumberOfSwapChainImages;
    swapChainInfo.imageFormat = scPublicVars.format;
    swapChainInfo.imageExtent.width = scPrivateVars.swapChainExtent.width;
    swapChainInfo.imageExtent.height = scPrivateVars.swapChainExtent.height;
    swapChainInfo.preTransform = scPrivateVars.preTransform;
    swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainInfo.imageArrayLayers = 1;
    swapChainInfo.presentMode = scPrivateVars.swapchainPresentMode;
    swapChainInfo.oldSwapchain = VK_NULL_HANDLE;
    swapChainInfo.clipped = true;
    swapChainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainInfo.queueFamilyIndexCount = 0;
    swapChainInfo.pQueueFamilyIndices = nullptr;

    result = fpCreateSwapchainKHR(rendererObj->getDevice()->device, &swapChainInfo, nullptr, &scPublicVars.swapChain);
    assert(result == VK_SUCCESS);

    // 创建交换链对象
    result = fpGetSwapchainImagesKHR(rendererObj->getDevice()->device, scPublicVars.swapChain, &scPublicVars.swapchainImageCount, nullptr);
    assert(result == VK_SUCCESS);

    scPrivateVars.swapchainImages.clear();
    // 获取交换链中图像的数量
    scPrivateVars.swapchainImages.resize(scPublicVars.swapchainImageCount);
    assert(!scPrivateVars.swapchainImages.empty());

    // 获取交换链图像
    result = fpGetSwapchainImagesKHR(rendererObj->getDevice()->device, scPublicVars.swapChain, &scPublicVars.swapchainImageCount, &scPrivateVars.swapchainImages[0]);
    assert(result == VK_SUCCESS);
}

void VulkanSwapChain::createColorImageView(const VkCommandBuffer &cmd) {
    VkResult result;
    scPublicVars.colorBuffer.clear();
    for (uint32_t i = 0; i < scPublicVars.swapchainImageCount; i++) {
        SwapChainBuffer sc_buffer;

        VkImageViewCreateInfo imgViewInfo = {};
        imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imgViewInfo.pNext = nullptr;
        imgViewInfo.format = scPublicVars.format;
        imgViewInfo.components = {VK_COMPONENT_SWIZZLE_IDENTITY};
        imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgViewInfo.subresourceRange.baseMipLevel = 0;
        imgViewInfo.subresourceRange.levelCount = 1;
        imgViewInfo.subresourceRange.baseArrayLayer = 0;
        imgViewInfo.subresourceRange.layerCount = 1;
        imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imgViewInfo.flags = 0;

        sc_buffer.image = scPrivateVars.swapchainImages[i];

        //由于交换链不是我们所有的，我们不能设置图像布局，在设置实现时可能会出现错误，图像布局是由WSI实现创建的，不是我们创建的

        imgViewInfo.image = sc_buffer.image;

        result = vkCreateImageView(rendererObj->getDevice()->device, &imgViewInfo, nullptr, &sc_buffer.view);
        scPublicVars.colorBuffer.push_back(sc_buffer);
        assert(result == VK_SUCCESS);
    }
    scPublicVars.currentColorBuffer = 0;
}

void VulkanSwapChain::destroySwapChain() {
    VulkanDevice *deviceObj = appObj->deviceObj;

    for (uint32_t i = 0; i < scPublicVars.swapchainImageCount; i++) {
        vkDestroyImageView(deviceObj->device, scPublicVars.colorBuffer[i].view, nullptr);
    }
    fpDestroySwapchainKHR(deviceObj->device, scPublicVars.swapChain, nullptr);

    vkDestroySurfaceKHR(appObj->instanceObj.instance, scPublicVars.surface, nullptr);
}
