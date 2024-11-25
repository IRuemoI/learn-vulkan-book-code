#pragma once
#include "CommonTexDataObject.h"
#include <string>

using namespace std;
class FileUtil {
public:
    static string loadAssetStr(string filename);
    static CommonTexDataObject *loadCommonTexData(string filename);
};