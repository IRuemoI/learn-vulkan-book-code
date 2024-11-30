#pragma once
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanLED.h"
#include "VulkanRenderer.h"

class VulkanApplication {
private:
    // CTOR: Application constructor responsible for layer enumeration.
    VulkanApplication();

public:
    // DTOR
    ~VulkanApplication();

private:
    // 单例单例
    static std::unique_ptr<VulkanApplication> instance;
    static std::once_flag onlyOnce;

public:
    static VulkanApplication *GetInstance();

    // 程序生命周期中的函数
    void initialize();// 初始化和分配资源
    void prepare();// 准备资源
    void update();// 更新数据
    void resize();// 重设窗口大小
    bool render();// 渲染循环
    void deInitialize();// 释放资源

private:
    VkResult createVulkanInstance(std::vector<const char *> &layers, std::vector<const char *> &extensions, const char *applicationName);
    VkResult handShakeWithDevice(VkPhysicalDevice *gpu, std::vector<const char *> &layers, std::vector<const char *> &extensions);
    VkResult enumeratePhysicalDevices(std::vector<VkPhysicalDevice> &gpus);

public:
    VulkanInstance instanceObj;
    VulkanDevice *deviceObj;
    VulkanRenderer *rendererObj;
    bool isPrepared;
    bool isResizing;

private:
    bool debugFlag;
    std::vector<VkPhysicalDevice> gpuList;
};