#include "VulkanLED.h"
#include "VulkanApplication.h"

VulkanLayerAndExtension::VulkanLayerAndExtension() {
}

VulkanLayerAndExtension::~VulkanLayerAndExtension() {
}

VkResult VulkanLayerAndExtension::getInstanceLayerProperties() {
    uint32_t instanceLayerCount;// 存储实例层的数目
    std::vector<VkLayerProperties> layerProperties;// 通过向量组来存储层的属性
    VkResult result;// 检查VulkanAPI的执行结果

    // 查询各个层的所有扩展并保存
    do {
        result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

        if (result)
            return result;

        if (instanceLayerCount == 0)
            return VK_INCOMPLETE;// 获取失败

        layerProperties.resize(instanceLayerCount);
        result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperties.data());
    } while (result == VK_INCOMPLETE);//不断尝试查询，直到查询成功

    // 查询每个层的所有跨站信息。并进行保存
    std::cout << "\n实例层列表：" << std::endl;
    std::cout << "===================" << std::endl;
    for (auto globalLayerProp: layerProperties) {
        // 输出层的名称和描述信息
        std::cout << "\n"
                  << globalLayerProp.description << "\n\t|\n\t|---[层名称]--> " << globalLayerProp.layerName << "\n";

        LayerProperties layerProps;
        layerProps.properties = globalLayerProp;

        // 根据层属性信息，获取实例级别的扩展
        result = getExtensionProperties(layerProps);

        if (result) {
            continue;
        }

        layerPropertyList.push_back(layerProps);
        // 输出每个实例层的扩展名称
        for (auto j: layerProps.extensions) {
            std::cout << "\t\t|\n\t\t|---[层扩展]--> " << j.extensionName << "\n";
        }
    }
    return result;
}

/*
* 获取设备扩展
*/

VkResult VulkanLayerAndExtension::getDeviceExtensionProperties(VkPhysicalDevice *gpu) {
    VkResult result;// 检查Vulkan API执行结果状态的变量

    // 查询所有层中的扩展列表，并保存
    std::cout << "设备扩展" << std::endl;
    std::cout << "===================" << std::endl;
    VulkanApplication *appObj = VulkanApplication::GetInstance();
    std::vector<LayerProperties> *instanceLayerProp = &appObj->GetInstance()->instanceObj.layerExtension.layerPropertyList;
    for (auto globalLayerProp: *instanceLayerProp) {
        LayerProperties layerProps;
        layerProps.properties = globalLayerProp.properties;

        if (result = getExtensionProperties(layerProps, gpu))
            continue;

        std::cout << "\n"
                  << globalLayerProp.properties.description << "\n\t|\n\t|---[层名称]--> " << globalLayerProp.properties.layerName << "\n";
        layerPropertyList.push_back(layerProps);

        if (layerProps.extensions.size()) {
            for (auto j: layerProps.extensions) {
                std::cout << "\t\t|\n\t\t|---[设备扩展]--> " << j.extensionName << "\n";
            }
        } else {
            std::cout << "\t\t|\n\t\t|---[设备扩展]--> 无扩展 \n";
        }
    }
    return result;
}

/// 这个函数可以获取实例界别或者设备级别的扩展及其属性信息。传入一个合法的物理设备(GPU)指针即可获取设备级别的扩展。传入NULL就可以获得实例级别的扩展了
/// \param layerProps 层属性变量的引用
/// \param gpu 物理设备句柄
/// \return
VkResult VulkanLayerAndExtension::getExtensionProperties(LayerProperties &layerProps, VkPhysicalDevice *gpu) {
    uint32_t extensionCount;// 保存每一层的扩展总属灵
    VkResult result;// VulkanAPI执行结果的变量
    char *layerName = layerProps.properties.layerName;// 层名称

    do {
        // 获取当前层的扩展总数
        if (gpu)
            result = vkEnumerateDeviceExtensionProperties(*gpu, layerName, &extensionCount, nullptr);
        else
            result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, nullptr);

        if (result || extensionCount == 0)
            continue;

        layerProps.extensions.resize(extensionCount);

        // 获取所有的扩展属性
        if (gpu)
            result = vkEnumerateDeviceExtensionProperties(*gpu, layerName, &extensionCount, layerProps.extensions.data());
        else
            result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, layerProps.extensions.data());
    } while (result == VK_INCOMPLETE);

    return result;
}
