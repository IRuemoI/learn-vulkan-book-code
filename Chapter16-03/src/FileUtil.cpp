#include "FileUtil.h"
#include "TexDataObject.h"
#include <assert.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
string FileUtil::loadAssetStr(string filename) {
    ifstream infile;
    infile.open(filename.data());
    cout << "filename>" << filename << endl;
    assert(infile.is_open());
    string ss;
    string s;
    while (getline(infile, s)) {
        ss += s;
        ss += '\n';
    }
    infile.close();
    /*cout << ss << endl;*/
    return ss;
}
int fromBytesToInt(unsigned char *buff) {
    int k = 0;
    unsigned char *temp = (unsigned char *) (&k);
    temp[0] = buff[0];
    temp[1] = buff[1];
    temp[2] = buff[2];
    temp[3] = buff[3];
    return k;
}
TexDataObject *FileUtil::loadCommonTexData(string filename) {
    unsigned char *buf = new unsigned char[4];
    unsigned char *buf2 = new unsigned char[4];
    char c_file[100];
    strcpy(c_file, filename.c_str());
    FILE *fpPhoto;
    fpPhoto = fopen(c_file, "rb");
    if (fpPhoto == nullptr) {
        printf("打开文件失败\n");
    }
    fread(buf, 4, 1, fpPhoto);
    int width = fromBytesToInt(buf);
    fread(buf2, 4, 1, fpPhoto);
    int height = fromBytesToInt(buf2);
    unsigned char *data = new unsigned char[width * height * 4];
    fread(data, width * height * 4, 1, fpPhoto);
    TexDataObject *ctdo = new TexDataObject(width, height, data, width * height * 4);
    return ctdo;
}
unsigned char *FileUtil::loadBinary(std::string filename) {
    char c_file[100];
    strcpy(c_file, filename.c_str());
    FILE *fpPhoto;
    fpPhoto = fopen(c_file, "rb");
    if (fpPhoto == nullptr) {
        printf("打开文件失败\n");
    }
    fseek(fpPhoto, 0, SEEK_END);
    long size = ftell(fpPhoto);
    unsigned char *data = (unsigned char *) malloc(size + 1);
    fseek(fpPhoto, 0, SEEK_SET);
    fread(data, size, 1, fpPhoto);
    fclose(fpPhoto);
    return data;
}
int FileUtil::myReadInt(unsigned char *binaryData, int *binaryData_index) {
    int readInt = 0;
    unsigned char *readInt_char = (unsigned char *) &readInt;
    for (int i = 0; i < sizeof(int); i++) {
        readInt_char[i] = (unsigned char) binaryData[*binaryData_index];
        (*binaryData_index)++;
    }
    return readInt;
}
short FileUtil::myReadUnsignedShort(unsigned char *binaryData, int *binaryData_index) {
    short readShort = 0;
    unsigned char *readShort_char = (unsigned char *) &readShort;
    for (int i = 0; i < sizeof(short); i++) {
        readShort_char[i] = (unsigned char) binaryData[*binaryData_index];
        (*binaryData_index)++;
    }
    return readShort;
}
int FileUtil::myReadByte(unsigned char *binaryData, int *binaryData_index) {
    int readByte = 0;
    unsigned char *readByte_char = (unsigned char *) &readByte;
    for (int i = 0; i < 1; i++) {
        readByte_char[i] = (unsigned char) binaryData[*binaryData_index];
        (*binaryData_index)++;
    }
    return readByte;
}
float FileUtil::myReadFloat(unsigned char *binaryData, int *binaryData_index) {
    float readFloat = 0.0f;
    unsigned char *readFloat_char = (unsigned char *) &readFloat;
    for (int i = 0; i < sizeof(float); i++) {
        readFloat_char[i] = (unsigned char) binaryData[*binaryData_index];
        (*binaryData_index)++;
    }
    return readFloat;
}
string FileUtil::myReadString(unsigned char *binaryData, int *binaryData_index, int length) {
    int len = length;
    if (len == 0) {
        string s;
        return s;
    }
    unsigned char *ansChar = new unsigned char[len + 1];
    for (int i = 0; i < len; i++) {
        ansChar[i] = (unsigned char) binaryData[*binaryData_index];
        (*binaryData_index)++;
    }
    ansChar[len] = '\0';
    string str((const char *) ansChar);
    return str;
}
