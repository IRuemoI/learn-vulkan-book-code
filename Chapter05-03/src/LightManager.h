#pragma once

class LightManager {
public:
    static float lx, ly, lz;//光源位置
    static float lightDiffuseR, lightDiffuseG, lightDiffuseB, lightDiffuseA;//散射光强度RGBA分量
    static void setLightPosition(float lxIn, float lyIn, float lzIn);//设置光源位置的方法
    static void setLightDiffuse(float lightDiffuseRIn, float lightDiffuseGIn, float lightDiffuseBIn, float lightDiffuseAIn);//设置散射光强度RGBA分量
};