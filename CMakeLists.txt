cmake_minimum_required(VERSION 3.4...3.29)

if (${CMAKE_VERSION} VERSION_LESS 3.29)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.29)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(VulkanDemoProjects VERSION 1.0 LANGUAGES CXX)

# AUTO_LOCATE_VULKAN - 值可以是ON或OFF
# ON  - 使用CMake自动定位Vulkan SDK
# OFF - Vulkan SDK路径可以手动指定。这对于在不同的Vulkan版本上测试构建很有帮助
option(AUTO_LOCATE_VULKAN "AUTO_LOCATE_VULKAN" ON)

# AUTO_LOCATE_VULKAN - 值可以是ON或OFF
# ON  - 使用CMake自动定位Vulkan SDK
# OFF - Vulkan SDK路径可以手动指定。这对于在不同的Vulkan版本上测试构建很有帮助
option(AUTO_LOCATE_VULKAN "AUTO_LOCATE_VULKAN" ON)

if (AUTO_LOCATE_VULKAN)
    message(STATUS "尝试使用CMake自动定位Vulkan SDK......")

    # 使用CMake的Vulkan模块查找Vulkan路径
    # 返回布尔值'Vulkan_FOUND'，表示查找成功(ON)或失败(OFF)的状态.
    # 头文件目录路径 - 'Vulkan_INCLUDE_DIRS'；所需库路径 - 'Vulkan_LIBRARY'
    find_package(Vulkan)

    # 尝试从 ${Vulkan_INCLUDE_DIRS} 提取 VulkanSDK 路径
    if (NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
        set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
        STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
    endif ()

    if (NOT Vulkan_FOUND)
        # CMake 可能无法定位库，但可以在 Vulkan SDK 头文件目录变量'Vulkan_INCLUDE_DIRS' 中包含了路径，尝试从这个变量中提取路径。
        message(STATUS "未能定位 Vulkan SDK，正在重试...")
        if (EXISTS "${VULKAN_PATH}")
            message(STATUS "成功定位 Vulkan SDK: ${VULKAN_PATH}")
        else ()
            message("错误: 无法定位 Vulkan SDK。请通过指定'AUTO_LOCATE_VULKAN'为'OFF'关闭自动定位选项并在 CMakeLists.txt 中手动指定路径使用 'VULKAN_SDK' 和 'VULKAN_VERSION' 变量。")
            return()
        endif ()
    endif ()
else ()
    message(STATUS "检查手动设置的SDK路径是否存在......")
    set(VULKAN_SDK "E:/IDE/VulkanSDK")
    set(VULKAN_VERSION "1.3.290.0")
    set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}")
    message(STATUS "使用手动指定的路径: ${VULKAN_PATH}")

    # 检查手动设置的路径是否存在
    if (NOT EXISTS "${VULKAN_PATH}")
        message("无法定位此VulkanSDK路径 VULKAN_PATH: ${VULKAN_PATH}，请指定正确的路径。有关正确安装过程的更多信息，请参阅本书第3章 '与设备握手' 的小节 'Lunar-G SDK入门'和 '使用 CMake 设置第一个项目'，书名 'Learning Vulkan'，ISBN - 9781786469809。")
        return()
    endif ()
endif ()

# ---- Include guards ----
if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(
            FATAL_ERROR
            "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
    )
endif ()

# 不编译GLFW中的示例代码
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

include_directories(Include)

# 跳过glslang库中用不到的部分
set(GLSLANG_TESTS OFF CACHE BOOL "Enable glslang testing" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "Builds glslang and spirv-remap" FORCE)
set(ENABLE_HLSL OFF CACHE BOOL "Enables HLSL input support" FORCE)
set(ENABLE_PCH OFF CACHE BOOL "Enables Precompiled header" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "Enables spirv-opt capability" FORCE)

find_package(Vulkan REQUIRED)

add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/glslang)

# Vulkan学习指南的各个章节
#add_subdirectory(Basic_chapter_03_HandShake)
#add_subdirectory(Basic_chapter_04_DebugVulkan)
#add_subdirectory(Basic_chapter_05_CommandBuffer)
#add_subdirectory(Basic_chapter_06_SwapChain)
#add_subdirectory(Basic_chapter_07a_VertexBuffer)
#add_subdirectory(Basic_chapter_07b_RenderPass)
#add_subdirectory(Basic_chapter_07c_FrameBuffer)
#add_subdirectory(Basic_chapter_07d_ClearingBackground)
#add_subdirectory(Basic_chapter_07e_ShadersWithSPIRV)
#add_subdirectory(Basic_chapter_08_PipelineStateMgmt)
#add_subdirectory(Basic_chapter_09a_DrawingHelloWorld)
#add_subdirectory(Basic_chapter_09b_IndexedDraw)
#add_subdirectory(Basic_chapter_09c_Resizing)
#add_subdirectory(Basic_chapter_10a_Uniforms_And_Descriptors)
add_subdirectory(Basic_chapter_10b_PushConstant) # 黑屏
add_subdirectory(Basic_chapter_11a_Texture_LinearLayout)
add_subdirectory(Basic_chapter_11b_Texture_OptimalLayout)

# Vulkan开发实战详解的各个章节
#add_subdirectory(Chapter01-01)

#add_subdirectory(Chapter02-01)
#add_subdirectory(Chapter02-02)

#add_subdirectory(Chapter04-01)
#add_subdirectory(Chapter04-02)
#add_subdirectory(Chapter04-03)
#add_subdirectory(Chapter04-04)
#add_subdirectory(Chapter04-05)
#add_subdirectory(Chapter04-06)
#add_subdirectory(Chapter04-07)
#add_subdirectory(Chapter04-08)
#add_subdirectory(Chapter04-09)
#add_subdirectory(Chapter04-10)
#add_subdirectory(Chapter04-11)
#add_subdirectory(Chapter04-12)
#add_subdirectory(Chapter04-13)
#add_subdirectory(Chapter04-14)
#add_subdirectory(Chapter04-15)
#add_subdirectory(Chapter04-16)

#add_subdirectory(Chapter05-01)
#add_subdirectory(Chapter05-02)
#add_subdirectory(Chapter05-03)
#add_subdirectory(Chapter05-04)
#add_subdirectory(Chapter05-05)
#add_subdirectory(Chapter05-06)
#add_subdirectory(Chapter05-07)
#add_subdirectory(Chapter05-08)
#add_subdirectory(Chapter05-09)
#add_subdirectory(Chapter05-10)

#add_subdirectory(Chapter06-01)
#add_subdirectory(Chapter06-02)
#add_subdirectory(Chapter06-03)
#add_subdirectory(Chapter06-04)
#add_subdirectory(Chapter06-05)
#add_subdirectory(Chapter06-06)
#add_subdirectory(Chapter06-07)
#add_subdirectory(Chapter06-08)
#add_subdirectory(Chapter06-09)
#add_subdirectory(Chapter06-10)
#add_subdirectory(Chapter06-11)

#add_subdirectory(Chapter07-01)
#add_subdirectory(Chapter07-02)
#add_subdirectory(Chapter07-03)
#add_subdirectory(Chapter07-04)
#add_subdirectory(Chapter07-05)
#add_subdirectory(Chapter07-06)

#add_subdirectory(Chapter08-01)
#add_subdirectory(Chapter08-01A)
#add_subdirectory(Chapter08-02)
#add_subdirectory(Chapter08-03)
#add_subdirectory(Chapter08-04)

#add_subdirectory(Chapter09-01)
#add_subdirectory(Chapter09-02)
#add_subdirectory(Chapter09-03)
#add_subdirectory(Chapter09-04)
#add_subdirectory(Chapter09-04A)
#add_subdirectory(Chapter09-05)
#add_subdirectory(Chapter09-06)
#add_subdirectory(Chapter09-07)
#add_subdirectory(Chapter09-08)
#add_subdirectory(Chapter09-09)
#add_subdirectory(Chapter09-10)
#add_subdirectory(Chapter09-11)
#add_subdirectory(Chapter09-12)
#add_subdirectory(Chapter09-13)
#add_subdirectory(Chapter09-14)

#add_subdirectory(Chapter10-01)
#add_subdirectory(Chapter10-02)
#add_subdirectory(Chapter10-03)
#add_subdirectory(Chapter10-04)

#add_subdirectory(Chapter11-01)
#add_subdirectory(Chapter11-02)
#add_subdirectory(Chapter11-03)
#add_subdirectory(Chapter11-04)
#add_subdirectory(Chapter11-05)
#add_subdirectory(Chapter11-06)
#add_subdirectory(Chapter11-07)

#add_subdirectory(Chapter12-01)
#add_subdirectory(Chapter12-02)
#add_subdirectory(Chapter12-03)
#add_subdirectory(Chapter12-04)
#add_subdirectory(Chapter12-05)
#add_subdirectory(Chapter12-05A)
#add_subdirectory(Chapter12-06)
#add_subdirectory(Chapter12-07)
#add_subdirectory(Chapter12-08)
#add_subdirectory(Chapter12-09)
#add_subdirectory(Chapter12-10)
#add_subdirectory(Chapter12-11)
#add_subdirectory(Chapter12-12)
#add_subdirectory(Chapter12-13)
#add_subdirectory(Chapter12-14)
#add_subdirectory(Chapter12-15)

#add_subdirectory(Chapter13-01)
#add_subdirectory(Chapter13-02)
#add_subdirectory(Chapter13-03)
#add_subdirectory(Chapter13-04)
#add_subdirectory(Chapter13-05)
#add_subdirectory(Chapter13-06)
#add_subdirectory(Chapter13-07)
#add_subdirectory(Chapter13-08)
#add_subdirectory(Chapter13-09)
#add_subdirectory(Chapter13-10)
#add_subdirectory(Chapter13-11)

#add_subdirectory(Chapter14-01)
#add_subdirectory(Chapter14-02)
#add_subdirectory(Chapter14-03V1)
#add_subdirectory(Chapter14-03V2)
#add_subdirectory(Chapter14-03V3)
#add_subdirectory(Chapter14-04)
#add_subdirectory(Chapter14-05)
#add_subdirectory(Chapter14-06V1)
#add_subdirectory(Chapter14-06V2)
#add_subdirectory(Chapter14-07)
#add_subdirectory(Chapter14-08)
#add_subdirectory(Chapter14-09)
#add_subdirectory(Chapter14-10)

#add_subdirectory(Chapter15-01)
#add_subdirectory(Chapter15-02)
#add_subdirectory(Chapter15-03)
#add_subdirectory(Chapter15-04)
#add_subdirectory(Chapter15-05)
#add_subdirectory(Chapter15-06)
#add_subdirectory(Chapter15-07)
#add_subdirectory(Chapter15-08)

#add_subdirectory(Chapter16-01)
add_subdirectory(Chapter16-02) #未修复
#add_subdirectory(Chapter16-03)
#add_subdirectory(Chapter16-04)
#add_subdirectory(Chapter16-05)

#add_subdirectory(Chapter17-01)
#add_subdirectory(Chapter17-02)
#add_subdirectory(Chapter17-03)
#add_subdirectory(Chapter17-04)
#add_subdirectory(Chapter17-05)
#add_subdirectory(Chapter17-06)
add_subdirectory(Chapter17-07) #未修复
#add_subdirectory(Chapter17-08)
#add_subdirectory(Chapter17-09)
#add_subdirectory(Chapter17-10)

#add_subdirectory(Chapter18-01)
#add_subdirectory(Chapter18-02)
#add_subdirectory(Chapter18-03)
#add_subdirectory(Chapter18-04)
#add_subdirectory(Chapter18-05)
#add_subdirectory(Chapter18-06)
#add_subdirectory(Chapter18-07)

#add_subdirectory(Chapter19-01)