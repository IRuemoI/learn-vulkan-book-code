#pragma once
#define CAMERA_R 150

class CameraUtil {
public:
    static float tx;//摄像机目标方向（direction）
    static float ty;
    static float tz;
    static float cx;//摄像机位置（position）
    static float cy;
    static float cz;
    static float degree;//摄像机朝向角度
    static float yj;//摄像机仰角
    static float camera9Para[9];//摄像机9参数
            //计算摄像机新参数的方法
    static void calCamera(float yjSpan, float cxSpan);
    //摄像机平移
    static void cameraGo(float goBack, float leftRight);
    //将当前的摄像机9参数值更新到矩阵系统
    static void flushCameraToMatrix();

private:
    static void calCamera();
};