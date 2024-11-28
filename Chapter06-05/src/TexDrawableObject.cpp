#include "TexDrawableObject.h"
#include "HelpFunction.h"
#include "MatrixState3D.h"
#include <cassert>
#include <cstring>
TexDrawableObject::TexDrawableObject(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties) {
    pushConstantDataVertex = new float[16];
    pushConstantDataFrag = new float[1];
    this->devicePointer = &device;
    this->vdata = vdataIn;
    this->vCount = vCountIn;
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
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(device, vertexDatabuf, &mem_reqs);
    assert(dataByteCount <= mem_reqs.size);
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.memoryTypeIndex = 0;
    alloc_info.allocationSize = mem_reqs.size;
    VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bool flag = memoryTypeFromProperties(memoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
    assert(flag);
    printf("确定内存类型成功 类型索引为%d", alloc_info.memoryTypeIndex);
    result = vkAllocateMemory(device, &alloc_info, nullptr, &vertexDataMem);
    assert(result == VK_SUCCESS);
    uint8_t *pData;
    result = vkMapMemory(device, vertexDataMem, 0, mem_reqs.size, 0, (void **) &pData);
    assert(result == VK_SUCCESS);
    memcpy(pData, vdata, dataByteCount);
    vkUnmapMemory(device, vertexDataMem);
    result = vkBindBufferMemory(device, vertexDatabuf, vertexDataMem, 0);
    assert(result == VK_SUCCESS);
    vertexDataBufferInfo.buffer = vertexDatabuf;
    vertexDataBufferInfo.offset = 0;
    vertexDataBufferInfo.range = mem_reqs.size;
}
TexDrawableObject::~TexDrawableObject() {
    delete[] vdata;
    delete[] pushConstantDataVertex;
    delete[] pushConstantDataFrag;
    vkDestroyBuffer(*devicePointer, vertexDatabuf, nullptr);
    vkFreeMemory(*devicePointer, vertexDataMem, nullptr);
}
void TexDrawableObject::drawSelf(VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer, float lodLevel) {
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, desSetPointer, 0, nullptr);
    const VkDeviceSize offsetsVertex[1] = {0};
    vkCmdBindVertexBuffers(
            cmd,
            0,
            1,
            &(vertexDatabuf),
            offsetsVertex);
    float *mvp = MatrixState3D::getFinalMatrix();//获取最终变换矩阵
    memcpy(pushConstantDataVertex, mvp, sizeof(float) * 16);//将最终变换矩阵数据送入推送常量数据
    vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, pushConstantDataVertex);//将最终变换矩阵数据送入推送常量
    pushConstantDataFrag[0] = lodLevel;//纹理采样细节级别数据
    vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(float) * 16, sizeof(float) * 1, pushConstantDataFrag);//将纹理采样细节级别数据送入推送常量
    vkCmdDraw(cmd, vCount, 1, 0, 0);//执行绘制
}