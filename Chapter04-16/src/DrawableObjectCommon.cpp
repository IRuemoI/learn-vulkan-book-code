#include "DrawableObjectCommon.h"

#include <cassert>
#include <cstring>

#include "HelpFunction.h"
#include "MatrixState3D.h"

DrawableObjectCommonLight::DrawableObjectCommonLight(
        float *vdataIn,
        int dataByteCount,
        int vCountIn,//传入的顶点数据相关参数
        uint16_t *idataIn,
        int indexByteCount,
        int iCountIn,//传入的索引数据相关参数
        VkDevice &device,
        VkPhysicalDeviceMemoryProperties &memoryProperties) {
    pushConstantData = new float[16];                           //创建推送常量数据数组
    this->devicePointer = &device;                              //接收逻辑设备指针并保存
    this->vdata = vdataIn;                                      //接收顶点数据数组首地址指针并保存
    this->vCount = vCountIn;                                    //接收顶点数量并保存
    this->idata = idataIn;                                      //接收索引数据数组首地址指针并保存
    this->iCount = iCountIn;                                    //接收索引数量并保存
    createVertexBuffer(dataByteCount, device, memoryProperties);//调用方法创建顶点数据缓冲
    createIndexBuffer(indexByteCount, device, memoryProperties);//调用方法创建索引数据缓冲
    initDrawCmdbuf(device, memoryProperties);
}

void DrawableObjectCommonLight::createVertexBuffer(int dataByteCount, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties) {
    //创建Buffer创建信息实例，为创建顶点数据Buffer服务
    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.pNext = nullptr;
    buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buf_info.size = dataByteCount;
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = nullptr;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buf_info.flags = 0;
    VkResult result = vkCreateBuffer(device, &buf_info, nullptr, &vertexDatabuf);
    assert(result == VK_SUCCESS);

    //获取内存需求
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(device, vertexDatabuf, &mem_reqs);
    assert(dataByteCount <= mem_reqs.size);
    //内存分配信息
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.memoryTypeIndex = 0;
    alloc_info.allocationSize = mem_reqs.size;

    //需要的内存类型掩码
    VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    //获取所需内存类型索引
    bool flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
    if (flag) {
        printf("确定内存类型成功 类型索引为%d\n", alloc_info.memoryTypeIndex);
    } else {
        printf("确定内存类型失败!\n");
    }
    //为顶点数据缓冲分配内存
    result = vkAllocateMemory(device, &alloc_info, nullptr, &vertexDataMem);
    assert(result == VK_SUCCESS);

    uint8_t *pData;
    //将显存映射为CPU可访问
    result = vkMapMemory(device, vertexDataMem, 0, mem_reqs.size, 0, (void **) &pData);
    assert(result == VK_SUCCESS);

    //将顶点数据拷贝进显存
    memcpy(pData, vdata, dataByteCount);
    //解除内存映射
    vkUnmapMemory(device, vertexDataMem);

    //绑定内存与缓冲
    result = vkBindBufferMemory(device, vertexDatabuf, vertexDataMem, 0);
    assert(result == VK_SUCCESS);

    //记录Buffer Info
    vertexDataBufferInfo.buffer = vertexDatabuf;
    vertexDataBufferInfo.offset = 0;
    vertexDataBufferInfo.range = mem_reqs.size;
}

void DrawableObjectCommonLight::createIndexBuffer(int indexByteCount, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties) {
    //创建Buffer创建信息实例，为创建索引数据Buffer服务
    VkBufferCreateInfo index_buf_info = {};
    index_buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    index_buf_info.pNext = nullptr;
    index_buf_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    index_buf_info.size = indexByteCount;
    index_buf_info.queueFamilyIndexCount = 0;
    index_buf_info.pQueueFamilyIndices = nullptr;
    index_buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    index_buf_info.flags = 0;
    VkResult result = vkCreateBuffer(device, &index_buf_info, nullptr, &indexDatabuf);
    assert(result == VK_SUCCESS);

    //获取内存需求
    VkMemoryRequirements index_mem_reqs;
    vkGetBufferMemoryRequirements(device, indexDatabuf, &index_mem_reqs);
    assert(indexByteCount <= index_mem_reqs.size);
    //内存分配信息
    VkMemoryAllocateInfo index_alloc_info = {};
    index_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    index_alloc_info.pNext = nullptr;
    index_alloc_info.memoryTypeIndex = 0;
    index_alloc_info.allocationSize = index_mem_reqs.size;
    VkFlags index_requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    //获取所需内存类型索引
    bool index_flag = memoryTypeFromProperties(memoryProperties, index_mem_reqs.memoryTypeBits, index_requirements_mask, &index_alloc_info.memoryTypeIndex);
    if (index_flag) {
        printf("确定内存类型成功 类型索引为%d\n", index_alloc_info.memoryTypeIndex);
    } else {
        printf("确定内存类型失败!\n");
    }
    //为顶点数据缓冲分配内存
    result = vkAllocateMemory(device, &index_alloc_info, nullptr, &indexDataMem);
    assert(result == VK_SUCCESS);

    uint8_t *index_pData;
    //将显存映射为CPU可访问
    result = vkMapMemory(device, indexDataMem, 0, index_mem_reqs.size, 0, (void **) &index_pData);
    assert(result == VK_SUCCESS);

    //将顶点数据拷贝进显存
    memcpy(index_pData, idata, indexByteCount);
    //解除内存映射
    vkUnmapMemory(device, indexDataMem);

    //绑定内存与缓冲
    result = vkBindBufferMemory(device, indexDatabuf, indexDataMem, 0);
    assert(result == VK_SUCCESS);

    //记录Buffer Info
    indexDataBufferInfo.buffer = indexDatabuf;
    indexDataBufferInfo.offset = 0;
    indexDataBufferInfo.range = index_mem_reqs.size;
}

void DrawableObjectCommonLight::initDrawCmdbuf(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties) {
    indirectDrawCount = 2;//间接绘制信息数据组的数量
    drawCmdbufbytes = indirectDrawCount * sizeof(VkDrawIndexedIndirectCommand);
    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.pNext = nullptr;
    buf_info.usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    buf_info.size = drawCmdbufbytes;
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = nullptr;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buf_info.flags = 0;
    VkResult result = vkCreateBuffer(device, &buf_info, nullptr, &drawCmdbuf);
    assert(result == VK_SUCCESS);
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(device, drawCmdbuf, &mem_reqs);
    assert(drawCmdbufbytes <= mem_reqs.size);
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.memoryTypeIndex = 0;
    alloc_info.allocationSize = mem_reqs.size;
    VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bool flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
    if (flag) {
        printf("确定内存类型成功 类型索引为%d", alloc_info.memoryTypeIndex);
    } else {
        printf("确定内存类型失败!");
    }
    result = vkAllocateMemory(device, &alloc_info, nullptr, &drawCmdMem);
    assert(result == VK_SUCCESS);
    uint8_t *pData;
    result = vkMapMemory(device, drawCmdMem, 0, mem_reqs.size, 0, (void **) &pData);
    assert(result == VK_SUCCESS);
    VkDrawIndexedIndirectCommand dic[2];                           //创建间接绘制信息结构体实例数组
    dic[0].indexCount = iCount;                                    //第1 组绘制信息数据的索引数量
    dic[0].instanceCount = 1;                                      //第1 组绘制信息数据的实例数量
    dic[0].firstIndex = 0;                                         //第1 组绘制信息数据的绘制用起始索引
    dic[0].vertexOffset = 0;                                       //第1 组绘制信息数据的顶点数据偏移量
    dic[0].firstInstance = 0;                                      //第1 组绘制信息数据的首实例索引
    dic[1].indexCount = iCount / 2 + 1;                            //第2 组绘制信息数据的索引数量
    dic[1].instanceCount = 1;                                      //第2 组绘制信息数据的实例数量
    dic[1].firstIndex = 0;                                         //第2 组绘制信息数据的绘制用起始索引
    dic[1].vertexOffset = 0;                                       //第2 组绘制信息数据的顶点数据偏移量
    dic[1].firstInstance = 0;                                      //第2 组绘制信息数据的首实例索引
    memcpy(pData, &dic, drawCmdbufbytes);                          //将数据拷贝进设备内存
    vkUnmapMemory(device, vertexDataMem);                          //解除内存映射
    result = vkBindBufferMemory(device, drawCmdbuf, drawCmdMem, 0);//绑定内存与缓冲
    assert(result == VK_SUCCESS);                                  //检查绑定是否成功
}

DrawableObjectCommonLight::~DrawableObjectCommonLight() {
    delete[] vdata;                                         //释放顶点数据内存
    vkDestroyBuffer(*devicePointer, vertexDatabuf, nullptr);//销毁顶点数据缓冲
    vkFreeMemory(*devicePointer, vertexDataMem, nullptr);   //释放顶点数据缓冲对应设备内存
    delete[] idata;                                         //释放索引数据内存
    vkDestroyBuffer(*devicePointer, indexDatabuf, nullptr); //销毁索引数据缓冲
    vkFreeMemory(*devicePointer, indexDataMem, nullptr);    //释放索引数据缓冲对应设备内存
    vkDestroyBuffer(*devicePointer, drawCmdbuf, nullptr);
    vkFreeMemory(*devicePointer, drawCmdMem, nullptr);
}

void DrawableObjectCommonLight::drawSelf(VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer, int cmdDataOffset) {
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, desSetPointer, 0, nullptr);
    const VkDeviceSize offsetsVertex[1] = {0};
    vkCmdBindVertexBuffers(
            cmd,
            0,
            1,
            &(vertexDatabuf),
            offsetsVertex);
    float *mvp = MatrixState3D::getFinalMatrix();                                                                //获取总变换矩阵
    memcpy(pushConstantData, mvp, sizeof(float) * 16);                                                           //将总变换矩阵拷贝入内存
    vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, pushConstantData);//将常量数据送入管线

    vkCmdBindIndexBuffer(         //将顶点数据与当前使用的命令缓冲绑定
            cmd,                  //当前使用的命令缓冲
            indexDatabuf,         //索引数据缓冲
            0,                    //索引数据缓冲首索引
            VK_INDEX_TYPE_UINT16);//索引数据类型
    vkCmdDrawIndexedIndirect(
            cmd,                                  //当前使用的命令缓冲
            drawCmdbuf,                           //间接绘制信息数据缓冲
            cmdDataOffset,                        //绘制信息数据的起始偏移量（以字节计）
            1,                                    //此次绘制使用的间接绘制信息组的数量
            sizeof(VkDrawIndexedIndirectCommand));//每组绘制信息数据所占字节数
}