#pragma once
#include <vulkan/vulkan.h>
class SkyCloudData {
public:
    static float *vdata;
    static int dataByteCount;
    static int vCount;
    static float radius;
    static float *generateTexCoor(int bw, int bh);
    static void initData();
};