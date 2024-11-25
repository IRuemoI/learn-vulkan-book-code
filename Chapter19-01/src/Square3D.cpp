#include "Square3D.h"
#include "LoadUtil.h"
#include "VulkanDemoApp.h"

//加载非压缩RGBA四通道普通纹理数据
ObjObject *Square3D::create(string objname, string texturename) {
    ObjObject *tempOO = LoadUtil::loadFromFile(objname, VulkanDemoApp::device, VulkanDemoApp::memoryProperties);
    tempOO->texturename = texturename;
    return tempOO;
}
