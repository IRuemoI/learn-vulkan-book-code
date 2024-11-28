#pragma once
#include "ColorObject.h"
#include "Dashboard2DObject.h"
#include "ObjObject.h"
#include "ShaderQueueSuit_Common.h"
#include "ShaderQueueSuit_CommonTex.h"
#include "ShaderQueueSuit_Dashboard2D.h"
#include <GLFW/glfw3.h>
#include <cstdlib>//标准库标准通用工具函数
#include <functional>//标准库与函数对象相关的模板类和函数
#include <iostream>//标准库输入输出头文件
#include <mutex>
#include <stdexcept>//标准库异常处理文件
#include <thread>
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
    VkRenderPass renderPass;
    VkClearValue clear_values[2];
    VkRenderPassBeginInfo rp_begin;
    VkFence taskFinishFence;
    VkPresentInfoKHR present;
    VkFramebuffer *framebuffers;
    ShaderQueueSuit_Common *sqsCL;
    ShaderQueueSuit_CommonTex *sqsCT;
    ShaderQueueSuit_Dashboard2D *sqsD2D;
    Dashboard2DObject *sky;//指向天空绘制对象的指针
    Dashboard2DObject *landForDraw;//指向山地绘制对象的指针
    ObjObject *tree1;//指向第一种树绘制对象的指针
    ObjObject *tree2;//指向第二种树绘制对象的指针
    Dashboard2DObject *d2dA;//指向纹理矩形绘制对象的指针
    static float SPAN;
    static float TIMESPAN;
    static float cz;
    static float targetz;
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
    void updatePosition();
    void drawObject();
    void initTextures();
    void initPipeline();
    void createFence();
    void initPresentInfo();
    void initMatrix();
    void destroyFence();
    void destroyPipeline();
    void destroyDrawableObject();
    void destroyTexture();
    void destroyFrameBuffer();
    void destroyRenderPass();
    void destroyVulkanDepthBuffer();
    void destroyVulkanSwapChain();
    void destroyVulkanCommandBuffer();
    void destroyVulkanDevices();
    void destroyVulkanInstance();
    VkFormat colorFormat;
    VkFormatProperties colorFormatProps;
    VkImage colorImage[5];//颜色附件图像数组
    VkDeviceMemory memColor[5];//颜色附件图像对应的设备内存数组
    VkImageView colorImageView[5];//颜色附件图像视图数组
    VkDescriptorImageInfo colorImageInfo[5];//颜色附件图像信息数组
    VkFramebuffer selfTexFramebuffer[5];//服务于绘制到纹理的专用帧缓冲数组
    void drawSceneToTex();
    void drawSceneToScreen();
    void flushUniformBufferForToTex();
    void flushTexToDesSetForToTex();
    void flushUniformBufferForToScreen();
    void flushTexToDesSetForToScreen();
    void create_vulkan_SelfColorBuffer();
    void destroy_vulkan_SelfColorBuffer();
    void drawTrees(int treeIndex, float transX, float transY, float transZ);//绘制树的方法

    std::thread treeMoveThread;
    static bool isTreeMoveThreadShouldClose;

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
};
