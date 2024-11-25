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
};