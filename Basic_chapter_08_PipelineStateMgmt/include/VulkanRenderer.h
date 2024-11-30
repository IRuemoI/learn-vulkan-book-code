#pragma once
#include "Headers.h"
#include "VulkanDrawable.h"
#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanSwapChain.h"

// 采样数需要在图像创建时相同，用于渲染通道创建（附件）和管道创建
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

// Vulkan Renderer是自定义类，它不是Vulkan特定的类
// 它可以作为一个窗口呈现管理器
// 它管理呈现窗口和绘图界面
class VulkanRenderer {
public:
    VulkanRenderer(VulkanApplication *app, VulkanDevice *deviceObject);
    ~VulkanRenderer();

public:
    //生命周期函数
    void initialize();
    void prepare();
    bool render();

    // 创建空白窗口
    void createPresentationWindow(const int &windowWidth = 500, const int &windowHeight = 500);
    void convertImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer &cmdBuf);

    //处理Windows窗口事件的回调函数
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // 销毁呈现窗口
    void destroyPresentationWindow();

    // 特定于类的成员变量的Getter函数
    inline VulkanApplication *getApplication() { return application; }
    inline VulkanDevice *getDevice() { return deviceObj; }
    inline VulkanSwapChain *getSwapChain() { return swapChainObj; }
    inline std::vector<VulkanDrawable *> *getDrawingItems() { return &drawableList; }
    inline VkCommandPool *getCommandPool() { return &cmdPool; }
    inline VulkanShader *getShader() { return &shaderObj; }
    inline VulkanPipeline *getPipelineObject() { return &pipelineObj; }

    void createCommandPool();// 创建指令池
    void buildSwapChainAndDepthImage();// 创建交换链颜色图和深度图
    void createDepthImage();// 创建深度图
    void createVertexBuffer();
    void createRenderPass(bool includeDepth, bool clear = true);// Render Pass creation
    void createFrameBuffer(bool includeDepth);
    void createShaders();
    void createPipelineStateManagement();

    void destroyCommandBuffer();
    void destroyCommandPool();
    void destroyDepthBuffer();
    void destroyDrawableVertexBuffer();
    void destroyRenderpass();// Destroy the render pass object when no more required
    void destroyFramebuffers();
    void destroyPipeline();

public:
#ifdef _WIN32
#define APP_NAME_STR_LEN 80
    HINSTANCE connection;// hInstance - Windows窗体实例
    char name[APP_NAME_STR_LEN];// name - 窗体名称
    HWND window;// hWnd - 窗体句柄
#else
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_intern_atom_reply_t *reply;
#endif

    // 深度图对应的数据结构
    struct
    {
        VkFormat format;
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } Depth;

    VkCommandBuffer cmdDepthImage;// 深度图指令缓存
    VkCommandPool cmdPool;// 指令池
    VkCommandBuffer cmdVertexBuffer;// Command buffer for vertex buffer - Triangle geometry

    VkRenderPass renderPass;// Render pass created object
    std::vector<VkFramebuffer> framebuffers;// 对应于每个交换链的帧缓冲的数量
    std::vector<VkPipeline *> pipelineList;// List of pipelines

    int width, height;

private:
    //本类的管理者对象
    VulkanApplication *application;
    // 与这个展示层关联的设备对象
    VulkanDevice *deviceObj;
    VulkanSwapChain *swapChainObj;
    std::vector<VulkanDrawable *> drawableList;
    VulkanShader shaderObj;
    VulkanPipeline pipelineObj;
};