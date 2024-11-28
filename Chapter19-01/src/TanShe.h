#pragma once
#include "FileUtil.h"
#include "ObjObject.h"
#include "VulkanDemoApp.h"
#include <string>
using namespace std;//指定使用的命名空间
class TanShe {
public:
    static float tanshe_time;//弹射time
    static bool tanshe_flag;//弹射标志位
    static void TanSheCheck();//检测是否弹射
};
