#include "TexLeafDrawableObject.h"
#include "Constant.h"
#include "HelpFunction.h"
#include "MatrixState3D.h"
#include "TreeControl.h"
#include <assert.h>
#include <string.h>
TexLeafDrawableObject::TexLeafDrawableObject(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties, float pxIn, float pyIn, float pzIn, float tangle, float idIn) {
    pushConstantData = new float[16];
    this->devicePointer = &device;
    this->vdata = vdataIn;
    this->vCount = vCountIn;
    this->postionX = pxIn;
    this->postionY = pyIn;
    this->postionZ = pzIn;
    this->centerx = pxIn + Constant::leaves_width / 2 * cos(toRadians(tangle)); //计算当前中心X坐标
    this->centery = pyIn + Constant::LEAVE_CENTER_HEIGHT;                       //计算当前中心Y坐标
    this->centerz = pzIn + Constant::leaves_height / 2 * sin(toRadians(tangle));//计算当前中心Z坐标
    float *initialPos = new float[3]{
            centerx, centery, centerz};
    TreeControl::leafInitialPos.push_back(initialPos);
    this->centerAngle = tangle;
    this->id = idIn;
    pushConstantData = new float[16];
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
    printf("确定内存类型成功 类型索引为%d\n", alloc_info.memoryTypeIndex);
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
TexLeafDrawableObject::~TexLeafDrawableObject() {
    delete vdata;
    vkDestroyBuffer(*devicePointer, vertexDatabuf, nullptr);
    vkFreeMemory(*devicePointer, vertexDataMem, nullptr);
}
void TexLeafDrawableObject::drawSelf(VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer) {
    MatrixState3D::pushMatrix();
    MatrixState3D::rotate(centerAngle, 0, 1, 0);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, desSetPointer, 0, nullptr);
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
    vkCmdDraw(cmd, vCount, 1, 0, 0);
    MatrixState3D::popMatrix();
}
