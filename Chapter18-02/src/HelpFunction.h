#pragma once
#include "MatrixState3D.h"
#include "Vector3f.h"
#include <cstdint>
#include <vulkan/vulkan.h>
bool memoryTypeFromProperties(VkPhysicalDeviceMemoryProperties &memoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
void calculateABPosition(
        float x,
        float y,
        float w,
        float h,
        float rightIn,
        float topIn,
        float nearIn,
        float farIn,
        float *A,
        float *B);
void calSQ(float x, float y, float *A, float *B);
void changeObj(int index);