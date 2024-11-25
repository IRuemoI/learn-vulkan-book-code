#pragma once
#define CAMERA_R 20
class CameraUtil {
public:
    static float tx;
    static float ty;
    static float tz;
    static float cx;
    static float cy;
    static float cz;
    static float degree;
    static float yj;
    static float camera9Para[9];
    static void calCamera(float yjSpan, float cxSpan);
    static void cameraGo(float goBack, float leftRight);
    static void flushCameraToMatrix();

private:
    static void calCamera();
};