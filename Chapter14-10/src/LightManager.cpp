#include "LightManager.h"
#include <cmath>
Light LightManager::lightArray[LIGHTS_COUNT];
float LightManager::lightFWJ = 0;
void LightManager::setLightRange(int id, float lrIn) {
    lightArray[id].lr = lrIn;
}
void LightManager::setLightPosition(int id, float lxIn, float lyIn, float lzIn) {
    lightArray[id].lx = lxIn;
    lightArray[id].ly = lyIn;
    lightArray[id].lz = lzIn;
}
void LightManager::setLightAmbient(int id, float lightAmbientRIn, float lightAmbientGIn, float lightAmbientBIn, float lightAmbientAIn) {
    lightArray[id].lightAmbientR = lightAmbientRIn;
    lightArray[id].lightAmbientG = lightAmbientGIn;
    lightArray[id].lightAmbientB = lightAmbientBIn;
    lightArray[id].lightAmbientA = lightAmbientAIn;
}
void LightManager::setLightDiffuse(int id, float lightDiffuseRIn, float lightDiffuseGIn, float lightDiffuseBIn, float lightDiffuseAIn) {
    lightArray[id].lightDiffuseR = lightDiffuseRIn;
    lightArray[id].lightDiffuseG = lightDiffuseGIn;
    lightArray[id].lightDiffuseB = lightDiffuseBIn;
    lightArray[id].lightDiffuseA = lightDiffuseAIn;
}
void LightManager::setLightSpecular(int id, float lightSpecularRIn, float lightSpecularGIn, float lightSpecularBIn, float lightSpecularAIn) {
    lightArray[id].lightSpecularR = lightSpecularRIn;
    lightArray[id].lightSpecularG = lightSpecularGIn;
    lightArray[id].lightSpecularB = lightSpecularBIn;
    lightArray[id].lightSpecularA = lightSpecularAIn;
}
void LightManager::move() {
    lightFWJ = lightFWJ + 0.5f;
    for (int i = 0; i < LIGHTS_COUNT; i++) {
        float tempLx = (float) sin((lightFWJ + 90 * i) / 180 * 3.14150265f) * 300 + i * 50;
        float tempLz = (float) cos((lightFWJ + 90 * i) / 180 * 3.14150265f) * 300 + i * 50;
        LightManager::setLightPosition(i, tempLx, 300, tempLz);
    }
    if (lightFWJ >= 360) {
        lightFWJ = 0;
    }
}