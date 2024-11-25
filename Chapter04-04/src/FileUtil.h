#pragma once
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;// 指定使用的命名空间
class FileUtil {
public:
    static string loadAssetStr(string filename);// 加载Assets文件夹下的指定文本性质文件内容作为字符串返回
};
