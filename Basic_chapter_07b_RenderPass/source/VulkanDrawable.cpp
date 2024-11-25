#include "VulkanDrawable.h"

#include "VulkanApplication.h"
#include "VulkanRenderer.h"

VulkanDrawable::VulkanDrawable(VulkanRenderer *parent) {
    // 注意需要使用0来填充内存
    memset(&VertexBuffer, 0, sizeof(VertexBuffer));
    rendererObj = parent;
}

VulkanDrawable::~VulkanDrawable() {
}

void VulkanDrawable::createVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture) {
    VulkanApplication *appObj = VulkanApplication::GetInstance();
    VulkanDevice *deviceObj = appObj->deviceObj;

    VkResult result;
    bool pass;

    // 创建缓存资源的元数据
    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.pNext = nullptr;
    bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufInfo.size = dataSize;
    bufInfo.queueFamilyIndexCount = 0;
    bufInfo.pQueueFamilyIndices = nullptr;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufInfo.flags = 0;

    // 创建缓存资源
    result = vkCreateBuffer(deviceObj->device, &bufInfo, nullptr, &VertexBuffer.buf);
    assert(result == VK_SUCCESS);

    // 获取缓存资源的内存需求
    VkMemoryRequirements memRqrmnt;
    vkGetBufferMemoryRequirements(deviceObj->device, VertexBuffer.buf, &memRqrmnt);

    // 创建内存分配元数据信息
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.memoryTypeIndex = 0;
    allocInfo.allocationSize = memRqrmnt.size;

    // 获取兼容的内存类型
    pass = deviceObj->memoryTypeFromProperties(memRqrmnt.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocInfo.memoryTypeIndex);
    assert(pass);

    // 为缓存资源分配实际的物理设备内存
    result = vkAllocateMemory(deviceObj->device, &allocInfo, nullptr, &(VertexBuffer.mem));
    assert(result == VK_SUCCESS);
    VertexBuffer.bufferInfo.range = memRqrmnt.size;
    VertexBuffer.bufferInfo.offset = 0;

    // 将物理设备内存区域映射到主机上
    uint8_t *pData;
    result = vkMapMemory(deviceObj->device, VertexBuffer.mem, 0, memRqrmnt.size, 0, (void **) &pData);
    assert(result == VK_SUCCESS);

    // 将数据复制到映射的内存
    memcpy(pData, vertexData, dataSize);

    // 取消物理设备内存映射
    vkUnmapMemory(deviceObj->device, VertexBuffer.mem);

    // 将已分配的缓存资源绑定到物理内存
    result = vkBindBufferMemory(deviceObj->device, VertexBuffer.buf, VertexBuffer.mem, 0);
    assert(result == VK_SUCCESS);

    // 在viIpBind中保存顶点数据的遍历方式
    viIpBind.binding = 0;
    viIpBind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    viIpBind.stride = dataStride;

    // 存储顶点数据解析信息
    viIpAttrb[0].binding = 0;
    viIpAttrb[0].location = 0;
    viIpAttrb[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    viIpAttrb[0].offset = 0;
    viIpAttrb[1].binding = 0;
    viIpAttrb[1].location = 1;
    viIpAttrb[1].format = useTexture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
    viIpAttrb[1].offset = 16;// After, 4 components - RGBA  each of 4 bytes(32bits)
}

void VulkanDrawable::destroyVertexBuffer() {
    VulkanDevice *deviceObj = rendererObj->getDevice();

    vkDestroyBuffer(deviceObj->device, VertexBuffer.buf, nullptr);
    vkFreeMemory(deviceObj->device, VertexBuffer.mem, nullptr);
}
