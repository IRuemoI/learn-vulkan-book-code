#pragma once
#include "TexDataObject.h"
#include <string>
using namespace std;
class FileUtil {
public:
    static string loadAssetStr(string filename);
    static TexDataObject *loadCommonTexData(string filename);
    static TexDataObject *loadCubemapTexData(string filename);//加载非压缩RGBA四通道立方图纹理数据
};
