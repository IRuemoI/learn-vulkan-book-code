#include "VulkanDemoApp.h"
#include <thread>

// 程序入口
int main() {
    try {
        // 实例化一个三角形渲染程序对象
        // 这里使用多线程调用vulkan程序，在后期我们将探讨如何实现多线程渲染
        auto *app = new VulkanDemoApp();
        std::thread renderThread(&VulkanDemoApp::run, app);
        renderThread.join();
    } catch (const std::exception &e) {
        // 如果有异常，输出异常信息
        std::cerr << e.what() << std::endl;
        // 返回程序运行结果为失败
        return EXIT_FAILURE;
    }
}