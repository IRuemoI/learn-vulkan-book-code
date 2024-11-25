#include "FileUtil.h"
#include <cstdio>
#include <cstring>

int getFileSize(const string &filename) {
    FILE *fp;
    if ((fp = fopen(filename.c_str(), "r")) == nullptr)
        return 0;
    fseek(fp, 0, SEEK_END);
    return ftell(fp);
}

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
    return shaderSource;
}

SpvData FileUtil::loadSPV(const string &filename)//加载文件夹下的SPIR-V 数据文件
{
    size_t size = (getFileSize(filename));//获取SPIR-V 数据文件的总字节数
    cout << "正在读取文件" << filename << "，大小为:" << size << '\n';
    SpvData spvData;                           //构建SpvData 结构体实例
    spvData.size = size;                       //设置SPIR-V 数据总字节数
    spvData.data = (uint32_t *) (malloc(size));//分配相应字节数的内存
    char *buf = (char *) spvData.data;         //从文件中加载数据进入内存
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