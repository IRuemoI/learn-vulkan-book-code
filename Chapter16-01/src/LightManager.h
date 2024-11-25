#pragma once
class LightManager {
public:
    static float lx, ly, lz;
    static float lightAmbientR, lightAmbientG, lightAmbientB, lightAmbientA;
    static float lightDiffuseR, lightDiffuseG, lightDiffuseB, lightDiffuseA;
    static float lightSpecularR, lightSpecularG, lightSpecularB, lightSpecularA;
    static void setLightPosition(float lxIn, float lyIn, float lzIn);
    static void setLightAmbient(float lightAmbientRIn, float lightAmbientGIn, float lightAmbientBIn, float lightAmbientAIn);
    static void setLightDiffuse(float lightDiffuseRIn, float lightDiffuseGIn, float lightDiffuseBIn, float lightDiffuseAIn);
    static void setLightSpecular(float lightSpecularRIn, float lightSpecularGIn, float lightSpecularBIn, float lightSpecularAIn);
    static float lightFWJ;
    static void move();
};
