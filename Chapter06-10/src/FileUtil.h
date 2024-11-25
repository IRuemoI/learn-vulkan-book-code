#pragma once
#include "TexArrayDataObject.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;
class FileUtil {
public:
    static string loadAssetStr(string filename);
    static TexArrayDataObject *load2DArrayTexData(string filename);
};
