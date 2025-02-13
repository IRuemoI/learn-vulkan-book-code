#pragma once
class LightManager {
public:
    //光源位置
    static float lx, ly, lz;
    //环境光强度RGBA
    static float lightAmbientR, lightAmbientG, lightAmbientB, lightAmbientA;
    //散射光强度RGBA
    static float lightDiffuseR, lightDiffuseG, lightDiffuseB, lightDiffuseA;
    //镜面光强度RGBA
    static float lightSpecularR, lightSpecularG, lightSpecularB, lightSpecularA;

    //设置光源的位置
    static void setLightPosition(float lxIn, float lyIn, float lzIn);
    //设置环境光强度RGBA
    static void setLightAmbient(float lightAmbientRIn, float lightAmbientGIn, float lightAmbientBIn, float lightAmbientAIn);
    //设置散射光强度RGBA
    static void setLightDiffuse(float lightDiffuseRIn, float lightDiffuseGIn, float lightDiffuseBIn, float lightDiffuseAIn);
    //设置镜面光强度RGBA
    static void setLightSpecular(float lightSpecularRIn, float lightSpecularGIn, float lightSpecularBIn, float lightSpecularAIn);
};
