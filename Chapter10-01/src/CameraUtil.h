#pragma once
#define CAMERA_R 30
class CameraUtil {
public:
    static float degree;
    static float yj;
    static float camera9Para[9];
    static void calCamera(float yjSpan, float cxSpan);
    static void flushCameraToMatrix();
};