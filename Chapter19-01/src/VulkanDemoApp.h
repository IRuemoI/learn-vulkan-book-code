#pragma once
#include "Dashboard2DObject.h"
#include "ShaderQueueSuit_CommonTexLight.h"
#include "ShaderQueueSuit_Dashboard2D.h"
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

    //vulkan实例
    VkInstance instance;
    //物理设备数量
    uint32_t gpuCount;
    //物理设备列表
    std::vector<VkPhysicalDevice> gpus;
    //物理设备对应的队列族数量
    uint32_t queueFamilyCount;
    //物理设备对应的队列族列表
    std::vector<VkQueueFamilyProperties> queueFamilyProps;
    //支持GRAPHICS作业的一个队列族的索引
    uint32_t queueGraphicsFamilyIndex;
    //支持Graphics的队列
    VkQueue queueGraphics;
    //支持呈现作业作业的一个队列族的索引
    uint32_t queuePresentFamilyIndex;
    //逻辑设备所需的扩展
    std::vector<const char *> deviceExtensionNames;
    //创建的逻辑设备
    static VkDevice device;
    //创建的命令池
    VkCommandPool cmdPool;
    //创建的一级命令缓冲
    static VkCommandBuffer cmdBuffer;
    //一级命令缓冲启动信息
    VkCommandBufferBeginInfo cmd_buf_info;
    //供执行的命令缓冲数组
    VkCommandBuffer cmd_buffers[1];
    //一级命令缓冲提交执行信息
    VkSubmitInfo submit_info[1];

    //创建的Surface
    VkSurfaceKHR surface;
    //支持的格式
    std::vector<VkFormat> formats;
    //surface的能力
    VkSurfaceCapabilitiesKHR surfCapabilities;
    //呈现模式数量
    uint32_t presentModeCount;
    //呈现模式列表
    std::vector<VkPresentModeKHR> presentModes;
    //swapchain尺寸
    VkExtent2D swapchainExtent;
    //创建的swapchain
    VkSwapchainKHR swapChain;
    //swapchain中的Image数量
    uint32_t swapchainImageCount;
    //swapchain中的Image列表
    std::vector<VkImage> swapchainImages;
    //swapchain对应的的Image列表
    std::vector<VkImageView> swapchainImageViews;
    //深度Image格式
    VkFormat depthFormat;
    //物理设备支持的格式属性--为深度缓冲服务
    VkFormatProperties depthFormatProps;
    //深度缓冲Image
    VkImage depthImage;
    //物理设备内存属性
    static VkPhysicalDeviceMemoryProperties memoryProperties;
    //深度缓冲内存
    VkDeviceMemory memDepth;
    //深度缓冲ImageView
    VkImageView depthImageView;

    //渲染用Image准备完成信号量
    VkSemaphore imageAcquiredSemaphore;
    //从swapChain中获取的当前渲染用Image的缓冲编号或称之为索引
    uint32_t currentBuffer;
    //渲染通道
    VkRenderPass renderPass;
    //渲染通道用清除帧缓冲数据
    VkClearValue clear_values[2];
    //渲染通道启动信息
    VkRenderPassBeginInfo rp_begin;
    //等待任务完毕Fence
    VkFence taskFinishFence;
    //呈现信息
    VkPresentInfoKHR present;

    //帧缓冲
    VkFramebuffer *framebuffers;

    //纹理光照普通3D物体着色器管线套装
    static ShaderQueueSuit_CommonTexLight *sqsCTL;
    //纹理仪表板2D物体着色器管线套装
    static ShaderQueueSuit_Dashboard2D *sqsD2D;

    //Obj绘制者列表
    //     ObjObject* planeForDraw;

    //绘制用仪表板
    static Dashboard2DObject *d2dA;//游戏名称

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
    void createRenderPass();
    void destroyRenderPass();
    void initQueue();
    void createFrameBuffer();
    void destroyFrameBuffer();
    void createDrawableObject();
    void destroyDrawableObject();

    void drawObject();
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


    // 三角形应用运行函数
    void run();

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

    std::vector<const char *> get_required_extensions();
};
