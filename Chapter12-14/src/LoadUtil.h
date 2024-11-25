#pragma once
#include "ObjObject.h"
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
class LoadUtil {
public:
    static ObjObject *loadFromFile(const string &filename, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    static size_t splitString(const string &strSrc, const string &strDelims, vector<string> &strDest);
    static float parseFloat(const char *token);
    static ObjObject *loadFromFileVertexAndTex(const string &filename, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryProperties);
    static bool tryParseDouble(const char *s, const char *s_end, double *result);
    static int parseInt(const char *token);
};
