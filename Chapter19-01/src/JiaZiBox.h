#pragma once
#include "FileUtil.h"
#include "ObjObject.h"
#include "VulkanDemoApp.h"
#include <string>
using namespace std;//指定使用的命名空间
class JiaZiBox {
public:
    static int jiazi_time;
    static bool jiazi_up;//地刺上升了
    static int JiaZiIndex;//地刺格子个数
    static void JiaZiTimeUpdate();
    static float JiaZiUpDown();//地刺升起与放下
    static void JiaZiCheckLife();//检测是否被地刺扎死
};
