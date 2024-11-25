#pragma once
#include "ThreeDTexDataObject.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;
class FileUtil {
public:
    static string loadAssetStr(string filename);
    static ThreeDTexDataObject *load3DTexData(string filename);
};
