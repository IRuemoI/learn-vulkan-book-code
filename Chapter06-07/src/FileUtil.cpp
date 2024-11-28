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

int fromBytesToInt(unsigned char *buff) {//将字节序列转换为int值的方法
    int k = 0;//结果变量
    unsigned char *temp = (unsigned char *) (&k);//将结果变量所占内存以字节序列模式访问
    temp[0] = buff[0];//设置第1个字节的数据
    temp[1] = buff[1];//设置第2个字节的数据
    temp[2] = buff[2];//设置第3个字节的数据
    temp[3] = buff[3];//设置第4个字节的数据
    return k;//返回结果值
}
TexDataObject *FileUtil::loadCommonTexData(string filename)//加载bntex纹理数据
{
    unsigned char *buf = new unsigned char[4];//开辟长度为4字节的内存
    unsigned char *buf2 = new unsigned char[4];//开辟长度为4字节的内存
    char c_file[100];
    strcpy(c_file, filename.c_str());
    FILE *fpPhoto;
    fpPhoto = fopen(c_file, "rb");
    if (fpPhoto == nullptr) {
        printf("打开文件失败\n");
    }
    fread(buf, 4, 1, fpPhoto);//读取纹理宽度数据字节
    int width = fromBytesToInt(buf);//转换为int型数值
    fread(buf2, 4, 1, fpPhoto);//读取纹理高度数据字节
    int height = fromBytesToInt(buf2);//转换为int型数值
    unsigned char *data = new unsigned char[width * height * 4];//开辟纹理数据存储内存
    fread(data, width * height * 4, 1, fpPhoto);//读取纹理数据
    TexDataObject *ctdo = new TexDataObject(width, height, data, width * height * 4);//创建纹理数据对象
    return ctdo;//返回结果
}

int mymax(int a, int b)//比较大小的方法
{
    return (a > b) ? a : b;
}
int textureLevelSizeS3tcDxt5(int width, int height)//根据 dxt5 纹理的宽度和高度计算纹理数据字节数的函数
{
    return ((width + 3) >> 2) * ((height + 3) >> 2) * 16;
}
TexDataObject *FileUtil::load_DXT5_BC3_TexData(const string &filename)//加载 BC3 压缩纹理数据
{
#define DDS_HEADER_LENGTH 31//dds 文件头长度
#define DDS_HEADER_HEIGHT 3//纹理宽度偏移量
#define DDS_HEADER_WIDTH 4//纹理高度偏移量
#define DDS_HEADER_SIZE 1//文件头长度偏移量
#define DDSD_MIPMAPCOUNT 0x20000//MIPMAP 纹理数量标志掩码
#define DDS_HEADER_MIPMAPCOUNT 7//MIPMAP 纹理数量偏移量
#define DDS_HEADER_FLAGS 2//dds 文件头标记偏移量
    printf("文件名称：%s\n", filename.c_str());//打印压缩纹理文件名称
    char c_file[100];//存储文件路径用
    strcpy(c_file, filename.c_str());//将文件路径转换成字符数组
    FILE *fpPhoto;//文件指针
    fpPhoto = fopen(c_file, "rb");//将文件以二进制模式打开
    if (fpPhoto == nullptr) {//判断文件是否打开成功
        printf("打开文件失败\n");
    }
    fseek(fpPhoto, 0, SEEK_END);//跳转到文件尾
    int fileBytesCount = ftell(fpPhoto);//获取文件长度（以字节计）
    printf("文件大小(字节)： %d", fileBytesCount);//打印文件总字节数
    fseek(fpPhoto, 0, 0);//回到文件头
    unsigned char *dataTemp = new unsigned char[fileBytesCount];//指向文件数据存储内存首地址的指针
    fread(dataTemp, fileBytesCount, 1, fpPhoto);//获取 dds 文件中的数据
    int *headerI = (int *) dataTemp;//以整数数组的视角看文件数据，为文件头服务
    int width = headerI[DDS_HEADER_WIDTH];//获取纹理宽度
    printf("width=%d\n", width);//打印纹理宽度
    int height = headerI[DDS_HEADER_HEIGHT];//获取纹理高度
    printf("height=%d\n", height);//打印纹理高度
    int levels = 1;//声明纹理层次辅助变量
    if (headerI[DDS_HEADER_FLAGS] & DDSD_MIPMAPCOUNT)//计算 MipMap 纹理层次数量
    {
        levels = mymax(1, headerI[DDS_HEADER_MIPMAPCOUNT]);
    }
    printf("levels=%d %d\n", levels, headerI[DDS_HEADER_MIPMAPCOUNT]);//打印纹理层次数量
    int dataOffset = headerI[DDS_HEADER_SIZE] + 4;//纹理数据的起始偏移量
    unsigned char *dxtData = dataTemp + dataOffset;//计算纹理数据首地址指针
    TexDataObject *result = nullptr;//声明纹理数据对象指针
    int offset = 0;//每层纹理的数据字节偏移量
    for (int i = 0; i < levels; ++i)//对每个 mipmap 纹理层进行循环
    {
        int levelSize = textureLevelSizeS3tcDxt5(width, height);//计算本层纹理的数据字节数
        printf("levelSize %d offset %d\n", levelSize, offset);//打印字节数
        unsigned char *dataLevel = new unsigned char[levelSize];//本层纹理的数据存储
        memcpy(dataLevel, dxtData, levelSize);//拷贝对应纹理数据进本层存储
        result = new TexDataObject(width, height, dataLevel, levelSize);//创建纹理数据对象
        width = width >> 1;//计算下一层纹理的宽度
        height = height >> 1;//计算下一层纹理的高度
        offset += levelSize;//计算新一层纹理的数据字节偏移量
        break;//中断遍历
    }
    fclose(fpPhoto);//关闭文件
    delete dataTemp;//删除纹理数据
    return result;//返回结果
}
