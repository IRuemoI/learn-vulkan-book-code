#include "FileUtil.h"
#include "TexDataObject.h"
#include <assert.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
string FileUtil::loadAssetStr(string fname) {
    ifstream infile;
    infile.open(fname.data());
    cout << "fname>" << fname << endl;
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
TexDataObject *FileUtil::loadCommonTexData(string fname) {
    unsigned char *buf = new unsigned char[4];
    unsigned char *buf2 = new unsigned char[4];
    char c_file[100];
    strcpy(c_file, fname.c_str());
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
int getFileSize(const string &filename) {
    FILE *fp;
    if ((fp = fopen(filename.c_str(), "r")) == nullptr)
        return 0;
    fseek(fp, 0, SEEK_END);
    return ftell(fp);
}

SpvData FileUtil::loadSPV(const string &filename)//加载文件夹下的SPIR-V 数据文件
{
    size_t size = (getFileSize(filename));//获取SPIR-V 数据文件的总字节数
    cout << "正在读取文件" << filename << "，大小为:" << size << '\n';
    SpvData spvData;//构建SpvData 结构体实例
    spvData.size = size;//设置SPIR-V 数据总字节数
    spvData.data = (uint32_t *) (malloc(size));//分配相应字节数的内存
    char *buf = (char *) spvData.data;//从文件中加载数据进入内存
    char c_file[1000];
    strcpy(c_file, filename.c_str());
    FILE *fpSPV;
    fpSPV = fopen(c_file, "rb");
    if (fpSPV == nullptr) {
        printf("打开文件%s失败\n", filename.c_str());
    }
    fread(buf, size, 1, fpSPV);
    return spvData;//返回SpvData 结构体实例
}
