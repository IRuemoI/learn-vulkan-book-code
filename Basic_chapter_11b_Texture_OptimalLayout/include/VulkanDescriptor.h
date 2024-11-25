#pragma once
#include "Headers.h"

class VulkanDevice;
class VulkanApplication;

class VulkanDescriptor {
public:
    // 构造函数
    VulkanDescriptor();
    // Destructor
    ~VulkanDescriptor();

    // 从描述符池中分配描述符集
    void createDescriptor(bool useTexture);
    // 删除已经创建的描述符集对象
    void destroyDescriptor();

    // 定义描述符集的布局绑定，创建描述符布局
    virtual void createDescriptorSetLayout(bool useTexture) = 0;
    // 删除描述符布局对象
    void destroyDescriptorLayout();

    // 创建描述符池，以便从中分配新的描述符集
    virtual void createDescriptorPool(bool useTexture) = 0;
    // 删除描述符池
    void destroyDescriptorPool();

    // Create Descriptor set associated resources before creating the descriptor set
    virtual void createDescriptorResources() = 0;

    // 从描述符池分配的内存中创建描述符集，并将描述符集信息更新到描述符池中
    virtual void createDescriptorSet(bool useTexture) = 0;
    void destroyDescriptorSet();

    // Creates the pipeline layout to inject into the pipeline
    virtual void createPipelineLayout() = 0;
    // Destroys the create pipelineLayout
    void destroyPipelineLayouts();

public:
    // Pipeline layout object
    VkPipelineLayout pipelineLayout;

    // List of all the VkDescriptorSetLayouts
    std::vector<VkDescriptorSetLayout> descLayout;

    // Decriptor pool object that will be used for allocating VkDescriptorSet object
    VkDescriptorPool descriptorPool;

    // List of all created VkDescriptorSet
    std::vector<VkDescriptorSet> descriptorSet;

    // �߼��豸 used for creating the descriptor pool and descriptor sets
    VulkanDevice *deviceObj;
};