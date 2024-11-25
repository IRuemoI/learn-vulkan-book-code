#pragma once
#include "CommonTexDataObject.h"
#include "LandData.h"
#include <string>
using namespace std;
class FileUtil {
public:
    static string loadAssetStr(string filename);
    static CommonTexDataObject *loadCommonTexData(string filename);
    static LandData *loadHdtData(string filename);
};
