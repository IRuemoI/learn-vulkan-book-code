#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
bool memoryTypeFromProperties(VkPhysicalDeviceMemoryProperties &memoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
float toRadians(float degree);
std::vector<float> generateSkyTexCoor(int bw, int bh);