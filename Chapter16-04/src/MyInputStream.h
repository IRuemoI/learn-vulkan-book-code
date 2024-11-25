#pragma once
#include <string>
using namespace std;
class MyInputStream {
public:
    string sourceName;
    unsigned char *binaryData;
    int binaryData_index = 0;
    unsigned char *loadBinary();
    int myReadInt();
    int *myReadInts(int length);
    short myReadUnsignedShort();
    int myReadByte();
    float myReadFloat();
    float *myReadFloats(int length);
    string myReadString();
    MyInputStream(string sourceName);
    ~MyInputStream();
};
