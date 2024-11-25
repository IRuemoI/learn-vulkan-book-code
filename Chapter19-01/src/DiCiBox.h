#pragma once
#include "FileUtil.h"
#include "ObjObject.h"
#include "VulkanDemoApp.h"
#include <string>
using namespace std;//指定使用的命名空间
class DiCiBox {
public:
    static int dici_time;
    static bool dici_up;//地刺上升了
    static int DiCiIndex;//地刺格子个数
    static float DiCiUpDown();//地刺升起与放下
    static void DiCiTimeUpdate();//地刺升起下落
    static void DiCiCheckLife();//检测是否被地刺扎死
};
