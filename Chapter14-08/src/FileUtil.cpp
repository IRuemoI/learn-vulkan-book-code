#include "FileUtil.h"
#include "TexDataObject.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
int getfilesize(string filename) {
    FILE *fp;
    if ((fp = fopen(filename.c_str(), "r")) == nullptr)
        return 0;
    fseek(fp, 0, SEEK_END);
    return ftell(fp);
}
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
SpvData FileUtil::loadSPV(string filename) {
    size_t size = (getfilesize(filename));
    cout << "len:" << size << endl;
    SpvData spvData;
    spvData.size = size;
    spvData.data = (uint32_t *) (malloc(size));
    char *buf = (char *) spvData.data;
    char c_file[1000];
    strcpy(c_file, filename.c_str());
    FILE *fpSPV;
    fpSPV = fopen(c_file, "rb");
    if (fpSPV == nullptr) {
        printf("打开文件%s失败\n", filename.c_str());
    }
    fread(buf, size, 1, fpSPV);
    return spvData;
}
