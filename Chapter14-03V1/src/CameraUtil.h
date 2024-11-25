#pragma once
#define CAMERA_R 300
class CameraUtil {
public:
    static float degree;
    static float yj;
    static float camera9Para[9];
    static void calCamera();
    static void flushCameraToMatrix();
};