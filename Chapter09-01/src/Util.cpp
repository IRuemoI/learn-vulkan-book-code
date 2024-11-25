#include "Util.h"
#include "MatrixState3D.h"
#include "TreeControl.h"

using namespace std;

int preX = 0;//鼠标的横坐标
int preY = 0;//鼠标的纵坐标
bool left_mouse_down = false;
bool isClick = true;
void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
    VulkanDemoApp::currentCursorX = xpos;
    VulkanDemoApp::currentCursorY = ypos;

    // 处理鼠标拖动事件
    if (left_mouse_down) {
        VulkanDemoApp::cursorDeltaX = preX - VulkanDemoApp::currentCursorX;
        VulkanDemoApp::cursorDeltaY = preY - VulkanDemoApp::currentCursorY;
        if (abs((int) VulkanDemoApp::cursorDeltaX) > 2 || abs((int) VulkanDemoApp::cursorDeltaY) > 2) {
            isClick = false;
        }
        //printf("拖动鼠标的变化量:[X:%f,Y:%f]\n", VulkanAdvanceApplication::cursorDeltaX, VulkanAdvanceApplication::cursorDeltaY);
    }

    //保存本次的鼠标位置作为上次的位置
    preX = (int) xpos;
    preY = (int) ypos;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        left_mouse_down = true;
        printf("鼠标按下\n");
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        left_mouse_down = false;
        if (preX > 0 && preX < VulkanDemoApp::screenWidth / 2 && preY > 0 && preY < VulkanDemoApp::screenHeight / 2) {//左上部分
            VulkanDemoApp::direction += VulkanDemoApp::DEGREE_SPAN;
        } else if (preX > 0 && preX < VulkanDemoApp::screenWidth / 2 && preY > VulkanDemoApp::screenHeight / 2 && preY < VulkanDemoApp::screenHeight) {//左下部分
            VulkanDemoApp::Offset += 0.5;
        } else if (preX > VulkanDemoApp::screenWidth / 2 && preX < VulkanDemoApp::screenWidth && preY > 0 && preY < VulkanDemoApp::screenHeight / 2) {//右上部分
            VulkanDemoApp::direction -= VulkanDemoApp::DEGREE_SPAN;
        } else if (preX > VulkanDemoApp::screenWidth / 2 && preX < VulkanDemoApp::screenWidth && preY > VulkanDemoApp::screenHeight / 2 && preY < VulkanDemoApp::screenHeight) {//右下部分
            VulkanDemoApp::Offset -= 0.5;
        }
        VulkanDemoApp::cx = sinf(VulkanDemoApp::direction) * VulkanDemoApp::Offset;//计算摄像机的位置
        VulkanDemoApp::cz = cosf(VulkanDemoApp::direction) * VulkanDemoApp::Offset;
        MatrixState3D::setCamera(VulkanDemoApp::cx, 10, VulkanDemoApp::cz, 0, 0, 0, 0, 1, 0);//设置摄像机的位置
        TreeControl::calculateBillboardDirection();                                          //对树群按照到摄像机的距离大小进行排序
        printf("鼠标松开\n");
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    printf("scroll called [%F %F]\n", xoffset, yoffset);
}

// 键盘按键回调函数
void processInputs(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        std::cout << "按下了W键" << std::endl;
}

void init_resources(TBuiltInResource &Resources) {
    Resources.maxLights = 32;
    Resources.maxClipPlanes = 6;
    Resources.maxTextureUnits = 32;
    Resources.maxTextureCoords = 32;
    Resources.maxVertexAttribs = 64;
    Resources.maxVertexUniformComponents = 4096;
    Resources.maxVaryingFloats = 64;
    Resources.maxVertexTextureImageUnits = 32;
    Resources.maxCombinedTextureImageUnits = 80;
    Resources.maxTextureImageUnits = 32;
    Resources.maxFragmentUniformComponents = 4096;
    Resources.maxDrawBuffers = 32;
    Resources.maxVertexUniformVectors = 128;
    Resources.maxVaryingVectors = 8;
    Resources.maxFragmentUniformVectors = 16;
    Resources.maxVertexOutputVectors = 16;
    Resources.maxFragmentInputVectors = 15;
    Resources.minProgramTexelOffset = -8;
    Resources.maxProgramTexelOffset = 7;
    Resources.maxClipDistances = 8;
    Resources.maxComputeWorkGroupCountX = 65535;
    Resources.maxComputeWorkGroupCountY = 65535;
    Resources.maxComputeWorkGroupCountZ = 65535;
    Resources.maxComputeWorkGroupSizeX = 1024;
    Resources.maxComputeWorkGroupSizeY = 1024;
    Resources.maxComputeWorkGroupSizeZ = 64;
    Resources.maxComputeUniformComponents = 1024;
    Resources.maxComputeTextureImageUnits = 16;
    Resources.maxComputeImageUniforms = 8;
    Resources.maxComputeAtomicCounters = 8;
    Resources.maxComputeAtomicCounterBuffers = 1;
    Resources.maxVaryingComponents = 60;
    Resources.maxVertexOutputComponents = 64;
    Resources.maxGeometryInputComponents = 64;
    Resources.maxGeometryOutputComponents = 128;
    Resources.maxFragmentInputComponents = 128;
    Resources.maxImageUnits = 8;
    Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
    Resources.maxCombinedShaderOutputResources = 8;
    Resources.maxImageSamples = 0;
    Resources.maxVertexImageUniforms = 0;
    Resources.maxTessControlImageUniforms = 0;
    Resources.maxTessEvaluationImageUniforms = 0;
    Resources.maxGeometryImageUniforms = 0;
    Resources.maxFragmentImageUniforms = 8;
    Resources.maxCombinedImageUniforms = 8;
    Resources.maxGeometryTextureImageUnits = 16;
    Resources.maxGeometryOutputVertices = 256;
    Resources.maxGeometryTotalOutputComponents = 1024;
    Resources.maxGeometryUniformComponents = 1024;
    Resources.maxGeometryVaryingComponents = 64;
    Resources.maxTessControlInputComponents = 128;
    Resources.maxTessControlOutputComponents = 128;
    Resources.maxTessControlTextureImageUnits = 16;
    Resources.maxTessControlUniformComponents = 1024;
    Resources.maxTessControlTotalOutputComponents = 4096;
    Resources.maxTessEvaluationInputComponents = 128;
    Resources.maxTessEvaluationOutputComponents = 128;
    Resources.maxTessEvaluationTextureImageUnits = 16;
    Resources.maxTessEvaluationUniformComponents = 1024;
    Resources.maxTessPatchComponents = 120;
    Resources.maxPatchVertices = 32;
    Resources.maxTessGenLevel = 64;
    Resources.maxViewports = 16;
    Resources.maxVertexAtomicCounters = 0;
    Resources.maxTessControlAtomicCounters = 0;
    Resources.maxTessEvaluationAtomicCounters = 0;
    Resources.maxGeometryAtomicCounters = 0;
    Resources.maxFragmentAtomicCounters = 8;
    Resources.maxCombinedAtomicCounters = 8;
    Resources.maxAtomicCounterBindings = 1;
    Resources.maxVertexAtomicCounterBuffers = 0;
    Resources.maxTessControlAtomicCounterBuffers = 0;
    Resources.maxTessEvaluationAtomicCounterBuffers = 0;
    Resources.maxGeometryAtomicCounterBuffers = 0;
    Resources.maxFragmentAtomicCounterBuffers = 1;
    Resources.maxCombinedAtomicCounterBuffers = 1;
    Resources.maxAtomicCounterBufferSize = 16384;
    Resources.maxTransformFeedbackBuffers = 4;
    Resources.maxTransformFeedbackInterleavedComponents = 64;
    Resources.maxCullDistances = 8;
    Resources.maxCombinedClipAndCullDistances = 8;
    Resources.maxSamples = 4;
    Resources.limits.nonInductiveForLoops = 1;
    Resources.limits.whileLoops = 1;
    Resources.limits.doWhileLoops = 1;
    Resources.limits.generalUniformIndexing = 1;
    Resources.limits.generalAttributeMatrixVectorIndexing = 1;
    Resources.limits.generalVaryingIndexing = 1;
    Resources.limits.generalSamplerIndexing = 1;
    Resources.limits.generalVariableIndexing = 1;
    Resources.limits.generalConstantMatrixVectorIndexing = 1;
}

EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type) {
    switch (shader_type) {
        case VK_SHADER_STAGE_VERTEX_BIT:
            return EShLangVertex;

        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return EShLangTessControl;

        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return EShLangTessEvaluation;

        case VK_SHADER_STAGE_GEOMETRY_BIT:
            return EShLangGeometry;

        case VK_SHADER_STAGE_FRAGMENT_BIT:
            return EShLangFragment;

        case VK_SHADER_STAGE_COMPUTE_BIT:
            return EShLangCompute;

        default:
            return EShLangVertex;
    }
}

void wait_seconds(int seconds) {
    Sleep(seconds * 1000);
}

// 获取毫秒数
timestamp_t get_milliseconds() {
    LARGE_INTEGER frequency;
    BOOL useQPC = QueryPerformanceFrequency(&frequency);
    if (useQPC) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / frequency.QuadPart;
    } else {
        return GetTickCount64();
    }
}
