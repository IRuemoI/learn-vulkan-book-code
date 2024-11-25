#pragma once

#include "CommonObject.h"
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
class LoadUtil {
public:
    static CommonObject *loadFromFile(const string &filename, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
};