#include "FileUtil.h"
#include <assert.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
string FileUtil::loadAssetStr(string filename) {
    ifstream infile;
    infile.open(filename.data());
    cout << "\nfilename>" << filename << endl;
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
ThreeDTexDataObject *FileUtil::load3DTexData(string filename)//加载 3D 纹理数据的方法
{
    unsigned char *buf = new unsigned char[4];//开辟长度为 4 字节的内存
    char c_file[100];
    strcpy(c_file, filename.c_str());
    FILE *fpPhoto;
    fpPhoto = fopen(c_file, "rb");
    if (fpPhoto == nullptr) {
        printf("打开文件失败\n");
    }
    fread(buf, 4, 1, fpPhoto);                                                      //读取纹理宽度数据字节
    int width = fromBytesToInt(buf);                                                //转换为 int 型数值
    fread(buf, 4, 1, fpPhoto);                                                      //读取纹理高度数据字节
    int height = fromBytesToInt(buf);                                               //转换为 int 型数值
    fread(buf, 4, 1, fpPhoto);                                                      //读取纹理深度数据字节
    int depth = fromBytesToInt(buf);                                                //转换为 int 型数值
    unsigned char *data = new unsigned char[width * height * depth * 4];            //开辟纹理数据存储内存
    fread(data, width * height * depth * 4, 1, fpPhoto);                            //读取纹理数据
    ThreeDTexDataObject *ctdo = new ThreeDTexDataObject(width, height, depth, data);//创建纹理数据对象
    return ctdo;                                                                    //返回结果
}
