#pragma once
#define LIGNT_CAMERA_NEAR 2.0
#define LIGNT_CAMERA_FAR 1000.0
class LightManager {
public:
    static float fwj;
    static float lx, ly, lz;
    static float upxYS, upyYS, upzYS;
    static float lightAmbientR, lightAmbientG, lightAmbientB, lightAmbientA;
    static float lightDiffuseR, lightDiffuseG, lightDiffuseB, lightDiffuseA;
    static float lightSpecularR, lightSpecularG, lightSpecularB, lightSpecularA;
    static float lcm[16];
    static float lpm[16];
    static float lcpm[16];
    static void setLightPosition(float fwj, float yj);
    static void setLightAmbient(float lightAmbientRIn, float lightAmbientGIn, float lightAmbientBIn, float lightAmbientAIn);
    static void setLightDiffuse(float lightDiffuseRIn, float lightDiffuseGIn, float lightDiffuseBIn, float lightDiffuseAIn);
    static void setLightSpecular(float lightSpecularRIn, float lightSpecularGIn, float lightSpecularBIn, float lightSpecularAIn);
    static void getTYJZ(float ratio);
};