#pragma once
class SkyData {
public:
    static float *vdata;                 //顶点数据数组首地址指针
    static int dataByteCount;            //顶点数据所占总字节数
    static int vCount;                   //顶点数量
    static void genSkyData(int vCountIn);//生成星星顶点数据的方法
};
