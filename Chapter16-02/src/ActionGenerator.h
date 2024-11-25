#pragma once
#include "Action.h"
#include <vector>
using namespace std;
class ActionGenerator {
public:
    static vector<Action *> acVector;
    static void genData();
    ~ActionGenerator();
};
