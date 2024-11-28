#include "LightManager.h"
#include "HelpFunction.h"
#include "Matrix.h"
#include "MatrixState3D.h"
#include "stdio.h"
#include <math.h>

#define LR 1;

//光源位置
float LightManager::lx = 0;
float LightManager::ly = 0;
float LightManager::lz = 0;
//光源摄像机原始UP向量
float LightManager::upxYS = 0;
float LightManager::upyYS = 0;
float LightManager::upzYS = 1;
//光源摄像机UP向量旋转角
float LightManager::upDegree = 0;
//环境光强度RGBA
float LightManager::lightAmbientR = 0;
float LightManager::lightAmbientG = 0;
float LightManager::lightAmbientB = 0;
float LightManager::lightAmbientA = 0;
//散射光强度RGBA
float LightManager::lightDiffuseR = 0;
float LightManager::lightDiffuseG = 0;
float LightManager::lightDiffuseB = 0;
float LightManager::lightDiffuseA = 0;
//镜面光强度RGBA
float LightManager::lightSpecularR = 0;
float LightManager::lightSpecularG = 0;
float LightManager::lightSpecularB = 0;
float LightManager::lightSpecularA = 0;

//摄像机矩阵
float LightManager::lcm[16];
//投影矩阵
float LightManager::lpm[16];
//摄像机投影矩阵
float LightManager::lcpm[16];


//设置环境光强度RGBA
void LightManager::setLightAmbient(float lightAmbientRIn, float lightAmbientGIn, float lightAmbientBIn, float lightAmbientAIn) {
    lightAmbientR = lightAmbientRIn;
    lightAmbientG = lightAmbientGIn;
    lightAmbientB = lightAmbientBIn;
    lightAmbientA = lightAmbientAIn;
}
//设置散射光强度RGBA
void LightManager::setLightDiffuse(float lightDiffuseRIn, float lightDiffuseGIn, float lightDiffuseBIn, float lightDiffuseAIn) {
    lightDiffuseR = lightDiffuseRIn;
    lightDiffuseG = lightDiffuseGIn;
    lightDiffuseB = lightDiffuseBIn;
    lightDiffuseA = lightDiffuseAIn;
}
//设置镜面光强度RGBA
void LightManager::setLightSpecular(float lightSpecularRIn, float lightSpecularGIn, float lightSpecularBIn, float lightSpecularAIn) {
    lightSpecularR = lightSpecularRIn;
    lightSpecularG = lightSpecularGIn;
    lightSpecularB = lightSpecularBIn;
    lightSpecularA = lightSpecularAIn;
}

//设置光源的位置
void LightManager::setLightPosition(float x, float y, float z) {
    lx = x;
    ly = y;
    lz = z;
}

float *normalize(float x, float y, float z) {
    float *result = new float[3];

    float length = (float) (sqrt(x * x + y * y + z * z));
    result[0] = x / length;
    result[1] = y / length;
    result[2] = z / length;
    return result;
}

void LightManager::getTYJZ() {//获取投影与观察组合矩阵的方法
    upDegree = upDegree + 0.25;//改变UP 向量绕视线轴的旋转角度
    if (upDegree >= 360) { upDegree = 0; }//角度增加到360 度则归0
    const float cxV = 5, cyV = 0, czV = 5;//虚拟摄像机的目标点
    Matrix::frustumM(lpm, 0, -1.0f, 1.0f, -1.0f, 1.0f, 4.0f, 400);//生成透视投影矩阵
    float *zhou = normalize(lx - cxV, ly - cyV, lz - czV);//规格化旋转轴(视线向量)
    float *mm = new float[16]();//创建用于存储up 向量绕视线轴旋转的变换矩阵的数组
    Matrix::setRotateM(mm, 0, upDegree, zhou[0], zhou[1], zhou[2]);//生成旋转矩阵
    float *upvBefore = new float[4]{upxYS, upyYS, upzYS, 1.0};//旋转前的初始虚拟摄像机UP 向量
    float *upvAftere = new float[4]();//旋转后的虚拟摄像机UP 向量
    Matrix::multiplyMV(upvAftere, 0, mm, 0, upvBefore, 0);//对虚拟摄像机UP 向量进行旋转变换
    Matrix::setLookAtM(//生成虚拟摄像机观察矩阵
            lcm, 0, lx, ly, lz, cxV, cyV, czV, upvAftere[0], upvAftere[1], upvAftere[2]);
    delete[] upvAftere;
    delete[] upvBefore;
    delete[] mm;
    delete[] zhou;//释放辅助数组内存
    Matrix::multiplyMM(lcpm, 0, lpm, 0, lcm, 0);//将投影矩阵与摄像机观察矩阵相乘
    Matrix::multiplyMM(lcpm, 0, MatrixState3D::vulkanClipMatrix, 0, lcpm, 0);//乘以标准设备空间调整矩阵
}
