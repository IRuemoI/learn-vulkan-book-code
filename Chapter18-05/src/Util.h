#pragma once

#include "glslang/SPIRV/GlslangToSpv.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#pragma comment(linker, "/subsystem:console") /*windows--不带命令行输出窗口运行 console--带命令行输出窗口运行*/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#ifndef NOMINMAX
#define NOMINMAX /* 不要让Windows定义min()或max() */
#endif
#define APP_NAME_STR_LEN 80
#endif

#include "VulkanDemoApp.h"
#include "glslang/Public/ShaderLang.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

/* 管线布置创作，创作和描述符设置布局数 */
#define NUM_DESCRIPTOR_SETS 1
/*image renderpass  pipeline 创建数量 */
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS

/* 等待命令缓冲区填充的时间上限 纳秒*/
#define FENCE_TIMEOUT 100000000

void wait_seconds(int seconds);

void print_UUID(uint8_t *pipelineCacheUUID);

std::string get_file_directory();

typedef unsigned long long timestamp_t;

timestamp_t get_milliseconds();

void init_resources(TBuiltInResource &Resources);

EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type);

timestamp_t get_milliseconds();

void wait_seconds(int seconds);


// 设置光标位置回调函数
void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos);

// 设置鼠标滚轮回调函数
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// 设置鼠标点击回调函数
void mouse_button_callback(GLFWwindow *window, int button, int action, int i);

void processInputs(GLFWwindow *window);