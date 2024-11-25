#pragma once
#include "LandData.h"
#include "TexDataObject.h"
#include "ThreeDTexDataObject.h"
#include <cstdint>
#include <string>
using namespace std;
class FileUtil {
public:
    static string loadAssetStr(string filename);
    static TexDataObject *loadCommonTexData(string filename);
    static ThreeDTexDataObject *load3DTexData(string filename);
};