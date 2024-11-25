#pragma once
#include <string>
using namespace std;
class MS3DHeader {
private:
    string id;
    int version;

public:
    MS3DHeader(unsigned char *binaryData, int *binaryData_index);
    ~MS3DHeader();
    string getId();
    int getVersion();
};
