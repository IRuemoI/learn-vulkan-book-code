#pragma once
#include "ObjObject.h"
#include <cstdio>
#include <string>
#include <vector>
using namespace std;
class LoadUtil {
public:
    static ObjObject *loadFromFile(const string &filename, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
};