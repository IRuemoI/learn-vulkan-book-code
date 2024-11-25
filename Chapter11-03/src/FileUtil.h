#pragma once
#include "LandData.h"
#include "TexDataObject.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;// 指定使用的命名空间
class FileUtil {
public:
    static string loadAssetStr(string filename);// 加载Assets文件夹下的指定文本性质文件内容作为字符串返回
    static TexDataObject *loadCommonTexData(string filename);
    static LandData *loadHdtData(string filename);//加载灰度图数据，生成地形mesh数据
};
