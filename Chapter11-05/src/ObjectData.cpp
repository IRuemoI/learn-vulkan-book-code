#include "ObjectData.h"
#include <vector>
float *ObjectData::vdata;
int ObjectData::dataByteCount;
int ObjectData::vCount;
void ObjectData::genVertexData() {
    std::vector<float> alVertix;
    std::vector<float> alTexCoor;
    float Y_MAX = 1.5f;                //Y的最大值
    float Y_MIN = -1.5f;               //Y的最小值
    int FD = 6;                        //一面中矩形个数
    float hw = 0.575f;                 //矩形宽度的一半
    float yStart = Y_MIN;              //起始Y坐标
    float ySpan = (Y_MAX - Y_MIN) / FD;//Y坐标的步进
    for (int i = 0; i < FD; i++) {
        float x1 = -hw;
        float y1 = yStart;
        float z1 = hw;

        float x2 = hw;
        float y2 = yStart;
        float z2 = hw;

        float x3 = hw;
        float y3 = yStart;
        float z3 = -hw;

        float x4 = -hw;
        float y4 = yStart;
        float z4 = -hw;

        float x5 = -hw;
        float y5 = yStart + ySpan;
        float z5 = hw;

        float x6 = hw;
        float y6 = yStart + ySpan;
        float z6 = hw;

        float x7 = hw;
        float y7 = yStart + ySpan;
        float z7 = -hw;

        float x8 = -hw;
        float y8 = yStart + ySpan;
        float z8 = -hw;

        alVertix.push_back(x5);
        alVertix.push_back(y5);
        alVertix.push_back(z5);
        alVertix.push_back(x1);
        alVertix.push_back(y1);
        alVertix.push_back(z1);
        alVertix.push_back(x2);
        alVertix.push_back(y2);
        alVertix.push_back(z2);

        alVertix.push_back(x5);
        alVertix.push_back(y5);
        alVertix.push_back(z5);
        alVertix.push_back(x2);
        alVertix.push_back(y2);
        alVertix.push_back(z2);
        alVertix.push_back(x6);
        alVertix.push_back(y6);
        alVertix.push_back(z6);

        alVertix.push_back(x6);
        alVertix.push_back(y6);
        alVertix.push_back(z6);
        alVertix.push_back(x2);
        alVertix.push_back(y2);
        alVertix.push_back(z2);
        alVertix.push_back(x3);
        alVertix.push_back(y3);
        alVertix.push_back(z3);

        alVertix.push_back(x6);
        alVertix.push_back(y6);
        alVertix.push_back(z6);
        alVertix.push_back(x3);
        alVertix.push_back(y3);
        alVertix.push_back(z3);
        alVertix.push_back(x7);
        alVertix.push_back(y7);
        alVertix.push_back(z7);

        alVertix.push_back(x7);
        alVertix.push_back(y7);
        alVertix.push_back(z7);
        alVertix.push_back(x3);
        alVertix.push_back(y3);
        alVertix.push_back(z3);
        alVertix.push_back(x4);
        alVertix.push_back(y4);
        alVertix.push_back(z4);

        alVertix.push_back(x7);
        alVertix.push_back(y7);
        alVertix.push_back(z7);
        alVertix.push_back(x4);
        alVertix.push_back(y4);
        alVertix.push_back(z4);
        alVertix.push_back(x8);
        alVertix.push_back(y8);
        alVertix.push_back(z8);

        alVertix.push_back(x8);
        alVertix.push_back(y8);
        alVertix.push_back(z8);
        alVertix.push_back(x4);
        alVertix.push_back(y4);
        alVertix.push_back(z4);
        alVertix.push_back(x1);
        alVertix.push_back(y1);
        alVertix.push_back(z1);

        alVertix.push_back(x8);
        alVertix.push_back(y8);
        alVertix.push_back(z8);
        alVertix.push_back(x1);
        alVertix.push_back(y1);
        alVertix.push_back(z1);
        alVertix.push_back(x5);
        alVertix.push_back(y5);
        alVertix.push_back(z5);

        yStart = yStart + ySpan;

        alTexCoor.push_back(0);
        alTexCoor.push_back(0);
        alTexCoor.push_back(0);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(0);
        alTexCoor.push_back(0);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(0);

        alTexCoor.push_back(0);
        alTexCoor.push_back(0);
        alTexCoor.push_back(0);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(0);
        alTexCoor.push_back(0);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(0);

        alTexCoor.push_back(0);
        alTexCoor.push_back(0);
        alTexCoor.push_back(0);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(0);
        alTexCoor.push_back(0);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(0);

        alTexCoor.push_back(0);
        alTexCoor.push_back(0);
        alTexCoor.push_back(0);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(0);
        alTexCoor.push_back(0);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(1);
        alTexCoor.push_back(0);
    }
    vCount = FD * 4 * 6;                       //计算顶点数量
    dataByteCount = vCount * 5 * sizeof(float);//计算数据所占字节数
    vdata = new float[vCount * 5];             //创建存放数据的数组
    int index = 0;                             //辅助索引
    for (int i = 0; i < vCount; i++) {         //将顶点数据和纹理坐标数据存储到数组中
        vdata[index++] = alVertix[i * 3 + 0];
        vdata[index++] = alVertix[i * 3 + 1];
        vdata[index++] = alVertix[i * 3 + 2];
        vdata[index++] = alTexCoor[i * 2 + 0];
        vdata[index++] = alTexCoor[i * 2 + 1];
    }
}
