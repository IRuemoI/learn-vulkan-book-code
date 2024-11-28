#ifndef __H__LoadUtil__
#define __H__LoadUtil__
#include "ObjObject.h"
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
class LoadUtil {
public:
    static ObjObject *loadFromFile(const string &fname, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryroperties);
};
#endif
