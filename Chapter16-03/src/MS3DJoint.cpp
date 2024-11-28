#include "MS3DJoint.h"
#include "FileUtil.h"
#include <fstream>
MS3DJoint::MS3DJoint(unsigned char *binaryData, int *binaryData_index) {
    FileUtil::myReadByte(binaryData, binaryData_index);
    name = FileUtil::myReadString(binaryData, binaryData_index, 32);
    parentName = FileUtil::myReadString(binaryData, binaryData_index, 32);
    float x = FileUtil::myReadFloat(binaryData, binaryData_index);
    float y = FileUtil::myReadFloat(binaryData, binaryData_index);
    float z = FileUtil::myReadFloat(binaryData, binaryData_index);
    trotate = new Vector3f(
            x, y, z);
    x = FileUtil::myReadFloat(binaryData, binaryData_index);
    y = FileUtil::myReadFloat(binaryData, binaryData_index);
    z = FileUtil::myReadFloat(binaryData, binaryData_index);
    tposition = new Vector3f(
            x, y, z);
    count_rot = FileUtil::myReadUnsignedShort(binaryData, binaryData_index);
    count_pos = FileUtil::myReadUnsignedShort(binaryData, binaryData_index);
    if (count_rot > 0) {
        for (int i = 0; i < count_rot; i++) {
            MS3DKeyFrameRotate *rotateKF = new MS3DKeyFrameRotate(binaryData, binaryData_index);
            rotates.push_back(rotateKF);
        }
    }
    if (count_pos > 0) {
        for (int i = 0; i < count_pos; i++) {
            MS3DKeyFramePosition *position = new MS3DKeyFramePosition(binaryData, binaryData_index);
            positions.push_back(position);
        }
    }
    ifparent = false;
    tmatrix = new Mat4();
}
MS3DJoint::~MS3DJoint() {
    delete trotate;//TODO:这里有bug
    delete tposition;
    delete relative;
    delete absolute;
    delete tmatrix;
    delete parent;
    delete mHelper;
    delete tranV3Helper;
    delete tranV4Helper;
    vector<MS3DKeyFrameRotate *>::iterator iter = rotates.begin();
    for (; iter != rotates.end(); iter++) {
        MS3DKeyFrameRotate *vt = (*iter);
        delete vt;
    }
    rotates.clear();
    vector<MS3DKeyFramePosition *>::iterator iter0 = positions.begin();
    for (; iter0 != positions.end(); iter0++) {
        MS3DKeyFramePosition *vt = (*iter0);
        delete vt;
    }
    positions.clear();
}
void MS3DJoint::update(float ttime) {//更新关节数据的方法
    if (rotates.size() <= 0 && positions.size() <= 0) {//若没有旋转关键帧和平移关键帧数据
        tmatrix->copyFrom(absolute);//将初始绝对矩阵的值复制进当前绝对矩阵
        return;//返回
    }
    Mat4 *matrix0 = ttrotate(ttime);//获取当前时刻的旋转数据
    matrix0->setTranslation(ttposition(ttime));//将当前时刻的平移数据记录进矩阵
    matrix0->mul(relative, matrix0);//与自身相对矩阵相乘
    if (ifparent) {//若有父关节
        tmatrix = matrix0->mul(parent->tmatrix, matrix0);//乘以父关节的当前矩阵
    } else {//若无父关节
        tmatrix = matrix0;//给当前绝对矩阵赋值
    }
}
Vector3f *MS3DJoint::ttposition(float time) {//根据当前播放时间和关键帧数据进行平移数据插值计算的方法
    int index = 0;//初始化索引为0
    int size = count_pos;//得到平移关键帧的数量
    while (index < size && positions[index]->getTime() < time) {//根据当前时间计算用于插值的结束关键帧索引
        index++;//关键帧索引加1
    }
    if (index == 0) {//若结束关键帧索引为0
        return positions[index]->getPosition();//获取第一帧的平移数据并返回
    } else if (index == size) {//若结束关键帧索引等于关键帧数量
        return positions[index - 1]->getPosition();//获取最后一帧的平移数据并返回
    } else {//若结束关键帧索引既不为0 也不超过最终关键帧
        MS3DKeyFramePosition *right = positions[index];//插值用结束关键帧的平移数据
        MS3DKeyFramePosition *left = positions[index - 1];//上一关键帧的平移数据
        tranV3Helper->interpolate(left->getPosition(),//插值产生当前时刻的平移数据
                                  right->getPosition(), (time - left->getTime()) / (right->getTime() - left->getTime()));
        return tranV3Helper;//返回当前时刻的平移数据
    }
}
Mat4 *MS3DJoint::ttrotate(float time) {//根据当前播放时间和关键帧数据进行旋转数据插值计算的方法
    int index = 0;//初始化索引为0
    int size = count_rot;//获取旋转关键帧的数量
    while (index < size && rotates[index]->getTime() < time) {//根据当前时间计算用于插值的结束关键帧索引
        index++;//关键帧索引加1
    }
    if (index == 0) {//若结束关键帧索引为0
        tranV4Helper = rotates[index]->getRotate();//获取第一帧的旋转数据
    } else if (index == size) {//若结束关键帧索引等于关键帧数量
        tranV4Helper = rotates[index - 1]->getRotate();//获取最后一帧的旋转数据
    } else {//若结束关键帧索引既不为0 也不超过最终关键帧
        MS3DKeyFrameRotate *right = rotates[index];//插值用结束关键帧的旋转
        MS3DKeyFrameRotate *left = rotates[index - 1];//上一关键帧的旋转
        tranV4Helper->interpolate(left->getRotate(),//插值产生当前时刻的旋转（四元数格式）
                                  right->getRotate(), (time - left->getTime()) / (right->getTime() - left->getTime()));
    }
    mHelper->genRotateFromQuaternion(tranV4Helper);//将四元数形式的旋转转换为矩阵形式
    return mHelper;//返回当前时刻的旋转数据
}
Mat4 *MS3DJoint::getMatrix() {
    return tmatrix;
}
Mat4 *MS3DJoint::getAbsolute() {
    return absolute;
}
