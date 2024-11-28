#pragma once
#include "TexDataObject.h"
#include <string>
using namespace std;
class FileUtil {
public:
    static string loadAssetStr(string filename);
    static TexDataObject *loadCommonTexData(string filename);
};
