#pragma once
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "TexDataObject.h"
using namespace std;
class FileUtil {
public:
    static string loadAssetStr(string filename);
    static TexDataObject *loadCommonTexData(string filename);
    static unsigned char *loadBinary(std::string filename);
    static int myReadInt(unsigned char *binaryData, int *binaryData_index);
    static short myReadUnsignedShort(unsigned char *binaryData, int *binaryData_index);
    static int myReadByte(unsigned char *binaryData, int *binaryData_index);
    static float myReadFloat(unsigned char *binaryData, int *binaryData_index);
    static string myReadString(unsigned char *binaryData, int *binaryData_index, int length);

private:
};