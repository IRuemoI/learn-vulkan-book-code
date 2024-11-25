#pragma once
#include "ObjObject.h"
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
class LoadUtil {
public:
    static ObjObject *loadFromFile(const string &fname, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryroperties);
};