#pragma once
#include <set>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
class Normal {
public:
    float nx;
    float ny;
    float nz;
    static float *result;
    static bool exist(Normal *normal, set<Normal *> sn);
    static float *getAverage(set<Normal *> sn);
    Normal(float nx, float ny, float nz);
    ~Normal();
};