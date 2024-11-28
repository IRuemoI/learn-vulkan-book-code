#include "FileUtil.h"
#include <cstring>

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

TexDataObject *FileUtil::loadCubemapTexData(string filename) {

    unsigned char *buf = new unsigned char[4];
    char c_file[1000];//文件路径
    strcpy(c_file, filename.c_str());//将文件路径转换成字符数组
    FILE *fpPhoto;
    fpPhoto = fopen(c_file, "rb");//将文件以二进制形式打开
    if (fpPhoto == nullptr) {
        printf("打开文件3失败\n");
    }
    fread(buf, 4, 1, fpPhoto);//纹理宽度,读取4个字节存入buf
    int width = fromBytesToInt(buf);//将buf转成一个int宽度

    fread(buf, 4, 1, fpPhoto);//纹理高度读取4个字节存入buf
    int height = fromBytesToInt(buf);//将buf转成一个int高度

    fread(buf, 4, 1, fpPhoto);//纹理高度读取4个字节存入buf
    int count = fromBytesToInt(buf);//读取纹理元素数量
    assert(count == 6);

    unsigned char *data = new unsigned char[6 * width * height * 4];
    fread(data, 6 * width * height * 4, 1, fpPhoto);

    //创建纹理数据对象
    TexDataObject *ctdo = new TexDataObject(width, height, data, 6 * width * height * 4);
    //返回结果
    return ctdo;
}