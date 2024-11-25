#pragma once
#include "CommonTexDataObject.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;
class FileUtil {
public:
    static string loadAssetStr(string filename);
    static CommonTexDataObject *loadCommonTexData(string filename);
};
