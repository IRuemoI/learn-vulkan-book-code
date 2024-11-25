#pragma once
#include "ObjObject.h"
#include "ObjObjectUt.h"
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
class LoadUtil {
public:
    static ObjObject *loadFromFile(const string &filename, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    static ObjObjectUt *loadFromFileUt(const string &filename, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
};