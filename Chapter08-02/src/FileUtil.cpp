#include "FileUtil.h"
#include "TexDataObject.h"

// 加载Assets文件夹下的指定文本性质文件内容作为字符串返回
string FileUtil::loadAssetStr(string filename) {
    ifstream infile;
    infile.open(filename.data());// 将文件流对象与文件连接起来
    cout << "正在读取资产文件：" << filename << endl;
    assert(infile.is_open());// 若失败,则输出错误消息,并终止程序运行
    string shaderSource;
    string s;
    while (getline(infile, s)) {
        shaderSource += s;
        shaderSource += '\n';
    }
    infile.close();// 关闭文件输入流
    // cout << shaderSource << endl;
    return shaderSource;
}

int fromBytesToInt(unsigned char *buff) {        //将字节序列转换为int值的方法
    int k = 0;                                   //结果变量
    unsigned char *temp = (unsigned char *) (&k);//将结果变量所占内存以字节序列模式访问
    temp[0] = buff[0];                           //设置第1个字节的数据
    temp[1] = buff[1];                           //设置第2个字节的数据
    temp[2] = buff[2];                           //设置第3个字节的数据
    temp[3] = buff[3];                           //设置第4个字节的数据
    return k;                                    //返回结果值
}
TexDataObject *FileUtil::loadCommonTexData(const string &filename)//加载bntex纹理数据
{
    auto *buf = new unsigned char[4]; //开辟长度为4字节的内存
    auto *buf2 = new unsigned char[4];//开辟长度为4字节的内存
    char c_file[200];
    strcpy(c_file, filename.c_str());
    FILE *fpPhoto;
    fpPhoto = fopen(c_file, "rb");
    if (fpPhoto == nullptr) {
        printf("打开文件失败\n");
    }
    fread(buf, 4, 1, fpPhoto);                                              //读取纹理宽度数据字节
    int width = fromBytesToInt(buf);                                        //转换为int型数值
    fread(buf2, 4, 1, fpPhoto);                                             //读取纹理高度数据字节
    int height = fromBytesToInt(buf2);                                      //转换为int型数值
    auto *data = new unsigned char[width * height * 4];                     //开辟纹理数据存储内存
    fread(data, width * height * 4, 1, fpPhoto);                            //读取纹理数据
    auto *ctdo = new TexDataObject(width, height, data, width * height * 4);//创建纹理数据对象
    return ctdo;                                                            //返回结果
}