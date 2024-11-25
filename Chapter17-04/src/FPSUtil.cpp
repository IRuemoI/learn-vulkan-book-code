#include "FPSUtil.h"
#include "VulkanDemoApp.h"
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <time.h>
#include <winbase.h>
#include <windows.h>

#define LOGE printf

int FPSUtil::FPSCount;
long long FPSUtil::startTime;
float FPSUtil::currFPS;
long long FPSUtil::beforeTime;
std::vector<int> FPSUtil::drawCount;

std::string intToString(int n) {
    std::ostringstream stream;
    stream << n;//n为int类型
    return stream.str();
}

long long currentTimeMillis() {
    long long time = GetTickCount();
    return time;
}
void FPSUtil::init() {
    FPSCount = 0;
    startTime = currentTimeMillis();
}
void FPSUtil::calFPS() {
    FPSCount++;
    if (FPSCount == 100) {
        FPSCount = 0;
        long long endTime = currentTimeMillis();
        currFPS = (float) (1000.0 / ((endTime - startTime) / 100.0));
        startTime = endTime;
        LOGE("FPS: %f\n", FPSUtil::currFPS);

        //打印绘制数量
        std::string sfjcMsg = ((VulkanDemoApp::isSZTJC) ? "执行视锥体剪裁" : "不执行视锥体剪裁");
        std::string msg = "[" + sfjcMsg + "] Draw Calls [";
        for (int c: drawCount) {
            msg = msg + intToString(c) + " ";
        }
        msg = msg + "]\n";
        LOGE("%s\n", msg.c_str());
        drawCount.clear();
    }
}

void FPSUtil::before() {
    beforeTime = currentTimeMillis();
}
void FPSUtil::after(int dstFPS) {
    int dstSpan = (int) (1000 / dstFPS) + 1;
    long long span = currentTimeMillis() - beforeTime;
    if (span < dstSpan) {
        std::this_thread::sleep_for(std::chrono::milliseconds(dstSpan - span));
    }
}
