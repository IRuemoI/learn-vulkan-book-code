#pragma once
class LightManager {
public:
    static float lx, ly, lz;
    static float lightAmbientR, lightAmbientG, lightAmbientB, lightAmbientA;
    static float lightDiffuseR, lightDiffuseG, lightDiffuseB, lightDiffuseA;
    static float lightSpecularR, lightSpecularG, lightSpecularB, lightSpecularA;
    static float lcm[16];
    static float lpm[16];
    static float lcpm[16];
    static float upxYS, upyYS, upzYS;
    static float upDegree;
    static void setLightPosition(float x, float y, float z);
    static void setLightAmbient(float lightAmbientRIn, float lightAmbientGIn, float lightAmbientBIn, float lightAmbientAIn);
    static void setLightDiffuse(float lightDiffuseRIn, float lightDiffuseGIn, float lightDiffuseBIn, float lightDiffuseAIn);
    static void setLightSpecular(float lightSpecularRIn, float lightSpecularGIn, float lightSpecularBIn, float lightSpecularAIn);
    static void getTYJZ();
};