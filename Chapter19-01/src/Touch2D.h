#pragma once
#include <string>
using namespace std;//指定使用的命名空间
class Touch2D {
public:
    static bool Button_about;
    static bool Button_resume;
    static bool Button_start;
    static bool Button_exit;
    static bool Button_sound;
    static bool Button_back;

    static bool isBGMusicOff;
    static bool isEffectMusicOff;

    static bool flag;

    static void CheckButton_about(int x, int y);
    static void CheckButton_resume(int x, int y);
    static void CheckButton_back(int x, int y);
    static void CheckButton_exit(int x, int y);
    static void CheckButton_start(int x, int y);
    static void CheckButton_sound(int x, int y);
};
