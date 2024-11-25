#pragma once
class BallData {
public:
    static float *vdata;
    static int dataByteCount;
    static int vCount;
    static void genBallData(float angleSpan, float r2);
    static float r;
};