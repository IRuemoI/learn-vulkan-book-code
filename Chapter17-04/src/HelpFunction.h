#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>

float random(float start, float end);
float toRadians(float degree);//角度转换成弧度的方法
float toDegrees(float radian);//弧度转换成角度的方法
bool memoryTypeFromProperties(VkPhysicalDeviceMemoryProperties &memoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);