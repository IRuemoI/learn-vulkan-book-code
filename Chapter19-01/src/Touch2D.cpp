#include "Touch2D.h"
#include "My3DLayer.h"
#include "Util.h"
#include "VulkanDemoApp.h"

#define Touch_value 20
bool Touch2D::Button_about = false;
bool Touch2D::Button_resume = false;
bool Touch2D::Button_exit = false;
bool Touch2D::Button_start = false;
bool Touch2D::Button_sound = false;
bool Touch2D::Button_back = false;

//bool Touch2D::isBGMusicOff = false;
bool Touch2D::isEffectMusicOff = false;


void Touch2D::CheckButton_about(int x, int y) {
    if (y > (7 * VulkanDemoApp::screenHeight / 10.0f) && y < (8 * VulkanDemoApp::screenHeight / 10.0f)) {
        if (x > ((9 / 16.0f - 0.4) * VulkanDemoApp::screenHeight / 2 - VulkanDemoApp::screenHeight / 20.0f) && x < ((9 / 16.0f - 0.4) * VulkanDemoApp::screenHeight / 2 + VulkanDemoApp::screenHeight / 20.0f)) {//关于按钮
            printf("关于按钮");
            Touch2D::Button_about = !Touch2D::Button_about;
        }
    }
}

void Touch2D::CheckButton_sound(int x, int y) {
    if (y > (7 * VulkanDemoApp::screenHeight / 10.0f) && y < (8 * VulkanDemoApp::screenHeight / 10.0f)) {
        if (x > ((9 / 16.0f) * VulkanDemoApp::screenHeight / 2 - VulkanDemoApp::screenHeight / 20.0f) && x < ((9 / 16.0f) * VulkanDemoApp::screenHeight / 2 + VulkanDemoApp::screenHeight / 20.0f)) {
            printf("音效按钮\n");
            Touch2D::Button_sound = !Touch2D::Button_sound;

            //Touch2D::isBGMusicOff = !Touch2D::isBGMusicOff;

            Touch2D::isEffectMusicOff = !Touch2D::isEffectMusicOff;
        }
    }
}


void Touch2D::CheckButton_exit(int x, int y) {
    if (y > (7 * VulkanDemoApp::screenHeight / 10.0f) && y < (8 * VulkanDemoApp::screenHeight / 10.0f)) {
        if (x > ((9 / 16.0f + 0.4) * VulkanDemoApp::screenHeight / 2 - VulkanDemoApp::screenHeight / 20.0f) && x < ((9 / 16.0f + 0.4) * VulkanDemoApp::screenHeight / 2 + VulkanDemoApp::screenHeight / 20.0f)) {//退出按钮
            printf("退出游戏\n");
            exit(0);
        }
    }
}


void Touch2D::CheckButton_resume(int x, int y) {
    if (y > (7 * VulkanDemoApp::screenHeight / 10.0f) && y < (8 * VulkanDemoApp::screenHeight / 10.0f)) {
        if (x > ((9 / 16.0f - 0.11) * VulkanDemoApp::screenHeight / 2 - VulkanDemoApp::screenHeight / 20.0f) && x < ((9 / 16.0f - 0.11) * VulkanDemoApp::screenHeight / 2 + VulkanDemoApp::screenHeight / 20.0f)) {//重新开始按钮
            printf("重新开始\n");
            Touch2D::Button_resume = true;
        }
    }
}

void Touch2D::CheckButton_back(int x, int y) {
    if (y > (7 * VulkanDemoApp::screenHeight / 10.0f) && y < (8 * VulkanDemoApp::screenHeight / 10.0f)) {
        if (x > ((9 / 16.0f + 0.11) * VulkanDemoApp::screenHeight / 2 - VulkanDemoApp::screenHeight / 20.0f) && x < ((9 / 16.0f + 0.11) * VulkanDemoApp::screenHeight / 2 + VulkanDemoApp::screenHeight / 20.0f)) {//返回主菜单按钮
            printf("返回主菜单\n");
            My3DLayer::RepeatLevel();//重置关卡
            Touch2D::Button_back = true;
            Touch2D::Button_start = false;
            Touch2D::Button_about = false;
            Touch2D::Button_resume = false;
            Touch2D::Button_exit = false;
        }
    }
}


void Touch2D::CheckButton_start(int x, int y) {
    if (y > (0.3f * VulkanDemoApp::screenHeight) && y < (0.45f * VulkanDemoApp::screenHeight)) {
        if (x > ((9 / 16.0f) * VulkanDemoApp::screenHeight / 2 - 0.1f * VulkanDemoApp::screenHeight) && x < ((9 / 16.0f) * VulkanDemoApp::screenHeight / 2 + 0.1f * VulkanDemoApp::screenHeight)) {//开始按钮
            printf("开始游戏\n");
            Touch2D::Button_back = false;
            Touch2D::Button_start = true;
            Touch2D::Button_about = false;
        }
    }
}