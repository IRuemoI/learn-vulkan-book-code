#pragma once
#include "ColorObject.h"
#include "Dashboard2DObject.h"
#include "ObjObject.h"
#include "ShaderQueueSuit_CommonTexLight.h"
#include "ShaderQueueSuit_Dashboard2D.h"
#include "ShaderQueueSuit_ForCalAoFactor.h"
#include <GLFW/glfw3.h>
#include <cstdlib>//标准库标准通用工具函数
#include <functional>//标准库与函数对象相关的模板类和函数
#include <iostream>//标准库输入输出头文件
#include <stdexcept>//标准库异常处理文件
#include <vulkan/vulkan.h>//引用Vulkan的头文件

#define OUT_TEX_COUNT 4//第一轮绘制时的输出数量
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
    VkSemaphore imageAcquiredSemaphore;
    uint32_t currentBuffer;
    VkRenderPass renderPassScreen;
    VkClearValue clear_values_screen[2];
    VkRenderPassBeginInfo rp_begin_screen;
    VkFence taskFinishFence;
    VkPresentInfoKHR present;
    VkFramebuffer *framebuffers;
    ShaderQueueSuit_CommonTexLight *sqsCTL;
    ShaderQueueSuit_Dashboard2D *sqsD2D;
    ShaderQueueSuit_ForCalAoFactor *sqsFCAF;
    ObjObject *ptObjForDraw;
    ObjObject *carBObjForDraw;
    ObjObject *carWObjForDraw;
    ObjObject *skyboxObjForDraw;
    ObjObject *dxObjForDraw;
    Dashboard2DObject *d2dA;
    void initVulkanInstance();
    void enumerateVulkanPhysicalDevices();
    void createVulkanDevices();
    void createVulkanCommandBuffer();
    void createVulkanSwapChain();
    void createVulkanDepthBuffer();
    void create_render_pass_screen();
    void initQueue();
    void create_frame_buffer_screen();
    void createDrawableObject();
    void flushUniformBufferForToTex();
    void flushTexToDesSetForToTex();
    void flushUniformBufferForToScreen();
    void flushTexToDesSetForToScreen();
    void flushUniformBufferForCalAoFactor();
    void flushTexToDesSetForCalAoFactor();
    void drawSceneToTex();
    void drawForCalAoFactor();
    void drawSceneToScreen();
    void drawObject();
    void initTextures();
    void initPipeline();
    void createFence();
    void initPresentInfo();
    void initMatrixAndLight();
    void destroyFence();
    void destroyPipeline();
    void destroyDrawableObject();
    void destroyTexture();
    void destroyFrameBuffer();
    void destroy_render_pass_screen();
    void destroyVulkanDepthBuffer();
    void destroyVulkanSwapChain();
    void destroyVulkanCommandBuffer();
    void destroyVulkanDevices();
    void destroyVulkanInstance();
    VkImage colorImage[4];
    VkDeviceMemory memColor[4];
    VkImageView colorImageView[4];
    VkDescriptorImageInfo colorImageInfo[4];
    VkRenderPass renderPassSelf;
    VkClearValue clear_values_self[5];
    VkRenderPassBeginInfo rp_begin_self;
    VkFramebuffer selfTexFramebuffer;
    void create_vulkan_SelfColorBufferSpec(VkFormat colorFormat, int index);
    void create_vulkan_SelfColorBuffer();
    void destroy_vulkan_SelfColorBuffer();
    void create_render_pass_self();
    void destroy_render_pass_self();
    void create_frame_buffer_self();
    static int ifSSAO;
    VkImage aoFactorImage;
    VkDeviceMemory mem_aoFactor;
    VkImageView aoFactorImageView;
    VkDescriptorImageInfo aoFactorImageInfo;
    VkRenderPass renderPass_aoFactor;
    VkClearValue clear_values_aoFactor[2];
    VkRenderPassBeginInfo rp_begin_aoFactor;
    VkFramebuffer aoFactorTexFramebuffer;
    void create_vulkan_aoFactorBuffer();
    void destroy_vulkan_aoFactorBuffer();
    void create_render_pass_aoFactor();
    void destroy_render_pass_aoFactor();

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
