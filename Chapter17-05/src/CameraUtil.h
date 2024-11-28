#pragma once
#define CAMERA_R 30

class CameraUtil {
public:
    //摄像机目标点位置
    static float tx;
    static float ty;
    static float tz;

    static float degree;//摄像机朝向角度
    static float yj;//摄像机仰角
    static float camera9Para[9];//摄像机9参数
            //计算摄像机新参数的方法
    static void calCamera(float yjSpan, float cxSpan);
    //将当前的摄像机9参数值更新到矩阵系统
    static void flushCameraToMatrix();

private:
    static void calCamera();
};
