@echo off
chcp 65001 >nul 2>&1
echo [INFO] 🚀 polyglot编译器构建系统 (Windows)

REM 设置变量
set SRCDIR=compiler
set SOURCES=%SRCDIR%\main.cpp %SRCDIR%\lexer.cpp %SRCDIR%\parser.cpp %SRCDIR%\semantic.cpp %SRCDIR%\codegen.cpp
set TARGET=polyglot.exe

echo [INFO] 检测C++编译器...

REM 检查MinGW g++
where g++.exe >nul 2>&1
if %errorlevel% == 0 (
    echo [INFO] 使用编译器: MinGW GCC
    echo [INFO] 🔨 开始编译...
    echo [INFO] 编译命令: g++ -std=c++17 -Wall -Wextra -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8 %SOURCES% -o %TARGET%
    echo.
    g++ -std=c++17 -Wall -Wextra -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8 %SOURCES% -o %TARGET%
    goto :check_result
)

REM 检查MSVC cl
where cl.exe >nul 2>&1
if %errorlevel% == 0 (
    echo [INFO] 使用编译器: Microsoft Visual C++
    echo [INFO] 🔨 开始编译...
    echo [INFO] 编译命令: cl /std:c++17 /EHsc /O2 /utf-8 %SOURCES% /Fe%TARGET%
    echo.
    cl /std:c++17 /EHsc /O2 /utf-8 %SOURCES% /Fe%TARGET%
    goto :check_result
)

REM 检查Clang
where clang++.exe >nul 2>&1
if %errorlevel% == 0 (
    echo [INFO] 使用编译器: Clang
    echo [INFO] 🔨 开始编译...
    echo [INFO] 编译命令: clang++ -std=c++17 -Wall -Wextra -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8 %SOURCES% -o %TARGET%
    echo.
    clang++ -std=c++17 -Wall -Wextra -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8 %SOURCES% -o %TARGET%
    goto :check_result
)

echo [ERROR] ❌ 未找到C++编译器！
echo [INFO] 请安装以下之一：
echo   - Visual Studio (推荐): https://visualstudio.microsoft.com/
echo   - MinGW-w64: https://www.mingw-w64.org/
echo   - MSYS2: https://www.msys2.org/
echo.
pause
exit /b 1

:check_result
if %errorlevel% == 0 (
    echo.
    echo [SUCCESS] ✅ 编译成功！
    echo [SUCCESS] 生成文件: %TARGET%
    if exist "%TARGET%" (
        echo [INFO] 文件大小:
        dir "%TARGET%" | findstr "%TARGET%"

        REM 创建中文快捷方式
        echo [INFO] 📄 创建中文快捷方式: 文达
        copy "%TARGET%" "文达" >nul
        if exist "文达" (
            echo [SUCCESS] ✅ 中文快捷方式创建成功: 文达
        ) else (
            echo [WARNING] ⚠️ 中文快捷方式创建失败
        )
    )

    REM 如果参数是test，运行测试
    if "%1" == "test" (
        echo.
        echo [INFO] 🧪 运行测试...
        if exist test.pg (
            "%TARGET%" test.pg
        ) else (
            echo [WARNING] 未找到测试文件 test.pg
        )
    )

    echo.
    echo [INFO] 🎉 构建完成！可以使用: %TARGET% your_program.pg
) else (
    echo.
    echo [ERROR] ❌ 编译失败
    pause
    exit /b 1
)

if not "%1" == "test" pause