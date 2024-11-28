#pragma once
#include "ObjObject.h"
#include "PlanePosition.h"
#include "SCBArangeThreadTask.h"
#include "ShaderQueueSuit_Common.h"
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <cstdlib>//标准库标准通用工具函数
#include <functional>//标准库与函数对象相关的模板类和函数
#include <iostream>//标准库输入输出头文件
#include <mutex>
#include <stdexcept>//标准库异常处理文件
#include <thread>
#include <vulkan/vulkan.h>//引用Vulkan的头文件

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
    VkCommandBufferBeginInfo secondary_begin;
    VkCommandBufferInheritanceInfo cmd_buf_inheritance_info;
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
    VkRenderPass renderPass;
    VkClearValue clear_values[2];
    VkRenderPassBeginInfo rp_begin;
    VkFence taskFinishFence;
    VkPresentInfoKHR present;
    VkFramebuffer *framebuffers;
    ShaderQueueSuit_Common *sqsCL;
    ObjObject *objObject;
    ObjObject *objObject1;
    ObjObject *objObject2;
    ObjObject *objObject3;
    PlanePosition *planePosition0;
    PlanePosition *planePosition1;
    PlanePosition *planePosition2;
    PlanePosition *planePosition3;

    HANDLE hSemaphore[4];

    void drawTask0(SCBArangeThreadTask *stta);
    void drawTask1(SCBArangeThreadTask *stta);
    void drawTask2(SCBArangeThreadTask *stta);
    void drawTask3(SCBArangeThreadTask *stta);
    void drawSpec3D(SCBArangeThreadTask *stta, float pyx, float pyy, float pyz, float scx, float scy, float scz, ObjObject *oo, std::string texName, float *currMatrix, float *MVP);


    static float xAngle;
    static float yAngle;
    float zAngle;
    void initVulkanInstance();
    void enumerateVulkanPhysicalDevices();
    void createVulkanDevices();
    void createVulkanCommandBuffer();
    void createVulkanSwapChain();
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
    void destroyVulkanSwapChain();
    void destroyVulkanCommandBuffer();
    void destroyVulkanDevices();
    void destroyVulkanInstance();

    // TODO:在该需要重建交换链的地方修改他的值并重建交换链
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
    std::thread drawTask0Thread;
    static bool isDrawTask0ThreadShouldClose;
    std::thread drawTask1Thread;
    static bool isDrawTask1ThreadShouldClose;
    std::thread drawTask2Thread;
    static bool isDrawTask2ThreadShouldClose;
    std::thread drawTask3Thread;
    static bool isDrawTask3ThreadShouldClose;
};
