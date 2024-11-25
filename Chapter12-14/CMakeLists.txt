# 设置当前章节的名称
set(CHAPTER_NAME Chapter12-14)

# ---- Include guards ----
if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(
            FATAL_ERROR
            "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
    )
endif ()

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/${CHAPTER_NAME}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${CHAPTER_NAME})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CHAPTER_NAME}/lib)

# 查找当前目录下所有源文件和头文件，并将它们分别存入变量中
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/${CHAPTER_NAME}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/${CHAPTER_NAME}/src/*.h")

# 创建一个可执行文件
add_executable(${CHAPTER_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${CHAPTER_NAME}
        Vulkan::Vulkan
        glslang
        glslang::SPIRV
        glslang::glslang-default-resource-limits
        glfw
)