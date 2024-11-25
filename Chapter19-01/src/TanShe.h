#pragma once
#include <string>
#include "ObjObject.h"
#include "FileUtil.h"
#include "VulkanDemoApp.h"
using namespace std;//指定使用的命名空间
class TanShe {
public:
	static float tanshe_time;         //弹射time
	static bool tanshe_flag;         //弹射标志位
	static void TanSheCheck();    //检测是否弹射
};
