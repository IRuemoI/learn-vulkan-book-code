#pragma once
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;// 指定使用的命名空间
typedef struct SpvDataStruct
{                  //存储SPIR-V 数据的结构体
    uint32_t size; //SPIR-V 数据总字节数
    uint32_t *data;//指向SPIR-V 数据内存块首地址的指针
} SpvData;

class FileUtil {
public:
    static string loadAssetStr(string filename);   // 加载Assets文件夹下的指定文本性质文件内容作为字符串返回
    static SpvData loadSPV(const string &filename);//加载文件夹下的SPIR-V 数据
};
