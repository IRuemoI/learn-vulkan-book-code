#include "ObjObject.h"
#include "HelpFunction.h"
#include "MatrixState3D.h"
#include "My3DLayer.h"
#include <assert.h>
#include <string.h>
#include <string>

ObjObject::ObjObject(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties, string texturename) {
    pushConstantData = new float[16];

    this->devicePointer = &device;
    this->vdata = vdataIn;
    this->vCount = vCountIn;
    this->texturename = texturename;
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->Rx = 0;
    this->Ry = 0;
    this->Rz = 0;
    this->Rangle = 0;
    this->Goorientation = Goorientationinit;
    //创建Buffer创建信息实，为创建顶点数据Buffer服务
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
        printf("确定内存类型成功 类型索引为%d", alloc_info.memoryTypeIndex);
    } else {
        printf("确定内存类型失败!");
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

ObjObject::~ObjObject() {
    delete vdata;
    vkDestroyBuffer(*devicePointer, vertexDatabuf, nullptr);
    vkFreeMemory(*devicePointer, vertexDataMem, nullptr);
}

void ObjObject::drawSelf(VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer) {
    //将当前使用的二级命令缓冲与管线绑定
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    //将二级命令缓冲、管线数据布局、描述集绑定
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, desSetPointer, 0, nullptr);
    //将定点数据与当前使用的二级命令缓冲绑定
    const VkDeviceSize offsetsVertex[1] = {0};
    vkCmdBindVertexBuffers(
            cmd,
            0,
            1,
            &(vertexDatabuf),
            offsetsVertex);

    float *mvp = MatrixState3D::getFinalMatrix();
    memcpy(pushConstantData, mvp, sizeof(float) * 16);
    vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, pushConstantData);
    //执行绘制
    vkCmdDraw(cmd, vCount, 1, 0, 0);
}


void ObjObject::setPosition3D(int x, int y, int z) {
    this->x = x;
    this->y = y;
    this->z = z;
}