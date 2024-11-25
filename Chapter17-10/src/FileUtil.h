#pragma once
#include "TexDataObject.h"
#include <cstdint>
#include <string>
using namespace std;
typedef struct SpvDataStruct
{
    int size;
    uint32_t *data;
} SpvData;
class FileUtil {
public:
    static string loadAssetStr(string filename);
    static TexDataObject *loadCommonTexData(string filename);
    static TexDataObject *load_DXT5_BC3_TexData(string filename);
    static SpvData loadSPV(string filename);
};