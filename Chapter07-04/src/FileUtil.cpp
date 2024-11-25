#include "FileUtil.h"
#include "TexDataObject.h"
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
//加载非压缩RGBA四通道普通纹理数据
TexDataObject *FileUtil::loadCommonTexData(string filename) {
    unsigned char *buf = new unsigned char[4]; //存放图片宽度
    unsigned char *buf2 = new unsigned char[4];//存放图片高度
    char c_file[100];                          //文件路径
    strcpy(c_file, filename.c_str());             //将文件路径转换成字符数组
    FILE *fpPhoto;
    fpPhoto = fopen(c_file, "rb");//将文件以二进制形式打开
    if (fpPhoto == nullptr) {
        printf("打开文件失败\n");
    }
    fread(buf, 4, 1, fpPhoto);        //读取4个字节存入buf
    int width = fromBytesToInt(buf);  //将buf转成一个int宽度
    fread(buf2, 4, 1, fpPhoto);       //读取4个字节存入buf2
    int height = fromBytesToInt(buf2);//将buf转成一个int高度
    unsigned char *data = new unsigned char[width * height * 4];
    fread(data, width * height * 4, 1, fpPhoto);
    //创建纹理数据对象
    TexDataObject *ctdo = new TexDataObject(width, height, data, width * height * 4);
    //返回结果
    return ctdo;
}
