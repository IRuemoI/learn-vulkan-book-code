#pragma once
#include "FileUtil.h"
#include "ObjObject.h"
#include "VulkanDemoApp.h"
#include <string>
using namespace std;//指定使用的命名空间
class Square3D {
public:
    static ObjObject *create(string objname, string texturename);
};
