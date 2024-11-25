#include "FileUtil.h"
#include <assert.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
//加载Assets文件夹下的指定文本性质文件内容作为字符串返回
string FileUtil::loadAssetStr(string filename) {
    ifstream infile;
    infile.open(filename.data());//将文件流对象与文件连接起来
    cout << "filename>" << filename << endl;
    assert(infile.is_open());//若失败,则输出错误消息,并终止程序运行
    string ss;
    string s;
    while (getline(infile, s)) {
        ss += s;
        ss += '\n';
    }
    infile.close();//关闭文件输入流
    return ss;
}

//4个bit类型转成一个int类型
int fromBytesToInt(unsigned char *buff) {
    int k = 0;
    unsigned char *temp = (unsigned char *) (&k);
    temp[0] = buff[0];
    temp[1] = buff[1];
    temp[2] = buff[2];
    temp[3] = buff[3];
    return k;
}

//加载非压缩RGBA四通道普通纹理数据
TexDataObject *FileUtil::loadCommonTexData(string filename) {
    printf(filename.c_str());

    unsigned char *buf = new unsigned char[4];//存放图片宽度
    unsigned char *buf2 = new unsigned char[4];//存放图片高度
    char c_file[100];//文件路径
    strcpy(c_file, filename.c_str());//将文件路径转换成字符数组
    FILE *fpPhoto;
    fpPhoto = fopen(c_file, "rb");//将文件以二进制形式打开
    if (fpPhoto == nullptr) {
        printf("打开文件失败\n");
    }
    fread(buf, 4, 1, fpPhoto);//读取4个字节存入buf
    int width = fromBytesToInt(buf);//将buf转成一个int宽度

    fread(buf2, 4, 1, fpPhoto);//读取4个字节存入buf2
    int height = fromBytesToInt(buf2);//将buf转成一个int高度

    unsigned char *data = new unsigned char[width * height * 4];
    fread(data, width * height * 4, 1, fpPhoto);
    //创建纹理数据对象
    TexDataObject *ctdo = new TexDataObject(width, height, data, width * height * 4, COMMON);
    //返回结果
    return ctdo;
}


int mymax(int a, int b) {
    return (a > b) ? a : b;
}

//根据dxt5纹理的宽度和高度计算纹理数据字节数的函数
int textureLevelSizeS3tcDxt5(int width, int height) {
    return ((width + 3) >> 2) * ((height + 3) >> 2) * 16;
}

TexDataObject *FileUtil::load_DXT5_BC3_TexData(string filename) {
    //dds文件头长度
#define DDS_HEADER_LENGTH 31
    //纹理宽度偏移量
#define DDS_HEADER_HEIGHT 3
    //纹理高度偏移量
#define DDS_HEADER_WIDTH 4
    //文件头长度偏移量
#define DDS_HEADER_SIZE 1
    //MIPMAP纹理数量标志掩码
#define DDSD_MIPMAPCOUNT 0x20000
    //MIPMAP纹理数量偏移量
#define DDS_HEADER_MIPMAPCOUNT 7
    //dds文件头标记偏移量
#define DDS_HEADER_FLAGS 2

    printf("%s\n", filename.c_str());
    char c_file[200];//文件路径
    strcpy(c_file, filename.c_str());//将文件路径转换成字符数组
    FILE *fpPhoto;
    fpPhoto = fopen(c_file, "rb");//将文件以二进制形式打开
    if (fpPhoto == nullptr) {
        printf("打开文件失败\n");
    }
    fseek(fpPhoto, 0, SEEK_END);//定位到文件末
    int fileBytesCount = ftell(fpPhoto);//文件长度
    printf("fileBytesCount %d\n", fileBytesCount);
    fseek(fpPhoto, 0, 0);//回到文件头

    //获取dds文件的数据
    unsigned char *dataTemp = new unsigned char[fileBytesCount];
    fread(dataTemp, fileBytesCount, 1, fpPhoto);
    //以整数数组的视角看文件数据，为文件头服务
    int *headerI = (int *) dataTemp;
    //获取纹理宽度
    int width = headerI[DDS_HEADER_WIDTH];
    printf("width=%d\n", width);
    //获取纹理高度
    int height = headerI[DDS_HEADER_HEIGHT];
    printf("height=%d\n", height);
    //声明纹理层次辅助变量
    int levels = 1;
    //计算出实际的MipMap纹理层次数量
    if (headerI[DDS_HEADER_FLAGS] & DDSD_MIPMAPCOUNT) {
        levels = mymax(1, headerI[DDS_HEADER_MIPMAPCOUNT]);
    }
    printf("levels=%d %d\n", levels, headerI[DDS_HEADER_MIPMAPCOUNT]);

    //纹理数据的起始偏移量
    int dataOffset = headerI[DDS_HEADER_SIZE] + 4;

    //获取纹理数据
    unsigned char *dxtData = dataTemp + dataOffset;

    TexDataObject *result = nullptr;

    //声明每层纹理的数据字节偏移量
    int offset = 0;
    //对每个mipmap纹理层进行循环
    for (int i = 0; i < levels; ++i) {
        //计算本层纹理的数据字节数
        int levelSize = textureLevelSizeS3tcDxt5(width, height);

        printf("levelSize %d offset %d\n", levelSize, offset);

        //获取本层纹理的数据字节序列
        unsigned char *dataLevel = new unsigned char[levelSize];
        memcpy(dataLevel, dxtData, levelSize);

        result = new TexDataObject(width, height, dataLevel, levelSize, RGBA8_BC3_DXT5);

        //计算下一层纹理的宽度
        width = width >> 1;
        //计算下一层纹理的高度
        height = height >> 1;
        //计算新一层纹理的数据字节偏移量
        offset += levelSize;

        break;
    }
    fclose(fpPhoto);

    delete dataTemp;
    return result;
}
