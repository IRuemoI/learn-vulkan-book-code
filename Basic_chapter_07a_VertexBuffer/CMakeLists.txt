#
# Chapter 7a - Vertex Buffer.
#

cmake_minimum_required(VERSION 3.10.1)

set(Recipe_Name "7_a_VertexBuffer")

# 指定一个合适的工程名称
project(${Recipe_Name})

# 在此处添加任何所需预处理器定义
add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

# 用于构建 Vulkan 应用的 vulkan-1库
set(VULKAN_LIB_LIST "Vulkan::Vulkan")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	# 包含来自 Vulkan SDK 的 Vulkan 头文件
	include_directories(AFTER ${VULKAN_PATH}/Include)

	# 指定用于链接的vulkan-1库所在目录
	link_directories(${VULKAN_PATH}/Bin)
endif()


# 定义目录及其包含的文件夹和文件
if(WIN32)
    source_group("include" REGULAR_EXPRESSION "include/*")
    source_group("source" REGULAR_EXPRESSION "source/*")
endif(WIN32)

# 定义头文件路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 收集编译所需的头文件和源文件列表
file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
file(GLOB_RECURSE HPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.*)

# 构建项目，为其命名并附带要编译的文件列表
add_executable(${Recipe_Name} ${CPP_FILES} ${HPP_FILES})

# 链接用于调试或用于发布依赖库
target_link_libraries( ${Recipe_Name} ${VULKAN_LIB_LIST} )

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
