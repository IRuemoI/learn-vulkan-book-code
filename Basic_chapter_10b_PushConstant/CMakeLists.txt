#
# Chapter 10 - Push Contants.
#

cmake_minimum_required(VERSION 3.10.1)

set(Recipe_Name "10b_PushConstants")

# BUILD_SPV_ON_COMPILE_TIME - accepted value ON or OFF, default value OFF.
# ON  - Reads the GLSL shader file and auto convert in SPIR-V form (.spv). 
# 			This requires additional libraries support from 
#			VulkanSDK like SPIRV glslang OGLCompiler OSDependent HLSL
# OFF - Only reads .spv files, which need to be compiled offline 
#			using glslangValidator.exe.
# For example: glslangValidator.exe <GLSL file name> -V -o <output filename in SPIR-V(.spv) form>
option(BUILD_SPV_ON_COMPILE_TIME "BUILD_SPV_ON_COMPILE_TIME" OFF)

# 指定一个合适的工程名称
project(${Recipe_Name})

# 在此处添加任何所需预处理器定义
add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

# GLM SETUP - Mathematic libraries for 3D transformation
set(EXTDIR "${CMAKE_SOURCE_DIR}/dependencies")
set(GLMINCLUDES "${EXTDIR}")
get_filename_component(GLMINC_PREFIX "${GLMINCLUDES}" ABSOLUTE)
if (NOT EXISTS ${GLMINC_PREFIX})
    message(FATAL_ERROR "Necessary glm headers do not exist: " ${GLMINC_PREFIX})
endif ()
include_directories(${GLMINC_PREFIX})

# 用于构建 Vulkan 应用的 vulkan-1库
set(VULKAN_LIB_LIST "Vulkan::Vulkan")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 包含来自 Vulkan SDK 的 Vulkan 头文件
    include_directories(AFTER ${VULKAN_PATH}/Include)

    # 指定用于链接的vulkan-1库所在目录
    link_directories(${VULKAN_PATH}/Bin)

    if (BUILD_SPV_ON_COMPILE_TIME)

        # Preprocessor  flag allows the solution to use glslang library functions
        add_definitions(-DAUTO_COMPILE_GLSL_TO_SPV)

        #GLSL - use Vulkan SDK's glslang library for compling GLSL to SPV
        # This does not require offline coversion of GLSL shader to SPIR-V(.spv) form
        set(GLSLANGDIR "${VULKAN_PATH}/glslang")
        get_filename_component(GLSLANG_PREFIX "${GLSLANGDIR}" ABSOLUTE)
        if (NOT EXISTS ${GLSLANG_PREFIX})
            message(FATAL_ERROR "Necessary glslang components do not exist: " ${GLSLANG_PREFIX})
        endif ()
        include_directories(${GLSLANG_PREFIX})

        # If compiling GLSL to SPV using we need the following libraries
        set(GLSLANG_LIBS SPIRV glslang OGLCompiler OSDependent HLSL)

        # Generate the list of files to link, per flavor.
        foreach (x ${GLSLANG_LIBS})
            list(APPEND VULKAN_LIB_LIST debug ${x}d optimized ${x})
        endforeach ()

        # Note: While configuring CMake for glslang we created the binaries in a "build" folder inside ${VULKAN_PATH}/glslang.
        # Therefore, you must edit the below lines for your custorm path like <Your binary path>/OGLCompilersDLL , <Your binary path>/OSDependent/Windows
        link_directories(${VULKAN_PATH}/glslang/build/OGLCompilersDLL)
        link_directories(${VULKAN_PATH}/glslang/build/glslang/OSDependent/Windows)
        link_directories(${VULKAN_PATH}/glslang/build/glslang)
        link_directories(${VULKAN_PATH}/glslang/build/SPIRV)
        link_directories(${VULKAN_PATH}/glslang/build/hlsl)
    endif ()
endif ()


# 定义目录及其包含的文件夹和文件
if (WIN32)
    source_group("include" REGULAR_EXPRESSION "include/*")
    source_group("source" REGULAR_EXPRESSION "source/*")
endif (WIN32)

# 定义头文件路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 收集编译所需的头文件和源文件列表
file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
file(GLOB_RECURSE HPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.*)

# 构建项目，为其命名并附带要编译的文件列表
add_executable(${Recipe_Name} ${CPP_FILES} ${HPP_FILES})

# 链接用于调试或用于发布依赖库
target_link_libraries(${Recipe_Name} ${VULKAN_LIB_LIST})

# 定义项目属性
set_property(TARGET ${Recipe_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
set_property(TARGET ${Recipe_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
set_property(TARGET ${Recipe_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
set_property(TARGET ${Recipe_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
set_property(TARGET ${Recipe_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/binaries)

# 定义用于构建项目的C++标准版本号
set_property(TARGET ${Recipe_Name} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${Recipe_Name} PROPERTY CXX_STANDARD_REQUIRED ON)

# 定义用于构建项目的C标准版本号
set_property(TARGET ${Recipe_Name} PROPERTY C_STANDARD 99)
set_property(TARGET ${Recipe_Name} PROPERTY C_STANDARD_REQUIRED ON)
