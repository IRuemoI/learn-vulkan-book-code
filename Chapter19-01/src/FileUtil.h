#pragma once
#include <string>
#include "TexDataObject.h"

using namespace std;//指定使用的命名空间
class FileUtil
{
public:
	static string loadAssetStr(string filename);//加载Assets文件夹下的指定文本性质文件内容作为字符串返回
	static TexDataObject* loadCommonTexData(string filename);//加载非压缩RGBA四通道普通纹理数据
	static TexDataObject* load_DXT5_BC3_TexData(string filename);//加载DDS封装格式的BC3(DXT5)格式纹理数据
};
