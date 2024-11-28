#include "VulkanDemoApp.h"
#include "CameraUtil.h"
#include "FPSUtil.h"
#include "HelpFunction.h"
#include "LightManager.h"
#include "LoadUtil.h"
#include "MatrixState2D.h"
#include "MatrixState3D.h"
#include "TextureManager.h"
#include "Util.h"
#include <cassert>

// 程序调用的显卡索引 若只有一个GPU此值只可能是0
#define USED_GPU_INDEX 0

uint32_t VulkanDemoApp::screenWidth;// 屏幕宽度
uint32_t VulkanDemoApp::screenHeight;// 屏幕高度
int VulkanDemoApp::ifSSAO = 0;

#pragma region Framework

void VulkanDemoApp::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void VulkanDemoApp::initWindow() {
    glfwInit();// 初始化glfw

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);// 禁用OpenGL上下文

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan 14.9", nullptr, nullptr);// 创建窗口并返回窗口的句柄
    glfwSetWindowUserPointer(window, this);// 设置窗口用户指针

    glfwSetCursorPosCallback(window, cursor_pos_callback);// 设置光标位置回调函数
    glfwSetMouseButtonCallback(window, mouse_button_callback);// 设置鼠标点击回调函数
    glfwSetScrollCallback(window, scroll_callback);// 设置鼠标滚轮回调函数

    glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);// 设置帧缓冲区大小更改回到函数(窗口大小更改时调用)
}

// 帧缓冲区重设大小回调函数
void VulkanDemoApp::frameBufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<VulkanDemoApp *>(glfwGetWindowUserPointer(window));
    app->frameBufferResized = true;
}

void VulkanDemoApp::initVulkan() {
    initVulkanInstance();
    enumerateVulkanPhysicalDevices();
    createVulkanDevices();
    createVulkanCommandBuffer();
    initQueue();
    createVulkanSwapChain();
    createVulkanDepthBuffer();
    create_vulkan_SelfColorBuffer();
    create_render_pass_screen();
    create_render_pass_self();
    create_frame_buffer_screen();
    create_frame_buffer_self();
    initTextures();
    createDrawableObject();
    initPipeline();
    createFence();
    initPresentInfo();
    initMatrixAndLight();
}

void VulkanDemoApp::mainLoop() {
    FPSUtil::init();// 初始化FPS计算

    // 若glfw所管理的窗口没有点击关闭按钮则一直循环
    while (!glfwWindowShouldClose(window)) {
        processInputs(window);// 处理键鼠输入
        glfwPollEvents();// 处理glfw相关的事件，比如窗口的移动和关闭等
        drawObject();// 绘制帧
    }
    vkDeviceWaitIdle(device);// 等待所有指令提交后设备闲置
}

void VulkanDemoApp::cleanup() {
    destroyPipeline();
    destroyDrawableObject();
    destroyTexture();
    destroyFrameBuffer();
    destroy_render_pass_screen();
    destroy_render_pass_self();
    destroyVulkanDepthBuffer();
    destroy_vulkan_SelfColorBuffer();
    destroyVulkanSwapChain();
    destroyVulkanCommandBuffer();
    destroyVulkanDevices();
    destroyVulkanInstance();

    destroyWindow();
    glfwTerminate();// 终止glfw程序
}

void VulkanDemoApp::destroyWindow() const {
    glfwDestroyWindow(window);// 销毁由glfw创建的窗口对象
}

// 获取所有glfw要求的扩展
std::vector<const char *>
VulkanDemoApp::get_required_extensions() {
    uint32_t glfwExtensionCount = 0;// 定义glfw所需的扩展数量
    const char **glfwExtensions;// glfw所需的扩展名称
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);// 查找glfw所需的扩展

    std::vector<const char *> extensions(
            glfwExtensions,
            glfwExtensions + glfwExtensionCount);// 申请glfwExtensionCount个用于存储扩展名称的向量单位空间
    return extensions;
}

// 创建vulkan实例的方法
void VulkanDemoApp::initVulkanInstance() {
    instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    VkApplicationInfo app_info = {};// 构建应用信息结构体实例
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;// 结构体的类型
    app_info.pNext = nullptr;// 自定义数据的指针
    app_info.pApplicationName = "HelloVulkan";// 应用的名称
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);// 应用的版本号
    app_info.pEngineName = "HelloVulkan";// 应用的引擎名称
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);// 应用的引擎版本号
    app_info.apiVersion = VK_API_VERSION_1_2;// 使用的Vulkan图形应用程序API版本

    instanceExtensionNames = get_required_extensions();// 获取所有glfw要求的扩展

    VkInstanceCreateInfo inst_info = {};// 构建实例创建信息结构体实例
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;// 结构体的类型
    inst_info.pNext = nullptr;// 自定义数据的指针
    inst_info.flags = 0;// 供将来使用的标志
    inst_info.pApplicationInfo = &app_info;// 绑定应用信息结构体
    inst_info.enabledExtensionCount = static_cast<uint32_t>(instanceExtensionNames.size());// 设置启用扩展的数量
    inst_info.ppEnabledExtensionNames = instanceExtensionNames.data();// 设置所有启用的扩展名称

    inst_info.enabledLayerCount = 0;// 启动的层数量
    inst_info.ppEnabledLayerNames = nullptr;// 启动的层名称列表

    VkResult result;// 存储运行结果的辅助变量

    // 创建Vulkan实例
    result = vkCreateInstance(&inst_info, nullptr, &instance);// 创建实例
    if (result == VK_SUCCESS) {
        printf("Vulkan实例创建成功!\n");
    } else {
        printf("Vulkan实例创建失败!\n");
    }
}

// 销毁vulkan实例的方法
void VulkanDemoApp::destroyVulkanInstance() {
    vkDestroyInstance(instance, nullptr);
    printf("Vulkan实例销毁完毕!\n");
}

// 获取硬件设备的方法（GPU）
void VulkanDemoApp::enumerateVulkanPhysicalDevices() {
    gpuCount = 0;// 存储物理设备数量的变量
    VkResult result = vkEnumeratePhysicalDevices(instance, &gpuCount,
                                                 nullptr);// 获取物理设备数量
    assert(result == VK_SUCCESS);
    printf("[Vulkan硬件设备数量为%d个]\n", gpuCount);
    gpus.resize(gpuCount);// 设置物理设备列表尺寸
    result = vkEnumeratePhysicalDevices(instance, &gpuCount,
                                        gpus.data());// 填充物理设备列表
    assert(result == VK_SUCCESS);
    vkGetPhysicalDeviceMemoryProperties(gpus[USED_GPU_INDEX], &memoryProperties);// 获取第一物理设备的内存属性
}

// 创建逻辑设备的方法
void VulkanDemoApp::createVulkanDevices() {
    vkGetPhysicalDeviceQueueFamilyProperties(gpus[USED_GPU_INDEX], &queueFamilyCount,
                                             nullptr);// 获取物理设备0中队列家族的数量
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(gpus[USED_GPU_INDEX],
                                  &device_properties);// 获取物理设备0的属性
    printf("[Vulkan硬件设备\"%s\"支持的队列家族数量为%d]\n", device_properties.deviceName, queueFamilyCount);
    queueFamilyProps.resize(queueFamilyCount);// 随队列家族数量改变vector长度
    vkGetPhysicalDeviceQueueFamilyProperties(gpus[USED_GPU_INDEX], &queueFamilyCount,
                                             queueFamilyProps.data());// 填充物理设备0队列家族属性列表
    printf("[成功获取Vulkan硬件设备支持的队列家族属性列表]\n");

    VkDeviceQueueCreateInfo queueInfo = {};// 构建设备队列创建信息结构体实例
    for (unsigned int i = 0; i < queueFamilyCount; i++) {// 遍历所有队列家族
        if (queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {// 若当前队列家族支持图形工作
            queueInfo.queueFamilyIndex = i;// 绑定此队列家族索引
            queueGraphicsFamilyIndex = i;// 记录支持图形工作的队列家族索引
            printf("[支持GRAPHICS工作的一个队列家族的索引为%d]\n", i);
            printf("[此家族中的实际队列数量是%d]\n", queueFamilyProps[i].queueCount);
            break;
        }
    }

    float queue_priorities[1] = {0.0};// 创建队列优先级数组
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;// 给出结构体类型
    queueInfo.pNext = nullptr;// 自定义数据的指针
    queueInfo.queueCount = 1;// 指定队列数量
    queueInfo.pQueuePriorities = queue_priorities;// 给出每个队列的优先级
    queueInfo.queueFamilyIndex = queueGraphicsFamilyIndex;// 绑定队列家族索引
    deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);// 设置所需扩展

    VkDeviceCreateInfo deviceInfo = {};// 构建逻辑设备创建信息结构体实例
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;// 给出结构体类型
    deviceInfo.pNext = nullptr;// 自定义数据的指针
    deviceInfo.queueCreateInfoCount = 1;// 指定设备队列创建信息结构体数量
    deviceInfo.pQueueCreateInfos = &queueInfo;// 给定设备队列创建信息结构体列表
    deviceInfo.enabledExtensionCount = deviceExtensionNames.size();// 所需扩展数量
    deviceInfo.ppEnabledExtensionNames = deviceExtensionNames.data();// 所需扩展列表
    deviceInfo.enabledLayerCount = 0;// 需启动Layer的数量
    deviceInfo.ppEnabledLayerNames = nullptr;// 需启动Layer的名称列表
    deviceInfo.pEnabledFeatures = nullptr;// 启用的设备特性

    VkResult result = vkCreateDevice(gpus[USED_GPU_INDEX], &deviceInfo, nullptr, &device);// 创建逻辑设备
    assert(result == VK_SUCCESS);// 检查逻辑设备是否创建成功
}

// 销毁逻辑设备的方法
void VulkanDemoApp::destroyVulkanDevices() {
    vkDestroyDevice(device, nullptr);
    printf("逻辑设备销毁完毕！\n");
}

// 创建命令缓冲的方法
void VulkanDemoApp::createVulkanCommandBuffer() {
    VkCommandPoolCreateInfo cmd_pool_info = {};// 构建命令池创建信息结构体实例
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;// 给定结构体类型
    cmd_pool_info.pNext = nullptr;// 自定义数据的指针
    cmd_pool_info.queueFamilyIndex = queueGraphicsFamilyIndex;// 绑定所需队列家族索引
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;// 执行控制标志
    VkResult result = vkCreateCommandPool(device, &cmd_pool_info, nullptr,
                                          &cmdPool);// 创建命令池
    assert(result == VK_SUCCESS);// 检查命令池创建是否成功

    VkCommandBufferAllocateInfo cmdBAI = {};// 构建命令缓冲分配信息结构体实例
    cmdBAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;// 给定结构体类型
    cmdBAI.pNext = nullptr;// 自定义数据的指针
    cmdBAI.commandPool = cmdPool;// 指定命令池
    cmdBAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;// 分配的命令缓冲级别
    cmdBAI.commandBufferCount = 1;// 分配的命令缓冲数量
    result = vkAllocateCommandBuffers(device, &cmdBAI, &cmdBuffer);// 分配命令缓冲

    assert(result == VK_SUCCESS);// 检查分配是否成功
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;// 给定结构体类型
    cmd_buf_info.pNext = nullptr;// 自定义数据的指针
    cmd_buf_info.flags = 0;// 描述使用标志
    cmd_buf_info.pInheritanceInfo = nullptr;// 命令缓冲继承信息
    cmd_buffers[0] = cmdBuffer;// 要提交到队列执行的命令缓冲数组

    auto *pipe_stage_flags = new VkPipelineStageFlags();// 目标管线阶段
    *pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submit_info[0].pNext = nullptr;// 自定义数据的指针
    submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;// 给定结构体类型
    submit_info[0].pWaitDstStageMask = pipe_stage_flags;// 给定目标管线阶段
    submit_info[0].commandBufferCount = 1;// 命令缓冲数量
    submit_info[0].pCommandBuffers = cmd_buffers;// 提交的命令缓冲数组
    submit_info[0].signalSemaphoreCount = 0;// 信号量数量
    submit_info[0].pSignalSemaphores = nullptr;// 信号量数组
}

void VulkanDemoApp::destroyVulkanCommandBuffer() {// 销毁命令缓冲的方法
    // 创建要释放的命令缓冲数组
    VkCommandBuffer cmdBufferArray[1] = {cmdBuffer};
    // 释放命令缓冲
    vkFreeCommandBuffers(device,// 所属逻辑设备
                         cmdPool,// 所属命令池
                         1,// 要销毁的命令缓冲数量
                         cmdBufferArray// 要销毁的命令缓冲数组
    );
    // 销毁命令池
    vkDestroyCommandPool(device, cmdPool, nullptr);
}

// 获取设备中支持图形工作的队列
void VulkanDemoApp::initQueue() {
    // 获取指定家族中索引为0的队列
    vkGetDeviceQueue(device, queueGraphicsFamilyIndex, 0, &queueGraphics);
}

// 创建绘制用swapChain的方法
void VulkanDemoApp::createVulkanSwapChain() {
    // 创建KHR表面
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    assert(result == VK_SUCCESS);

    // 遍历设备对应的队列家族列表
    auto *pSupportsPresent = (VkBool32 *) malloc(queueFamilyCount * sizeof(VkBool32));
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(gpus[USED_GPU_INDEX], i, surface, &pSupportsPresent[i]);
        printf("队列家族索引=%d %s显示\n", i, (pSupportsPresent[i] == 1 ? "支持" : "不支持"));
    }

    queueGraphicsFamilyIndex = UINT32_MAX;// 支持图形工作的队列家族索引
    queuePresentFamilyIndex = UINT32_MAX;// 支持显示(呈现)工作的队列家族索引
    for (uint32_t i = 0; i < queueFamilyCount; ++i)// 遍历设备对应的队列家族列表
    {
        // 如果当前遍历到的队列家族支持Graphics（图形）工作
        if ((queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)// 若此队列家族支持图形工作
        {
            // 记录支持Graphics（图形）工作的队列家族索引
            if (queueGraphicsFamilyIndex == UINT32_MAX)
                queueGraphicsFamilyIndex = i;
            // 如果当前遍历到的队列家族支持Present（显示工作）工作
            if (pSupportsPresent[i] == VK_TRUE)// 如果当前队列家族支持显示工作
            {
                queueGraphicsFamilyIndex = i;// 记录此队列家族索引为支持图形工作的
                queuePresentFamilyIndex = i;// 记录此队列家族索引为支持显示工作的
                printf("队列家族索引=%d同时支持Graphics（图形）和Present（显示）工作\n", i);
                break;
            }
        }
    }

    if (queuePresentFamilyIndex == UINT32_MAX)// 若没有找到同时支持两项工作的队列家族
    {
        for (size_t i = 0; i < queueFamilyCount; ++i)// 遍历设备对应的队列家族列表
        {
            if (pSupportsPresent[i] == VK_TRUE)// 判断是否支持显示工作
            {
                queuePresentFamilyIndex = i;// 记录此队列家族索引为支持显示工作的
                break;
            }
        }
    }
    free(pSupportsPresent);// 释放存储是否支持呈现工作的布尔值列表

    // 没有找到支持Graphics（图形）或Present（显示）工作的队列家族
    // 没有找到支持图形或显示工作的队列家族
    if (queueGraphicsFamilyIndex == UINT32_MAX || queuePresentFamilyIndex == UINT32_MAX) {
        printf("没有找到支持Graphics（图形）或Present（显示）工作的队列家族\n");
        assert(false);// 若没有支持图形或显示操作的队列家族则程序终止
    }

    uint32_t formatCount;// 支持的格式数量
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[USED_GPU_INDEX], surface, &formatCount,
                                                  nullptr);// 获取支持的格式数量
    printf("支持的格式数量为 %d\n", formatCount);
    auto *surfFormats = (VkSurfaceFormatKHR *) malloc(formatCount * sizeof(VkSurfaceFormatKHR));// 分配对应数量的空间
    formats.resize(formatCount);// 调整对应Vector尺寸
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[USED_GPU_INDEX], surface, &formatCount,
                                                  surfFormats);// 获取支持的格式信息
    for (unsigned int i = 0; i < formatCount; i++) {// 记录支持的格式信息
        formats[i] = surfFormats[i].format;
        printf("[%d]支持的格式为%d\n", i, formats[i]);
    }
    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {// 特殊情况处理
        formats[0] = VK_FORMAT_B8G8R8A8_UNORM;
    }
    free(surfFormats);

    // 获取KHR表面的能力
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpus[USED_GPU_INDEX], surface, &surfCapabilities);
    assert(result == VK_SUCCESS);

    // 获取支持的显示模式数量
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[USED_GPU_INDEX], surface, &presentModeCount, nullptr);
    assert(result == VK_SUCCESS);
    printf("显示模式数量为%d\n", presentModeCount);

    // 调整对应Vector尺寸
    presentModes.resize(presentModeCount);
    // 获取支持的显示模式列表
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[USED_GPU_INDEX], surface, &presentModeCount,
                                                       presentModes.data());
    for (unsigned int i = 0; i < presentModeCount; i++) {
        printf("显示模式[%d]编号为%d\n", i, presentModes[i]);
    }

    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;// 确定交换链显示模式
    for (size_t i = 0; i < presentModeCount; i++)// 遍历显示模式列表
    {
        // 如果也支持VK_PRESENT_MODE_MAILBOX_KHR模式，由于其效率高，便选用
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
            // 如果没能用上VK_PRESENT_MODE_MAILBOX_KHR模式，但有VK_PRESENT_MODE_IMMEDIATE_KHR模式
            // 也比VK_PRESENT_MODE_FIFO_KHR模式强，故选用
            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    // 确定surface的宽度、高度
    // 如果surface能力中的尺寸没有定义（宽度为0xFFFFFFFF表示没定义）
    if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {
        swapchainExtent.width = screenWidth;// 设置宽度为窗体宽度
        swapchainExtent.height = screenHeight;// 设置高度为窗体高度
        // 宽度设置值限制到最大值与最小值之间
        if (swapchainExtent.width < surfCapabilities.minImageExtent.width) {
            swapchainExtent.width = surfCapabilities.minImageExtent.width;
        } else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width) {
            swapchainExtent.width = surfCapabilities.maxImageExtent.width;
        }
        // 高度设置值限制到最大值与最小值之间
        if (swapchainExtent.height < surfCapabilities.minImageExtent.height) {
            swapchainExtent.height = surfCapabilities.minImageExtent.height;
        } else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height) {
            swapchainExtent.height = surfCapabilities.maxImageExtent.height;
        }
        printf("使用自己设置的 宽度 %d 高度 %d\n", swapchainExtent.width, swapchainExtent.height);
    } else {
        // 若表面有确定尺寸
        swapchainExtent = surfCapabilities.currentExtent;
        printf("使用获取的surface能力中的 宽度 %d 高度 %d\n", swapchainExtent.width, swapchainExtent.height);
    }

    screenWidth = swapchainExtent.width;// 记录实际采用的宽度
    screenHeight = swapchainExtent.height;// 记录实际采用的高度

    // 期望交换链中的最少图像数量
    uint32_t desiredMinNumberOfSwapChainImages = surfCapabilities.minImageCount + 1;
    // 将数量限制到范围内
    if ((surfCapabilities.maxImageCount > 0) && (desiredMinNumberOfSwapChainImages > surfCapabilities.maxImageCount)) {
        desiredMinNumberOfSwapChainImages = surfCapabilities.maxImageCount;
    }

    // KHR表面变换标志
    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)// 若支持所需的变换
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else// 若不支持变换
    {
        preTransform = surfCapabilities.currentTransform;
    }

    VkSwapchainCreateInfoKHR swapchain_ci = {};// 构建交换链创建信息结构体实例
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;// 结构体类型
    swapchain_ci.pNext = nullptr;// 自定义数据的指针
    swapchain_ci.surface = surface;// 指定KHR表面
    swapchain_ci.minImageCount = desiredMinNumberOfSwapChainImages;// 最少图像数量
    swapchain_ci.imageFormat = formats[0];// 图像格式
    swapchain_ci.imageExtent.width = swapchainExtent.width;// 交换链图像宽度
    swapchain_ci.imageExtent.height = swapchainExtent.height;// 交换链图像高度
    swapchain_ci.preTransform = preTransform;// 指定变换标志
    swapchain_ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;// 混合Alpha值
    swapchain_ci.imageArrayLayers = 1;// 图像数组层数
    swapchain_ci.presentMode = swapchainPresentMode;// 交换链的显示模式
    swapchain_ci.oldSwapchain = VK_NULL_HANDLE;// 前导交换链
    swapchain_ci.clipped = true;// 开启剪裁
    swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;// 色彩空间
    swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;// 图像用途
    swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;// 图像共享模式
    swapchain_ci.queueFamilyIndexCount = 0;// 队列家族数量
    swapchain_ci.pQueueFamilyIndices = nullptr;// 队列家族索引列表

    if (queueGraphicsFamilyIndex != queuePresentFamilyIndex)// 若支持图形和显示工作的队列家族不相同
    {
        swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_ci.queueFamilyIndexCount = 2;// 交换链所需的队列家族索引数量为2
        uint32_t queueFamilyIndices[2] = {queueGraphicsFamilyIndex, queuePresentFamilyIndex};
        swapchain_ci.pQueueFamilyIndices = queueFamilyIndices;// 交换链所需的队列家族索引列表
    }

    result = vkCreateSwapchainKHR(device, &swapchain_ci, nullptr,
                                  &swapChain);// 创建交换链
    assert(result == VK_SUCCESS);// 检查交换链是否创建成功

    // 获取交换链中的图像数量
    result = vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount, nullptr);
    assert(result == VK_SUCCESS);
    printf("[SwapChain中的Image数量为%d]\n", swapchainImageCount);// 检查是否获取成功
    swapchainImages.resize(swapchainImageCount);// 调整图像列表尺寸
    // 获取交换链中的图像列表
    result = vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount, swapchainImages.data());
    assert(result == VK_SUCCESS);
    swapchainImageViews.resize(swapchainImageCount);// 调整图像视图列表尺寸
    for (uint32_t i = 0; i < swapchainImageCount; i++)// 为交换链中的各幅图像创建图像视图
    {
        VkImageViewCreateInfo color_image_view = {};// 构建图像视图创建信息结构体实例
        color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;// 设置结构体类型
        color_image_view.pNext = nullptr;// 自定义数据的指针
        color_image_view.flags = 0;// 供将来使用的标志
        color_image_view.image = swapchainImages[i];// 对应交换链图像
        color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;// 图像视图的类型
        color_image_view.format = formats[0];// 图像视图格式
        color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;// 设置R通道调和
        color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;// 设置G通道调和
        color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;// 设置B通道调和
        color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;// 设置A通道调和
        color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;// 图像视图使用方面
        color_image_view.subresourceRange.baseMipLevel = 0;// 基础Mipmap级别
        color_image_view.subresourceRange.levelCount = 1;// Mipmap级别的数量
        color_image_view.subresourceRange.baseArrayLayer = 0;// 基础数组层
        color_image_view.subresourceRange.layerCount = 1;// 数组层的数量
        result = vkCreateImageView(device, &color_image_view, nullptr,
                                   &swapchainImageViews[i]);// 创建图像视图
        assert(result == VK_SUCCESS);// 检查是否创建成功
    }
}

void VulkanDemoApp::destroyVulkanSwapChain() {// 销毁交换链相关的方法
    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        vkDestroyImageView(device, swapchainImageViews[i], nullptr);
        printf("[销毁SwapChain ImageView %d 成功]\n", i);
    }
    vkDestroySwapchainKHR(device, swapChain, nullptr);
    printf("销毁SwapChain成功!\n");
}

#pragma endregion


void VulkanDemoApp::createVulkanDepthBuffer() {
    depthFormat = VK_FORMAT_D16_UNORM;
    VkImageCreateInfo image_info = {};
    vkGetPhysicalDeviceFormatProperties(gpus[USED_GPU_INDEX], depthFormat, &depthFormatProps);
    if (depthFormatProps.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        image_info.tiling = VK_IMAGE_TILING_LINEAR;
        printf("tiling为VK_IMAGE_TILING_LINEAR！\n");
    } else if (depthFormatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        printf("tiling为VK_IMAGE_TILING_OPTIMAL！\n");
    } else {
        printf("不支持VK_FORMAT_D16_UNORM！\n");
    }
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.pNext = nullptr;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = depthFormat;
    image_info.extent.width = screenWidth;
    image_info.extent.height = screenHeight;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = nullptr;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.flags = 0;
    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = nullptr;
    mem_alloc.allocationSize = 0;
    mem_alloc.memoryTypeIndex = 0;
    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.pNext = nullptr;
    view_info.image = VK_NULL_HANDLE;
    view_info.format = depthFormat;
    view_info.components.r = VK_COMPONENT_SWIZZLE_R;
    view_info.components.g = VK_COMPONENT_SWIZZLE_G;
    view_info.components.b = VK_COMPONENT_SWIZZLE_B;
    view_info.components.a = VK_COMPONENT_SWIZZLE_A;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.flags = 0;
    VkResult result = vkCreateImage(device, &image_info, nullptr, &depthImage);
    assert(result == VK_SUCCESS);
    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(device, depthImage, &mem_reqs);
    mem_alloc.allocationSize = mem_reqs.size;
    VkFlags requirements_mask = 0;
    bool flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &mem_alloc.memoryTypeIndex);
    assert(flag);
    printf("确定内存类型成功 类型索引为%d\n", mem_alloc.memoryTypeIndex);
    result = vkAllocateMemory(device, &mem_alloc, nullptr, &memDepth);
    assert(result == VK_SUCCESS);
    result = vkBindImageMemory(device, depthImage, memDepth, 0);
    assert(result == VK_SUCCESS);
    view_info.image = depthImage;
    result = vkCreateImageView(device, &view_info, nullptr, &depthImageView);
    assert(result == VK_SUCCESS);
}
void VulkanDemoApp::destroyVulkanDepthBuffer() {
    vkDestroyImageView(device, depthImageView, nullptr);
    vkDestroyImage(device, depthImage, nullptr);
    vkFreeMemory(device, memDepth, nullptr);
    printf("销毁深度缓冲相关成功!\n");
}
void VulkanDemoApp::create_render_pass_screen() {
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = nullptr;
    imageAcquiredSemaphoreCreateInfo.flags = 0;
    VkResult result = vkCreateSemaphore(device, &imageAcquiredSemaphoreCreateInfo, nullptr, &imageAcquiredSemaphore);
    assert(result == VK_SUCCESS);
    VkAttachmentDescription attachments[2];
    attachments[0].format = formats[0];
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[0].flags = 0;
    attachments[1].format = depthFormat;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[1].flags = 0;
    VkAttachmentReference color_reference = {};
    color_reference.attachment = 0;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkAttachmentReference depth_reference = {};
    depth_reference.attachment = 1;
    depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_reference;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = &depth_reference;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;
    VkRenderPassCreateInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.pNext = nullptr;
    rp_info.attachmentCount = 2;
    rp_info.pAttachments = attachments;
    rp_info.subpassCount = 1;
    rp_info.pSubpasses = &subpass;
    rp_info.dependencyCount = 0;
    rp_info.pDependencies = nullptr;
    result = vkCreateRenderPass(device, &rp_info, nullptr, &renderPassScreen);
    assert(result == VK_SUCCESS);
    clear_values_screen[0].color.float32[0] = 0.0f;
    clear_values_screen[0].color.float32[1] = 0.0f;
    clear_values_screen[0].color.float32[2] = 0.0f;
    clear_values_screen[0].color.float32[3] = 0.0f;
    clear_values_screen[1].depthStencil.depth = 1.0f;
    clear_values_screen[1].depthStencil.stencil = 0;
    rp_begin_screen.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin_screen.pNext = nullptr;
    rp_begin_screen.renderPass = renderPassScreen;
    rp_begin_screen.renderArea.offset.x = 0;
    rp_begin_screen.renderArea.offset.y = 0;
    rp_begin_screen.renderArea.extent.width = screenWidth;
    rp_begin_screen.renderArea.extent.height = screenHeight;
    rp_begin_screen.clearValueCount = 2;
    rp_begin_screen.pClearValues = clear_values_screen;
}
void VulkanDemoApp::destroy_render_pass_screen() {
    vkDestroyRenderPass(device, renderPassScreen, nullptr);
    vkDestroySemaphore(device, imageAcquiredSemaphore, nullptr);
}
void VulkanDemoApp::create_render_pass_self() {
    VkFormat colorFormat[OUT_TEX_COUNT] = {VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT};
    VkAttachmentDescription attachments[5];
    for (int i = 0; i < OUT_TEX_COUNT; i++) {
        attachments[i].format = colorFormat[i];
        attachments[i].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachments[i].flags = 0;
    }
    attachments[4].format = depthFormat;
    attachments[4].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[4].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[4].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[4].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[4].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[4].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[4].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[4].flags = 0;
    VkAttachmentReference color_reference[OUT_TEX_COUNT];
    for (int i = 0; i < OUT_TEX_COUNT; i++) {
        color_reference[i].attachment = i;
        color_reference[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    VkAttachmentReference depth_reference = {};
    depth_reference.attachment = 4;
    depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = OUT_TEX_COUNT;
    subpass.pColorAttachments = color_reference;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = &depth_reference;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;
    VkRenderPassCreateInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.pNext = nullptr;
    rp_info.attachmentCount = 5;
    rp_info.pAttachments = attachments;
    rp_info.subpassCount = 1;
    rp_info.pSubpasses = &subpass;
    rp_info.dependencyCount = 0;
    rp_info.pDependencies = nullptr;
    VkResult result = vkCreateRenderPass(device, &rp_info, nullptr, &renderPassSelf);
    assert(result == VK_SUCCESS);
    for (int i = 0; i < OUT_TEX_COUNT; i++) {
        clear_values_self[i].color.float32[0] = 0.0f;
        clear_values_self[i].color.float32[1] = 0.0f;
        clear_values_self[i].color.float32[2] = 0.0f;
        clear_values_self[i].color.float32[3] = 0.0f;
    }
    clear_values_self[4].depthStencil.depth = 1.0f;
    clear_values_self[4].depthStencil.stencil = 0;
    rp_begin_self.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin_self.pNext = nullptr;
    rp_begin_self.renderPass = renderPassSelf;
    rp_begin_self.renderArea.offset.x = 0;
    rp_begin_self.renderArea.offset.y = 0;
    rp_begin_self.renderArea.extent.width = screenWidth;
    rp_begin_self.renderArea.extent.height = screenHeight;
    rp_begin_self.clearValueCount = 5;
    rp_begin_self.pClearValues = clear_values_self;
}
void VulkanDemoApp::destroy_render_pass_self() {
    vkDestroyRenderPass(device, renderPassSelf, nullptr);
}

void VulkanDemoApp::create_frame_buffer_screen() {
    VkImageView attachments[2];
    attachments[1] = depthImageView;
    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;
    fb_info.renderPass = renderPassScreen;
    fb_info.attachmentCount = 2;
    fb_info.pAttachments = attachments;
    fb_info.width = screenWidth;
    fb_info.height = screenHeight;
    fb_info.layers = 1;
    uint32_t i;
    framebuffers = (VkFramebuffer *) malloc(swapchainImageCount * sizeof(VkFramebuffer));
    assert(framebuffers);
    for (i = 0; i < swapchainImageCount; i++) {
        attachments[0] = swapchainImageViews[i];
        VkResult result = vkCreateFramebuffer(device, &fb_info, nullptr, &framebuffers[i]);
        assert(result == VK_SUCCESS);
        printf("[创建帧缓冲%d成功！]\n", i);
    }
}
void VulkanDemoApp::create_frame_buffer_self() {
    VkImageView attachments[5];
    attachments[4] = depthImageView;
    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;
    fb_info.renderPass = renderPassSelf;
    fb_info.attachmentCount = 5;
    fb_info.pAttachments = attachments;
    fb_info.width = screenWidth;
    fb_info.height = screenHeight;
    fb_info.layers = 1;
    attachments[0] = colorImageView[0];
    attachments[1] = colorImageView[1];
    attachments[2] = colorImageView[2];
    attachments[3] = colorImageView[3];
    VkResult result = vkCreateFramebuffer(device, &fb_info, nullptr, &selfTexFramebuffer);
    assert(result == VK_SUCCESS);
}
void VulkanDemoApp::destroyFrameBuffer() {
    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        vkDestroyFramebuffer(device, framebuffers[i], nullptr);
    }
    free(framebuffers);
    vkDestroyFramebuffer(device, selfTexFramebuffer, nullptr);
    printf("销毁帧缓冲成功！\n");
}
void VulkanDemoApp::createDrawableObject() {
    const char *modelPathPrefix = "../models/1408_";
    carBObjForDraw = LoadUtil::loadFromFile(modelPathPrefix + (string) ("carBody.obj"), device, memoryProperties);
    carWObjForDraw = LoadUtil::loadFromFile(modelPathPrefix + (string) ("carWheel.obj"), device, memoryProperties);
    ptObjForDraw = LoadUtil::loadFromFile(modelPathPrefix + (string) ("pingtai.obj"), device, memoryProperties);
    skyboxObjForDraw = LoadUtil::loadFromFile(modelPathPrefix + (string) ("skybox.obj"), device, memoryProperties);
    dxObjForDraw = LoadUtil::loadFromFile(modelPathPrefix + (string) ("guanyin10.obj"), device, memoryProperties);
    float ratio = (float) screenWidth / (float) screenHeight;
    float *vdataIn = new float[30]{
            -ratio, 1.0f, 0, 0, 0, ratio, -1.0f, 0, 1, 1, ratio, 1.0f, 0, 1, 0,
            -ratio, 1.0f, 0, 0, 0, -ratio, -1.0f, 0, 0, 1, ratio, -1.0f, 0, 1, 1};
    d2dA = new Dashboard2DObject(vdataIn, 30 * 4, 6, device, memoryProperties);
}
void VulkanDemoApp::destroyDrawableObject() {
    delete carBObjForDraw;
    delete carWObjForDraw;
    delete ptObjForDraw;
    delete skyboxObjForDraw;
    delete dxObjForDraw;
    delete d2dA;
}
void VulkanDemoApp::createFence() {
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;
    vkCreateFence(device, &fenceInfo, nullptr, &taskFinishFence);
}
void VulkanDemoApp::destroyFence() {
    vkDestroyFence(device, taskFinishFence, nullptr);
}
void VulkanDemoApp::initPresentInfo() {
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = nullptr;
    present.swapchainCount = 1;
    present.pSwapchains = &swapChain;
    present.waitSemaphoreCount = 0;
    present.pWaitSemaphores = nullptr;
    present.pResults = nullptr;
}
void VulkanDemoApp::initMatrixAndLight() {
    CameraUtil::calCamera(0, 0);
    MatrixState3D::setInitStack();
    float ratio = (float) screenWidth / (float) screenHeight;
    MatrixState3D::setProjectFrustum(-ratio, ratio, -1, 1, 1.5f, 1000);
    MatrixState2D::setInitStack();
    MatrixState2D::setProjectOrtho(-ratio, ratio, -1, 1, 1.5f, 1000);
    MatrixState2D::setCamera(0, 0, 10, 0, 0, 0, 0, 1, 0);
    for (int i = 0; i < LIGHTS_COUNT; i++) {
        LightManager::setLightRange(i, (float) (350 + 80 * i));
        LightManager::setLightPosition(i, 0, 100, 100);
        LightManager::setLightAmbient(i, 0.15f, 0.15f, 0.15f, 0.15f);
        LightManager::setLightDiffuse(i, 0.4f, 0.4f, 0.4f, 0.4f);
        LightManager::setLightSpecular(i, 0.4f, 0.4f, 0.4f, 0.4f);
    }
}
void VulkanDemoApp::flushUniformBufferForToTex() {
    float fragmentUniformData[4] =
            {
                    CameraUtil::camera9Para[0], CameraUtil::camera9Para[1], CameraUtil::camera9Para[2], 1.0};
    uint8_t *pData;
    VkResult result = vkMapMemory(device, sqsCTL->memUniformBuf, 0, sqsCTL->bufferByteCount, 0, (void **) &pData);
    assert(result == VK_SUCCESS);
    memcpy(pData, fragmentUniformData, sqsCTL->bufferByteCount);
    vkUnmapMemory(device, sqsCTL->memUniformBuf);
    flushUniformBufferForToScreen();
}
void VulkanDemoApp::flushTexToDesSetForToTex() {
    for (int i = 0; i < TextureManager::texNames.size(); i++) {
        sqsCTL->writes[0].dstSet = sqsCTL->descSet[i];
        sqsCTL->writes[0].pImageInfo = &(TextureManager::texImageInfoList[TextureManager::texNames[i]]);
        sqsCTL->writes[1].dstSet = sqsCTL->descSet[i];
        vkUpdateDescriptorSets(device, 2, sqsCTL->writes, 0, nullptr);
    }
}
void VulkanDemoApp::flushUniformBufferForToScreen() {
    float fragmentUniformData[85] =
            {
                    CameraUtil::camera9Para[0], CameraUtil::camera9Para[1], CameraUtil::camera9Para[2], 1.0};
    memcpy(&fragmentUniformData[4], LightManager::lightArray, sizeof(float) * 64);
    memcpy(&fragmentUniformData[68], MatrixState3D::getZHMatrix(), sizeof(float) * 16);
    fragmentUniformData[84] = (float) ifSSAO;
    uint8_t *pData;
    VkResult result = vkMapMemory(device, sqsD2D->memUniformBuf, 0, sqsD2D->bufferByteCount, 0, (void **) &pData);
    assert(result == VK_SUCCESS);
    memcpy(pData, fragmentUniformData, sqsD2D->bufferByteCount);
    vkUnmapMemory(device, sqsD2D->memUniformBuf);
}
void VulkanDemoApp::flushTexToDesSetForToScreen() {
    sqsD2D->writes[0].dstSet = sqsD2D->descSet[0];
    sqsD2D->writes[0].pImageInfo = &(colorImageInfo[0]);
    sqsD2D->writes[1].dstSet = sqsD2D->descSet[0];
    sqsD2D->writes[1].pImageInfo = &(colorImageInfo[1]);
    sqsD2D->writes[2].dstSet = sqsD2D->descSet[0];
    sqsD2D->writes[2].pImageInfo = &(colorImageInfo[2]);
    sqsD2D->writes[3].dstSet = sqsD2D->descSet[0];
    sqsD2D->writes[3].pImageInfo = &(colorImageInfo[3]);
    sqsD2D->writes[4].dstSet = sqsD2D->descSet[0];
    vkUpdateDescriptorSets(device, 5, sqsD2D->writes, 0, nullptr);
}
void VulkanDemoApp::drawSceneToTex() {
    const char *tp = "../textures/1408_";
    rp_begin_self.framebuffer = selfTexFramebuffer;
    vkResetCommandBuffer(cmdBuffer, 0);
    VkResult result = vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
    LightManager::move();
    flushUniformBufferForToTex();
    flushTexToDesSetForToTex();
    vkCmdBeginRenderPass(cmdBuffer, &rp_begin_self, VK_SUBPASS_CONTENTS_INLINE);
    MatrixState3D::pushMatrix();
    MatrixState3D::translate(10, 0, 0);
    MatrixState3D::pushMatrix();
    MatrixState3D::scale(50.0f, 50.0f, 50.0f);
    skyboxObjForDraw->drawSelf(cmdBuffer, sqsCTL->pipelineLayout, sqsCTL->pipeline,
                               &(sqsCTL->descSet[TextureManager::getVkDescriptorSetIndex(tp + (string) ("dx1.bntex"))]));
    MatrixState3D::popMatrix();
    MatrixState3D::pushMatrix();
    MatrixState3D::scale(0.05f, 0.05f, 0.05f);
    ptObjForDraw->drawSelf(cmdBuffer, sqsCTL->pipelineLayout, sqsCTL->pipeline, &(sqsCTL->descSet[TextureManager::getVkDescriptorSetIndex(tp + (string) ("pt.bntex"))]));
    MatrixState3D::popMatrix();
    MatrixState3D::pushMatrix();
    MatrixState3D::translate(-150.0f, 90, 0.0f);
    MatrixState3D::scale(0.01f, 0.01f, 0.01f);
    carBObjForDraw->drawSelf(cmdBuffer, sqsCTL->pipelineLayout, sqsCTL->pipeline, &(sqsCTL->descSet[TextureManager::getVkDescriptorSetIndex(tp + (string) ("carBody.bntex"))]));
    carWObjForDraw->drawSelf(cmdBuffer, sqsCTL->pipelineLayout, sqsCTL->pipeline, &(sqsCTL->descSet[TextureManager::getVkDescriptorSetIndex(tp + (string) ("carWheel.bntex"))]));
    MatrixState3D::popMatrix();
    MatrixState3D::pushMatrix();
    MatrixState3D::scale(4.0f, 4.0f, 4.0f);
    MatrixState3D::translate(10, 8, 0);
    dxObjForDraw->drawSelf(cmdBuffer, sqsCTL->pipelineLayout, sqsCTL->pipeline, &(sqsCTL->descSet[TextureManager::getVkDescriptorSetIndex(tp + (string) ("sn.bntex"))]));
    MatrixState3D::popMatrix();
    MatrixState3D::popMatrix();
    vkCmdEndRenderPass(cmdBuffer);
    result = vkEndCommandBuffer(cmdBuffer);
    submit_info[0].waitSemaphoreCount = 0;
    submit_info[0].pWaitSemaphores = nullptr;
    result = vkQueueSubmit(queueGraphics, 1, submit_info, taskFinishFence);
    do {
        result = vkWaitForFences(device, 1, &taskFinishFence, VK_TRUE, FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);
    vkResetFences(device, 1, &taskFinishFence);
}
void VulkanDemoApp::drawSceneToScreen() {
    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &currentBuffer);
    rp_begin_screen.framebuffer = framebuffers[currentBuffer];
    vkResetCommandBuffer(cmdBuffer, 0);
    result = vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
    flushTexToDesSetForToScreen();
    vkCmdBeginRenderPass(cmdBuffer, &rp_begin_screen, VK_SUBPASS_CONTENTS_INLINE);
    d2dA->drawSelf(cmdBuffer, sqsD2D->pipelineLayout, sqsD2D->pipeline, &(sqsD2D->descSet[0]));
    vkCmdEndRenderPass(cmdBuffer);
    result = vkEndCommandBuffer(cmdBuffer);
    submit_info[0].waitSemaphoreCount = 1;
    submit_info[0].pWaitSemaphores = &imageAcquiredSemaphore;
    result = vkQueueSubmit(queueGraphics, 1, submit_info, taskFinishFence);
    do {
        result = vkWaitForFences(device, 1, &taskFinishFence, VK_TRUE, FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);
    vkResetFences(device, 1, &taskFinishFence);
    present.pImageIndices = &currentBuffer;
    result = vkQueuePresentKHR(queueGraphics, &present);
}
void VulkanDemoApp::drawObject() {
    FPSUtil::calFPS();
    FPSUtil::before();
    CameraUtil::flushCameraToMatrix();
    drawSceneToTex();
    drawSceneToScreen();
    FPSUtil::after(60);
}
void VulkanDemoApp::initTextures() {
    TextureManager::initTextures(device, gpus[USED_GPU_INDEX], memoryProperties, cmdBuffer, queueGraphics);
    for (int i = 0; i < OUT_TEX_COUNT; i++) {
        colorImageInfo[i].imageView = colorImageView[i];
        colorImageInfo[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        colorImageInfo[i].sampler = TextureManager::samplerList[0];
    }
}
void VulkanDemoApp::destroyTexture() {
    TextureManager::destroyTextures(device);
}
void VulkanDemoApp::initPipeline() {
    sqsCTL = new ShaderQueueSuit_CommonTexLight(&device, renderPassSelf, memoryProperties);
    sqsD2D = new ShaderQueueSuit_Dashboard2D(&device, renderPassScreen, memoryProperties);
}
void VulkanDemoApp::destroyPipeline() {
    delete sqsCTL;
    delete sqsD2D;
}
/*===========================================*/
void VulkanDemoApp::create_vulkan_SelfColorBufferSpec(VkFormat colorFormat, int index) {
    VkFormatProperties colorFormatProps;
    VkImageCreateInfo image_info = {};
    vkGetPhysicalDeviceFormatProperties(gpus[USED_GPU_INDEX], colorFormat, &colorFormatProps);
    if (colorFormatProps.linearTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) {
        image_info.tiling = VK_IMAGE_TILING_LINEAR;
        printf("tiling为VK_IMAGE_TILING_LINEAR！\n");
    } else if (colorFormatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) {
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        printf("tiling为VK_IMAGE_TILING_OPTIMAL！\n");
    } else {
        printf("不支持%d！\n", VK_FORMAT_R8G8B8A8_UNORM);
    }
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.pNext = nullptr;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = colorFormat;
    image_info.extent.width = screenWidth;
    image_info.extent.height = screenHeight;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = nullptr;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.flags = 0;
    VkResult result = vkCreateImage(device, &image_info, nullptr, &(colorImage[index]));
    assert(result == VK_SUCCESS);
    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = nullptr;
    mem_alloc.allocationSize = 0;
    mem_alloc.memoryTypeIndex = 0;
    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(device, colorImage[index], &mem_reqs);
    mem_alloc.allocationSize = mem_reqs.size;
    VkFlags requirements_mask = 0;
    bool flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &mem_alloc.memoryTypeIndex);
    assert(flag);
    printf("确定内存类型成功 类型索引为%d\n", mem_alloc.memoryTypeIndex);
    result = vkAllocateMemory(device, &mem_alloc, nullptr, &(memColor[index]));
    assert(result == VK_SUCCESS);
    result = vkBindImageMemory(device, colorImage[index], memColor[index], 0);
    assert(result == VK_SUCCESS);
    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.pNext = nullptr;
    view_info.image = VK_NULL_HANDLE;
    view_info.format = colorFormat;
    view_info.components.r = VK_COMPONENT_SWIZZLE_R;
    view_info.components.g = VK_COMPONENT_SWIZZLE_G;
    view_info.components.b = VK_COMPONENT_SWIZZLE_B;
    view_info.components.a = VK_COMPONENT_SWIZZLE_A;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.flags = 0;
    view_info.image = colorImage[index];
    result = vkCreateImageView(device, &view_info, nullptr, &(colorImageView[index]));
    assert(result == VK_SUCCESS);
}
void VulkanDemoApp::create_vulkan_SelfColorBuffer() {
    VkFormat colorFormat[5] = {VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT};
    for (int i = 0; i < OUT_TEX_COUNT; i++) {
        create_vulkan_SelfColorBufferSpec(colorFormat[i], i);
    }
}
void VulkanDemoApp::destroy_vulkan_SelfColorBuffer() {
    for (int i = 0; i < OUT_TEX_COUNT; i++) {
        vkDestroyImageView(device, colorImageView[i], nullptr);
        vkDestroyImage(device, colorImage[i], nullptr);
        vkFreeMemory(device, memColor[i], nullptr);
    }
}