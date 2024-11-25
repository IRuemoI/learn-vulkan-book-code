#pragma once
#include "SingleFlare.h"
#include <vector>
using namespace std;
class Flare {
public:
    Flare();                        //构造函数
    void initFlare();               //初始化光晕元素列表的方法
    vector<SingleFlare *> sFl;      //存放光源元素的列表
    void update(float lx, float ly);//更新光晕元素的方法
};