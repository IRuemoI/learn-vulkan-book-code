#pragma once
class LightManager {
public:
    static float lightAmbientR, lightAmbientG, lightAmbientB, lightAmbientA;                                                //环境光强度RGBA分量
    static void setlightAmbient(float lightAmbientRIn, float lightAmbientGIn, float lightAmbientBIn, float lightAmbientAIn);//设置环境光强度的方法
};