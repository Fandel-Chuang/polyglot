cmake_minimum_required(VERSION 3.15)

project(polyglot VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 编译选项
if(MSVC)
    # Visual Studio 编译器选项
    add_compile_options(/W4 /WX- /permissive- /utf-8)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2)
    endif()
else()
    # GCC/Clang 编译器选项
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O2)
    else()
        add_compile_options(-g)
    endif()
endif()

# 源文件
set(SOURCES
    compiler/main.cpp
    compiler/lexer.cpp
    compiler/parser.cpp
    compiler/semantic.cpp
    compiler/codegen.cpp
)

# 头文件
set(HEADERS
    compiler/lexer.h
    compiler/parser.h
    compiler/ast.h
    compiler/semantic.h
    compiler/codegen.h
    compiler/error.h
    compiler/package_manager.h
)

# 创建可执行文件
add_executable(polyglot ${SOURCES} ${HEADERS})

# 设置目标属性
set_target_properties(polyglot PROPERTIES
    OUTPUT_NAME "polyglot"
    VERSION ${PROJECT_VERSION}
)

# 包含目录
target_include_directories(polyglot PRIVATE compiler)

# 平台特定设置
if(WIN32)
    # Windows 特定设置
    set_target_properties(polyglot PROPERTIES
        WIN32_EXECUTABLE FALSE  # 控制台应用程序
    )

    # 如果使用 MinGW，链接静态运行时
    if(MINGW)
        target_link_options(polyglot PRIVATE -static-libgcc -static-libstdc++)
    endif()
elseif(APPLE)
    # macOS 特定设置
    set_target_properties(polyglot PROPERTIES
        MACOSX_BUNDLE FALSE
    )
elseif(UNIX)
    # Linux 特定设置
    # 可以在这里添加 Linux 特定的设置
endif()

# 安装规则
install(TARGETS polyglot
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 复制测试文件
configure_file(${CMAKE_SOURCE_DIR}/test.pg ${CMAKE_BINARY_DIR}/test.pg COPYONLY)

# 创建自定义目标用于测试
add_custom_target(run_test
    COMMAND $<TARGET_FILE:polyglot> test.pg
    DEPENDS polyglot
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "运行 polyglot 编译器测试"
)

# 打包设置 (CPack)
set(CPACK_PACKAGE_NAME "polyglot")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "polyglot 符号化编程语言编译器")
set(CPACK_PACKAGE_VENDOR "polyglot Team")

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "ZIP;DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
endif()

include(CPack)

# 打印配置信息
message(STATUS "=== polyglot 编译器配置 ===")
message(STATUS "版本: ${PROJECT_VERSION}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ 标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "编译器: ${CMAKE_CXX_COMPILER}")
message(STATUS "系统: ${CMAKE_SYSTEM_NAME}")
message(STATUS "输出目录: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "=============================")

# 使用示例信息
if(CMAKE_BUILD_TYPE STREQUAL "")
    message(STATUS "")
    message(STATUS "构建示例:")
    message(STATUS "  方法1 (推荐): ./build.sh 或 build.bat")
    message(STATUS "  方法2 (CMake): mkdir build && cd build && cmake .. && cmake --build .")
    message(STATUS "")
    message(STATUS "测试运行:")
    message(STATUS "  cmake --build . --target run_test")
    message(STATUS "")
endif()
