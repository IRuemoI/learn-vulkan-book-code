#pragma once
#include "TexDataObject.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;
class FileUtil {
public:
    static string loadAssetStr(string filename);
    static TexDataObject *loadCommonTexData(string filename);
    static TexDataObject *load_DXT5_BC3_TexData(const string &filename);
};
