#pragma once
#include "TexDataObject.h"
#include <cstdint>
#include <string>
using namespace std;
typedef struct SpvDataStruct
{//存储SPIR-V 数据的结构体
    uint32_t size;//SPIR-V 数据总字节数
    uint32_t *data;//指向SPIR-V 数据内存块首地址的指针
} SpvData;
class FileUtil {
public:
    static string loadAssetStr(string filename);
    static TexDataObject *loadCommonTexData(string filename);
    static SpvData loadSPV(const string &filename);//加载文件夹下的SPIR-V 数据
};
