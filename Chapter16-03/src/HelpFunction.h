#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
using namespace std;
bool memoryTypeFromProperties(VkPhysicalDeviceMemoryProperties &memoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
string texName2bntex(const string &texName);
size_t splitString(const string &strSrc, const string &strDelims, vector<string> &strDest);