#pragma once
#include "ObjObject.h"
#include "ShaderQueueSuit_Common.h"
#include <GLFW/glfw3.h>
#include <cstdlib>//标准库标准通用工具函数
#include <functional>//标准库与函数对象相关的模板类和函数
#include <iostream>//标准库输入输出头文件
#include <stdexcept>//标准库异常处理文件
#include <vulkan/vulkan.h>//引用Vulkan的头文件

#define MULSAMPLE_COUNT 4//使用的多重采样数
#define _MULSAMPLE_FACTOR_H(a, b, c) a##b##c//定义连接宏的函数
#define MULSAMPLE_FACTOR_H(a, b, c) _MULSAMPLE_FACTOR_H(a, b, c)
#define MULSAMPLE_FACTOR MULSAMPLE_FACTOR_H(VK_SAMPLE_COUNT_, MULSAMPLE_COUNT, _BIT)//构造VkSampleCountFlagBits 型的值

class VulkanDemoApp {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    static uint32_t screenWidth;// 屏幕宽度
    static uint32_t screenHeight;// 屏幕高度
    // 窗口辅助结构体
    GLFWwindow *window;
    std::vector<const char *> instanceExtensionNames;// 需要使用的实例扩展名称列表

    VkInstance instance;
    uint32_t gpuCount;
    std::vector<VkPhysicalDevice> gpus;
    uint32_t queueFamilyCount;
    std::vector<VkQueueFamilyProperties> queueFamilyProps;
    uint32_t queueGraphicsFamilyIndex;
    VkQueue queueGraphics;
    uint32_t queuePresentFamilyIndex;
    std::vector<const char *> deviceExtensionNames;
    VkDevice device;
    VkCommandPool cmdPool;
    VkCommandBuffer cmdBuffer;
    VkCommandBufferBeginInfo cmd_buf_info;
    VkCommandBuffer cmd_buffers[1];
    VkSubmitInfo submit_info[1];
    VkSurfaceKHR surface;
    std::vector<VkFormat> formats;
    VkSurfaceCapabilitiesKHR surfCapabilities;
    uint32_t presentModeCount;
    std::vector<VkPresentModeKHR> presentModes;
    VkExtent2D swapchainExtent;
    VkSwapchainKHR swapChain;
    uint32_t swapchainImageCount;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    VkSemaphore imageAcquiredSemaphore;
    VkFormat depthFormat;
    VkFormatProperties depthFormatProps;
    VkImage depthImage;
    VkDeviceMemory memDepth;
    VkImageView depthImageView;
    VkImage depthImageMul;
    VkDeviceMemory memDepthMul;
    VkImageView depthImageViewMul;
    VkFormat colorFormat;
    VkFormatProperties colorFormatProps;
    VkImage colorImage;
    VkDeviceMemory memColor;
    VkImageView colorImageView;
    uint32_t currentBuffer;
    VkRenderPass renderPass;
    VkClearValue clear_values[3];
    VkRenderPassBeginInfo rp_begin;
    VkFence taskFinishFence;
    VkPresentInfoKHR present;
    VkFramebuffer *framebuffers;
    ShaderQueueSuit_Common *sqsCL;
    ObjObject *objObject;
    static float xAngle;
    static float yAngle;
    static float zAngle;
    void initVulkanInstance();
    void enumerateVulkanPhysicalDevices();
    void createVulkanDevices();
    void createVulkanCommandBuffer();
    void createVulkanSwapChain();
    void create_vulkan_selfColorBuffer();
    void createVulkanDepthBuffer();
    void createRenderPass();
    void initQueue();
    void createFrameBuffer();
    void createDrawableObject();
    void drawObject();
    void initTextures();
    void initPipeline();
    void createFence();
    void initPresentInfo();
    void initMatrix();
    void flushUniformBuffer();
    void flushTexToDesSet();
    void destroyFence();
    void destroyPipeline();
    void destroyDrawableObject();
    void destroyFrameBuffer();
    void destroyRenderPass();
    void destroyVulkanDepthBuffer();
    void destroy_vulkan_selfColorBuffer();
    void destroyVulkanSwapChain();
    void destroyVulkanCommandBuffer();
    void destroyVulkanDevices();
    void destroyVulkanInstance();

    // 三角形应用运行函数
    void run();

    // TODO:在该需要重建交换链的地方修改他的值并重建交换链
    bool frameBufferResized = false;

private:
    // 初始化窗体
    void initWindow();

    // 初始化Vulkan
    void initVulkan();

    // 图形程序渲染流程主循环
    void mainLoop();

    // 后续清理工作
    void cleanup();

    void destroyWindow() const;

    static void frameBufferResizeCallback(GLFWwindow *window, int width, int height);

    std::vector<const char *> static get_required_extensions();
};
