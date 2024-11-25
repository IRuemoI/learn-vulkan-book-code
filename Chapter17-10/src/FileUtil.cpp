#include "FileUtil.h"
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
    printf(filename.c_str());
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
    TexDataObject *ctdo = new TexDataObject(width, height, data, width * height * 4, COMMON);
    return ctdo;
}
int mymax(int a, int b) {
    return (a > b) ? a : b;
}
int textureLevelSizeS3tcDxt5(int width, int height) {
    return ((width + 3) >> 2) * ((height + 3) >> 2) * 16;
}
TexDataObject *FileUtil::load_DXT5_BC3_TexData(string filename) {
#define DDS_HEADER_LENGTH 31
#define DDS_HEADER_HEIGHT 3
#define DDS_HEADER_WIDTH 4
#define DDS_HEADER_SIZE 1
#define DDSD_MIPMAPCOUNT 0x20000
#define DDS_HEADER_MIPMAPCOUNT 7
#define DDS_HEADER_FLAGS 2
    printf("%s\n", filename.c_str());
    char c_file[100];
    strcpy(c_file, filename.c_str());
    FILE *fpPhoto;
    fpPhoto = fopen(c_file, "rb");
    if (fpPhoto == nullptr) {
        printf("打开文件失败\n");
    }
    fseek(fpPhoto, 0, SEEK_END);
    int fileBytesCount = ftell(fpPhoto);
    printf("fileBytesCount %d", fileBytesCount);
    fseek(fpPhoto, 0, 0);
    unsigned char *dataTemp = new unsigned char[fileBytesCount];
    fread(dataTemp, fileBytesCount, 1, fpPhoto);
    int *headerI = (int *) dataTemp;
    int width = headerI[DDS_HEADER_WIDTH];
    printf("width=%d\n", width);
    int height = headerI[DDS_HEADER_HEIGHT];
    printf("height=%d\n", height);
    int levels = 1;
    if (headerI[DDS_HEADER_FLAGS] & DDSD_MIPMAPCOUNT) {
        levels = mymax(1, headerI[DDS_HEADER_MIPMAPCOUNT]);
    }
    printf("levels=%d %d\n", levels, headerI[DDS_HEADER_MIPMAPCOUNT]);
    int dataOffset = headerI[DDS_HEADER_SIZE] + 4;
    unsigned char *dxtData = dataTemp + dataOffset;
    TexDataObject *result = nullptr;
    int offset = 0;
    for (int i = 0; i < levels; ++i) {
        int levelSize = textureLevelSizeS3tcDxt5(width, height);
        printf("levelSize %d offset %d\n", levelSize, offset);
        unsigned char *dataLevel = new unsigned char[levelSize];
        memcpy(dataLevel, dxtData, levelSize);
        result = new TexDataObject(width, height, dataLevel, levelSize, RGBA8_BC3_DXT5);
        width = width >> 1;
        height = height >> 1;
        offset += levelSize;
        break;
    }
    fclose(fpPhoto);
    delete dataTemp;
    return result;
}