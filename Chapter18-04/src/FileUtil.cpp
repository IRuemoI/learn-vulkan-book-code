#include "FileUtil.h"
#include "TexDataObject.h"
#include <assert.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
typedef struct /**** BMP file header structure ****/
{
    unsigned int bfSize;        /* Size of file */
    unsigned short bfReserved1; /* Reserved */
    unsigned short bfReserved2; /* ... */
    unsigned int bfOffBits;     /* Offset to bitmap data */
} MyBITMAPFILEHEADER;
typedef struct /**** BMP file info structure ****/
{
    unsigned int biSize;         /* Size of info header */
    int biWidth;                 /* Width of image */
    int biHeight;                /* Height of image */
    unsigned short biPlanes;     /* Number of color planes */
    unsigned short biBitCount;   /* Number of bits per pixel */
    unsigned int biCompression;  /* Type of compression to use */
    unsigned int biSizeImage;    /* Size of image data */
    int biXPelsPerMeter;         /* X pixels per meter */
    int biYPelsPerMeter;         /* Y pixels per meter */
    unsigned int biClrUsed;      /* Number of colors used */
    unsigned int biClrImportant; /* Number of important colors */
} MyBITMAPINFOHEADER;
void FileUtil::storeData(string filename, const char *buf, int num, int width, int height) {
    int pixelCount = width * height;
    char *bufRGB = new char[pixelCount * 3];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int dstIndex = i * width + j;
            int srcIndex = (height - 1 - i) * width + j;
            bufRGB[dstIndex * 3 + 0] = buf[srcIndex * 4 + 0];
            bufRGB[dstIndex * 3 + 1] = buf[srcIndex * 4 + 1];
            bufRGB[dstIndex * 3 + 2] = buf[srcIndex * 4 + 2];
        }
    }
    MyBITMAPFILEHEADER bfh;
    MyBITMAPINFOHEADER bih;
    unsigned short bfType = 0x4d42;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfSize = 2 + sizeof(MyBITMAPFILEHEADER) + sizeof(MyBITMAPINFOHEADER) + width * height * 3;
    bfh.bfOffBits = 0x36;
    bih.biSize = sizeof(MyBITMAPINFOHEADER);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biSizeImage = 0;
    bih.biXPelsPerMeter = 5000;
    bih.biYPelsPerMeter = 5000;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;
    ofstream ofile;
    ofile.open(filename, ios::binary);
    ofile.write((const char *) (&bfType), sizeof(bfType));
    ofile.write((const char *) (&bfh), sizeof(bfh));
    ofile.write((const char *) (&bih), sizeof(bih));
    ofile.write(bufRGB, width * height * 3);
    ofile.close();
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
