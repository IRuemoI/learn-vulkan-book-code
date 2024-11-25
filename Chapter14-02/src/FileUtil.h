#pragma once
#include "TexDataObject.h"
#include <cstdint>
#include <string>
class FileUtil {
public:
    static std::string loadAssetStr(std::string filename);
    static TexDataObject *loadCommonTexData(std::string filename);
};