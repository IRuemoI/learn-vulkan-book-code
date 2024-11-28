#pragma once
#include "Dashboard2DObject.h"
#include "ObjObject.h"
#include "ShaderQueueSuit_CommonTexLight.h"
#include "ShaderQueueSuit_Dashboard2D.h"
#include <GLFW/glfw3.h>
#include <cstdlib>//标准库标准通用工具函数
#include <functional>//标准库与函数对象相关的模板类和函数
#include <iostream>//标准库输入输出头文件
#include <mutex>
#include <stdexcept>//标准库异常处理文件
#include <thread>
#include <vulkan/vulkan.h>//引用Vulkan的头文件

#define OUT_TEX_COUNT 2

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
    static float ratio;
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
    VkFormat depthFormat;
    VkFormatProperties depthFormatProps;
    VkImage depthImage;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    VkDeviceMemory memDepth;
    VkImageView depthImageView;
    VkFormat colorFormat;
    VkFormatProperties colorFormatProps;
    VkImage colorImage[2];
    VkDeviceMemory memColor[2];
    VkImageView colorImageView[2];
    VkDescriptorImageInfo colorImageInfo[2];
    VkSemaphore imageAcquiredSemaphore;
    uint32_t currentBuffer;
    VkRenderPass renderPassScreen;
    VkRenderPass renderPassSelf;
    VkClearValue clear_values_screen[2];
    VkClearValue clear_values_self[3];
    VkRenderPassBeginInfo rp_begin_screen;
    VkRenderPassBeginInfo rp_begin_self;
    VkFence taskFinishFence;
    VkPresentInfoKHR present;
    VkFramebuffer *framebuffers;
    VkFramebuffer selfTexFramebuffer;
    ShaderQueueSuit_CommonTexLight *sqsCTL;
    ShaderQueueSuit_Dashboard2D *sqsD2D;
    ObjObject *airship;
    ObjObject *base;
    Dashboard2DObject *d2dA;
    static float isBloom;
    static float rotateIndex;
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
    void create_render_pass_screen();
    void destroy_render_pass_screen();
    void create_render_pass_self();
    void destroy_render_pass_self();
    void initQueue();
    void create_frame_buffer_screen();
    void create_frame_buffer_self();
    void destroyFrameBuffer();
    void createDrawableObject();
    void destroyDrawableObject();
    void drawSceneToTex();
    void drawSceneToScreen();
    void drawObject();
    void initTextures();
    void destroyTextures();
    void initPipeline();
    void destroyPipeline();
    void createFence();
    void destroyFence();
    void initPresentInfo();
    void initMatrixAndLight();
    void flushUniformBufferForToTex();
    void flushTexToDesSetForToTex();
    void flushUniformBufferForToScreen();
    void flushTexToDesSetForToScreen();

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
