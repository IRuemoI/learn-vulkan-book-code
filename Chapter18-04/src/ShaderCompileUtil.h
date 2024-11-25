#pragma once
#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.h>

// 将GLSL着色器脚本编译为spirv的方法
bool compileGLSLtoSPV(VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv);