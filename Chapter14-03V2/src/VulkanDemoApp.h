#pragma once
#include "ColorObject.h"
#include "ObjObject.h"
#include "ShaderQueueSuit_CommonDepth.h"
#include "ShaderQueueSuit_CommonTexLight.h"
#include <GLFW/glfw3.h>
#include <cstdlib>//标准库标准通用工具函数
#include <functional>//标准库与函数对象相关的模板类和函数
#include <iostream>//标准库输入输出头文件
#include <stdexcept>//标准库异常处理文件
#include <vulkan/vulkan.h>//引用Vulkan的头文件

class VulkanDemoApp {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    static uint32_t screenWidth;// 屏幕宽度
    static uint32_t screenHeight;// 屏幕高度
    // 窗口辅助结构体
    GLFWwindow *window;
    std::vector<const char *> instanceExtensionNames;// 需要使用的实例扩展名称列表
    VkInstance instance;// Vulkan实例
    uint32_t gpuCount;// 物理设备数量
    std::vector<VkPhysicalDevice> gpus;// 物理设备列表
    uint32_t queueFamilyCount;// 物理设备对应的队列家族数量
    std::vector<VkQueueFamilyProperties> queueFamilyProps;// 物理设备对应的队列家族属性列表
    uint32_t queueGraphicsFamilyIndex;// 支持图形工作的队列家族索引
    VkQueue queueGraphics;// 支持图形工作的队列
    uint32_t queuePresentFamilyIndex;// 支持显示工作的队列家族索引
    std::vector<const char *> deviceExtensionNames;// 所需的设备扩展名称列表
    VkDevice device;// 逻辑设备
    VkCommandPool cmdPool;// 命令池
    VkCommandBuffer cmdBuffer;// 命令缓冲
    VkCommandBufferBeginInfo cmd_buf_info;// 命令缓冲启动信息
    VkCommandBuffer cmd_buffers[1];// 供提交执行的命令缓冲数组
    VkSubmitInfo submit_info[1];// 命令缓冲提交执行信息数组
    VkSurfaceKHR surface;// KHR表面
    std::vector<VkFormat> formats;// KHR表面支持的格式
    VkSurfaceCapabilitiesKHR surfCapabilities;// 表面的能力
    uint32_t presentModeCount;// 显示模式数量
    std::vector<VkPresentModeKHR> presentModes;// 显示模式列表
    VkExtent2D swapchainExtent;// 交换链尺寸
    VkSwapchainKHR swapChain;// 交换链

    uint32_t swapchainImageCount;// 交换链中的图像数量
    std::vector<VkImage> swapchainImages;// 交换链中的图像列表
    std::vector<VkImageView> swapchainImageViews;// 交换链对应的的图像视图列表
    VkFormat depthFormat;// 深度图像格式
    VkFormatProperties depthFormatProps;// 物理设备支持的深度格式属性
    VkImage depthImage;// 深度缓冲图像
    VkPhysicalDeviceMemoryProperties memoryProperties;// 物理设备内存属性
    VkDeviceMemory memDepth;// 深度缓冲图像对应的内存
    VkImageView depthImageView;// 深度缓冲图像视图
    VkSemaphore imageAcquiredSemaphore;// 渲染目标图像获取完成信号量
    uint32_t currentBuffer;// 从交换链中获取的当前渲染用图像对应的缓冲编号
    VkRenderPass renderPass;// 渲染通道
    VkClearValue clear_values[2];// 渲染通道用清除帧缓冲深度、颜色附件的数据
    VkRenderPassBeginInfo rp_begin;// 渲染通道启动信息
    VkFence taskFinishFence;// 等待任务完毕的栅栏
    VkPresentInfoKHR present;// 呈现信息
    VkFramebuffer *framebuffers;// 帧缓冲序列首指针

    VkFormat colorFormat;
    VkFormatProperties colorFormatProps;
    VkImage colorImage;
    VkDeviceMemory memColor;
    VkImageView colorImageView;
    VkDescriptorImageInfo colorImageInfo;

    VkFramebuffer selfTexFramebuffer;
    ShaderQueueSuit_CommonDepth *sqsCD;
    ShaderQueueSuit_CommonTexLight *sqsCTL;
    ObjObject *CHForDraw;
    ObjObject *carForDraw;
    ObjObject *treeForDraw;
    ObjObject *pmForDraw;
    static float yAngle;
    static float zAngle;

    //拖动光标的变化量
    static double cursorDeltaX;
    static double cursorDeltaY;

    //当前贯标的坐标
    static double currentCursorX;
    static double currentCursorY;

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
    void createVulkanSelfColorBuffer();
    void destroyVulkanSelfColorBuffer();
    void createRenderPass();
    void destroyRenderPass();
    void initQueue();
    void createFrameBuffer();
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
