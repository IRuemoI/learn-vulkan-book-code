#pragma once
#include "ColorObject.h"
#include "ObjObject.h"
#include "ShaderQueueSuit_Light.h"
#include "ShaderQueueSuit_PreProcess.h"
#include <GLFW/glfw3.h>
#include <cstdlib>//标准库标准通用工具函数
#include <functional>//标准库与函数对象相关的模板类和函数
#include <iostream>//标准库输入输出头文件
#include <stdexcept>//标准库异常处理文件
#include <vulkan/vulkan.h>//引用Vulkan的头文件

#define SELF_COLOR_COUNT 3
#define COLOR_ATTACH_COUNT 4
class VulkanDemoApp {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    static uint32_t screenWidth;// 屏幕宽度
    static uint32_t screenHeight;// 屏幕高度

    // 窗口辅助结构体
    GLFWwindow *window;
    std::vector<const char *> instanceExtensionNames;

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
    float ratio;
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
    VkFormat depthFormat;
    VkFormatProperties depthFormatProps;
    VkImage depthImage;
    VkDeviceMemory memDepth;
    VkImageView depthImageView;

    VkImage colorImage[SELF_COLOR_COUNT];
    VkDeviceMemory memColor[SELF_COLOR_COUNT];
    static VkImageView colorImageView[SELF_COLOR_COUNT];
    VkDescriptorImageInfo colorImageInfo[SELF_COLOR_COUNT];
    VkSemaphore imageAcquiredSemaphore;
    uint32_t currentBuffer;
    VkRenderPass renderPass;
    VkClearValue clear_values[5];
    VkRenderPassBeginInfo rp_begin;
    VkFence taskFinishFence;
    VkPresentInfoKHR present;
    VkFramebuffer *framebuffers;
    ShaderQueueSuit_PreProcess *sqsCTL;
    ShaderQueueSuit_Light *sqsL;
    ObjObject *CHForDraw;
    ObjObject *carForDraw;
    ObjObject *treeForDraw;
    ObjObject *floorForDraw;
    static float yAngle;
    static float zAngle;
    void initVulkanInstance();
    void destroyVulkanInstance();
    void enumerateVulkanPhysicalDevices();
    void createVulkanDevices();
    void destroyVulkanDevices();
    void createVulkanCommandBuffer();
    void destroyVulkanCommandBuffer();
    void createVulkanSwapChain();
    void destroyVulkanSwapChain();
    void createVulkanDepthBuffer();
    void destroyVulkanDepthBuffer();
    void create_vulkan_SelfColorBufferSpec(VkFormat colorFormat, int index);
    void create_vulkan_SelfColorBuffer();
    void destroy_vulkan_SelfColorBuffer();
    void createRenderPass();
    void destroyRenderPass();
    void initQueue();
    void createFrameBuffer();
    void destroyFrameBuffer();
    void createDrawableObject();
    void destroyDrawableObject();
    void drawObject();
    void doVulkan();
    void initTextures();
    void destroyTextures();
    void initPipeline();
    void destroyPipeline();
    void createFence();
    void destroyFence();
    void initPresentInfo();
    void initMatrixAndLight();
    void flushUniformBuffer();
    void flushTexToDesSet();

    bool frameBufferResized = false;

    //程序的骨架
    void initWindow();// 初始化窗体
    void initVulkan();// 初始化Vulkan
    void run();
    void mainLoop();// 图形程序渲染流程主循环
    void cleanup();// 后续清理工作
    void destroyWindow() const;
    static void frameBufferResizeCallback(GLFWwindow *window, int width, int height);
    std::vector<const char *> static get_required_extensions();
};
