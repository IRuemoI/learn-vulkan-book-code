#include "BnggdhDraw.h"
#include "TextureManager.h"
#include "VulkanDemoApp.h"
BnggdhDraw::BnggdhDraw(string sourceName, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties) {
    pushConstantData = new float[32];
    this->devicePointer = &device;
    this->memoryPropertiesPointer = &memoryProperties;
    bnggdh = new Bnggdh(sourceName);
    initVertexData();
    initIndexData();
    initVertexBuffer();
    initIndexBuffer();
}
void BnggdhDraw::initVertexData() {
    maxKeyTime = bnggdh->getMaxKeytime();
    vCount = bnggdh->getVerNums();
    vdata = new float[vCount * 8]();
    vDataTransfer = new float[vCount * 8]();
    vdataByteCount = vCount * 8 * sizeof(float);
    /*======================================================组装顶点数据Begin====================================================*/
    int count = 0;
    positionData = bnggdh->getPosition();
    textureData = bnggdh->getTextures();
    normalData = bnggdh->getCurrentNormal();
    for (int i = 0; i < vCount; i++) {
        vdata[count++] = positionData[i * 3 + 0];
        vdata[count++] = positionData[i * 3 + 1];
        vdata[count++] = positionData[i * 3 + 2];
        vdata[count++] = textureData[i * 2 + 0];
        vdata[count++] = textureData[i * 2 + 1];
        vdata[count++] = normalData[i * 3 + 0];
        vdata[count++] = normalData[i * 3 + 1];
        vdata[count++] = normalData[i * 3 + 2];
    }
    count = 0;
    for (int i = 0; i < vCount; i++) {
        vDataTransfer[count++] = positionData[i * 3 + 0];
        vDataTransfer[count++] = positionData[i * 3 + 1];
        vDataTransfer[count++] = positionData[i * 3 + 2];
        vDataTransfer[count++] = textureData[i * 2 + 0];
        vDataTransfer[count++] = textureData[i * 2 + 1];
        vDataTransfer[count++] = normalData[i * 3 + 0];
        vDataTransfer[count++] = normalData[i * 3 + 1];
        vDataTransfer[count++] = normalData[i * 3 + 2];
    }
}
void BnggdhDraw::initIndexData() {
    iCount = bnggdh->getIndexNums();
    idata = new uint16_t[iCount]();
    idataByteCount = iCount * sizeof(uint16_t);
    /*======================================================组装索引数据Begin====================================================*/
    for (int i = 0; i < iCount; i++) {
        idata[i] = bnggdh->getIndices()[i];
    }
    /*======================================================组装索引数据End======================================================*/
}
void BnggdhDraw::initVertexBuffer() {
    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.pNext = nullptr;
    buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buf_info.size = vdataByteCount;
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = nullptr;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buf_info.flags = 0;
    VkResult result = vkCreateBuffer(*devicePointer, &buf_info, nullptr, &vertexDatabuf);
    assert(result == VK_SUCCESS);
    vkGetBufferMemoryRequirements(*devicePointer, vertexDatabuf, &vmem_reqs);
    assert(vdataByteCount <= vmem_reqs.size);
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.memoryTypeIndex = 0;
    alloc_info.allocationSize = vmem_reqs.size;
    VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bool flag = memoryTypeFromProperties(*memoryPropertiesPointer, vmem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
    if (flag) {
        printf("确定内存类型成功 类型索引为%d\n", alloc_info.memoryTypeIndex);
    } else {
        printf("确定内存类型失败!\n");
    }
    result = vkAllocateMemory(*devicePointer, &alloc_info, nullptr, &vertexDataMem);
    assert(result == VK_SUCCESS);
    uint8_t *pData;
    result = vkMapMemory(*devicePointer, vertexDataMem, 0, vmem_reqs.size, 0, (void **) &pData);
    assert(result == VK_SUCCESS);
    memcpy(pData, vdata, vdataByteCount);
    vkUnmapMemory(*devicePointer, vertexDataMem);
    result = vkBindBufferMemory(*devicePointer, vertexDatabuf, vertexDataMem, 0);
    assert(result == VK_SUCCESS);
    vertexDataBufferInfo.buffer = vertexDatabuf;
    vertexDataBufferInfo.offset = 0;
    vertexDataBufferInfo.range = vmem_reqs.size;
}
void BnggdhDraw::initIndexBuffer() {
    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.pNext = nullptr;
    buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buf_info.size = idataByteCount;
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = nullptr;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buf_info.flags = 0;
    VkResult result = vkCreateBuffer(*devicePointer, &buf_info, nullptr, &indexDatabuf);
    assert(result == VK_SUCCESS);
    vkGetBufferMemoryRequirements(*devicePointer, indexDatabuf, &imem_reqs);
    assert(idataByteCount <= imem_reqs.size);
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.memoryTypeIndex = 0;
    alloc_info.allocationSize = imem_reqs.size;
    VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bool flag = memoryTypeFromProperties(*memoryPropertiesPointer, imem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
    if (flag) {
        printf("确定内存类型成功 类型索引为%d\n", alloc_info.memoryTypeIndex);
    } else {
        printf("确定内存类型失败!\n");
    }
    result = vkAllocateMemory(*devicePointer, &alloc_info, nullptr, &indexDataMem);
    assert(result == VK_SUCCESS);
    uint8_t *pData;
    result = vkMapMemory(*devicePointer, indexDataMem, 0, imem_reqs.size, 0, (void **) &pData);
    assert(result == VK_SUCCESS);
    memcpy(pData, idata, idataByteCount);
    vkUnmapMemory(*devicePointer, indexDataMem);
    result = vkBindBufferMemory(*devicePointer, indexDatabuf, indexDataMem, 0);
    assert(result == VK_SUCCESS);
    indexDataBufferInfo.buffer = indexDatabuf;
    indexDataBufferInfo.offset = 0;
    indexDataBufferInfo.range = imem_reqs.size;
}
void BnggdhDraw::updateData(float time) {
    bnggdh->updata(time);
    positionData = bnggdh->getPosition();
    textureData = bnggdh->getTextures();
    normalData = bnggdh->getCurrentNormal();
}
void BnggdhDraw::copyData() {
    int count = 0;
    for (int i = 0; i < vCount; i++) {
        vDataTransfer[count++] = positionData[i * 3 + 0];
        vDataTransfer[count++] = positionData[i * 3 + 1];
        vDataTransfer[count++] = positionData[i * 3 + 2];
        vDataTransfer[count++] = textureData[i * 2 + 0];
        vDataTransfer[count++] = textureData[i * 2 + 1];
        vDataTransfer[count++] = normalData[i * 3 + 0];
        vDataTransfer[count++] = normalData[i * 3 + 1];
        vDataTransfer[count++] = normalData[i * 3 + 2];
    }
}
void BnggdhDraw::referVertexBuffer() {
    memcpy(vdata, vDataTransfer, vdataByteCount);
}
void BnggdhDraw::drawSelf(string picName, VkCommandBuffer &cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline) {
    uint8_t *pData;
    VkResult result = vkMapMemory(*devicePointer, vertexDataMem, 0, vmem_reqs.size, 0, (void **) &pData);
    assert(result == VK_SUCCESS);
    memcpy(pData, vdata, vdataByteCount);
    vkUnmapMemory(*devicePointer, vertexDataMem);
    VkDescriptorSet desSetPointer = VulkanDemoApp::sqsCL->descSet[TextureManager::getVkDescriptorSetIndex(picName)];
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &desSetPointer, 0, nullptr);
    const VkDeviceSize offsetsVertex[1] = {0};
    vkCmdBindVertexBuffers(cmd, 0, 1, &(vertexDatabuf), offsetsVertex);
    vkCmdBindIndexBuffer(cmd, indexDatabuf, 0, VK_INDEX_TYPE_UINT16);
    float *mvp = MatrixState3D::getFinalMatrix();
    float *mm = MatrixState3D::getMMatrix();
    memcpy(pushConstantData, mvp, sizeof(float) * 16);
    memcpy(pushConstantData + 16, mm, sizeof(float) * 16);
    vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 32, pushConstantData);
    vkCmdDrawIndexed(cmd, iCount, 1, 0, 0, 0);
}
BnggdhDraw::~BnggdhDraw() {
    delete bnggdh;
    delete[] positionData;
    delete[] textureData;
    delete[] normalData;
    delete[] pushConstantData;
    delete[] vdata;
    vkDestroyBuffer(*devicePointer, vertexDatabuf, nullptr);
    vkFreeMemory(*devicePointer, vertexDataMem, nullptr);
    vkDestroyBuffer(*devicePointer, indexDatabuf, nullptr);
    vkFreeMemory(*devicePointer, indexDataMem, nullptr);
}