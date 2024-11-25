#pragma once
#include "TexDrawableObject.h"
class TreeControl {
public:
    static void sortForTree();
    static bool compare(TexDrawableObject *A, TexDrawableObject *B);
    static void calculateBillboardDirection();
};
