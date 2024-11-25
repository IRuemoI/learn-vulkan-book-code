#pragma once

#include "Headers.h"
class VulkanInstance;
class VulkanDevice;
class VulkanRenderer;
class VulkanApplication;
/*
* Keep each of our swap chain buffers' image, command buffer and view in one spot
*/
struct SwapChainBuffer
{
    VkImage image;
    VkImageView view;
};

struct SwapChainPrivateVariables
{
    // 保存了窗口表面属性
    VkSurfaceCapabilitiesKHR surfCapabilities;

    // 保存了呈现模式的数量
    uint32_t presentModeCount;

    // 保存呈现模式的数据
    std::vector<VkPresentModeKHR> presentModes;

    // 交换链中颜色图的尺寸
    VkExtent2D swapChainExtent;

    // 可用的交换链图像数量
    uint32_t desiredNumberOfSwapChainImages;

    VkSurfaceTransformFlagBitsKHR preTransform;

    // 保存了交换链创建对应的呈现模式标识(按位与)
    VkPresentModeKHR swapchainPresentMode;

    // 交换链内图像对象
    std::vector<VkImage> swapchainImages;

    std::vector<VkSurfaceFormatKHR> surfFormats;
};

struct SwapChainPublicVariables
{
    // 系统平台相关的逻辑表面
    VkSurfaceKHR surface;

    // 交换链用到的缓存图像数量
    uint32_t swapchainImageCount;

    // 交换链对象
    VkSwapchainKHR swapChain;

    // 交换链图像列表
    std::vector<SwapChainBuffer> colorBuffer;

    // Semaphore for sync purpose
    VkSemaphore presentCompleteSemaphore;

    // 当前可用的绘制表面索引
    uint32_t currentColorBuffer;

    // 图像的格式
    VkFormat format;
};

class VulkanSwapChain {

    // Public member function
public:
    VulkanSwapChain(VulkanRenderer *renderer);
    ~VulkanSwapChain();
    void initializeSwapChain();
    void createSwapChain(const VkCommandBuffer &cmd);
    void destroySwapChain();


    // Private member variables
private:
    VkResult getSwapChainExtensions();
    void getSupportedFormats();
    VkResult createSurface();
    uint32_t getGraphicsQueueWithPresentationSupport();
    void getSurfaceCapabilitiesAndPresentMode();
    void managePresentMode();
    void createSwapChainColorImages();
    void createColorImageView(const VkCommandBuffer &cmd);

    // Public member variables
public:
    // 用户定义的结构体，其中包含了交换链的私有和共有函数用到各个变量
    SwapChainPublicVariables scPublicVars;
    PFN_vkQueuePresentKHR fpQueuePresentKHR;
    PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;

    // Private member variables
private:
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkDestroySurfaceKHR fpDestroySurfaceKHR;

    // Layer Extensions Debugging
    PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;

    // User define structure containing private variables used
    // by the swap chain private and public functions.
    SwapChainPrivateVariables scPrivateVars;
    VulkanRenderer *rendererObj;// parent
    VulkanApplication *appObj;
};
