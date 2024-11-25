#pragma once
#include <string>
#include <vulkan/vulkan.h>
using namespace std;
bool memoryTypeFromProperties(VkPhysicalDeviceMemoryProperties &memoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
int parseInt(const char *token);
float toRadians(float f);