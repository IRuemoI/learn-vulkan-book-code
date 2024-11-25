#include "CameraUtil.h"
#include "HelpFunction.h"
#include "MatrixState3D.h"
#include <iostream>
#include <math.h>
float CameraUtil::degree = 0;
float CameraUtil::yj = 0;
float CameraUtil::camera9Para[9];
float CameraUtil::tx = 0;
float CameraUtil::ty = 0;
float CameraUtil::tz = -20;
float CameraUtil::cx = 0;
float CameraUtil::cy = 4.0f;
float CameraUtil::cz = 0;
void CameraUtil::calCamera() {
    ty = float(sin(toRadians(yj)) * CAMERA_R + cy);
    float txz = float(cos(toRadians(yj)) * CAMERA_R);
    tx = float(sin(toRadians(degree)) * txz + cx);
    tz = float(cos(toRadians(degree)) * txz + cz);
    float upY = float(cos(toRadians(yj)));
    float upXZ = float(sin(toRadians(yj)));
    float upX = float(-upXZ * sin(toRadians(degree)));
    float upZ = float(-upXZ * cos(toRadians(degree)));
    camera9Para[0] = cx;
    camera9Para[1] = cy;
    camera9Para[2] = cz;
    camera9Para[3] = tx;
    camera9Para[4] = ty;
    camera9Para[5] = tz;
    camera9Para[6] = upX;
    camera9Para[7] = upY;
    camera9Para[8] = upZ;
}
/*
* @param yjSpan y方向的偏移距离
* @param cxSpan x方向的偏移距离
* */
void CameraUtil::calCamera(float yjSpan, float cxSpan) {
    yj = yj + yjSpan;
    degree = degree + cxSpan;
    if (degree >= 360) {
        degree = degree - 360;
    } else if (degree <= 0) {
        degree = degree + 360;
    }
    calCamera();
}
void CameraUtil::cameraGo(float goBack, float leftRight) {
    float xStep = float(-goBack * sin(toRadians(degree)) - leftRight * sin(toRadians(degree + 90)));
    float zStep = float(-goBack * cos(toRadians(degree)) - leftRight * cos(toRadians(degree + 90)));
    tx = tx + xStep;
    tz = tz + zStep;
    calCamera();
}
void CameraUtil::flushCameraToMatrix() {
    MatrixState3D::setCamera(
            camera9Para[0], camera9Para[1], camera9Para[2],
            camera9Para[3], camera9Para[4], camera9Para[5],
            camera9Para[6], camera9Para[7], camera9Para[8]);
}
