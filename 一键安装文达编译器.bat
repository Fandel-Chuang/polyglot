@echo off
chcp 65001 >nul 2>&1
echo 🚀 文达编译器一键安装程序
echo ============================
echo.
echo [INFO] 这个脚本将会：
echo   1. 🔧 构建文达编译器
echo   2. 📝 生成文件关联
echo   3. ⚙️ 安装系统集成
echo.

REM 检查管理员权限
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] ⚠️ 需要管理员权限来安装文件关联
    echo [INFO] 请右键以管理员身份运行此脚本
    echo.
    echo [INFO] 或者你可以选择：
    echo   A - 仅构建编译器（无需管理员权限）
    echo   B - 退出
    echo.
    choice /C AB /M "请选择"
    if %errorlevel%==1 goto :build_only
    if %errorlevel%==2 exit /b 0
)

echo [INFO] ✅ 已获得管理员权限，开始完整安装...
echo.

:full_install
echo [STEP 1/3] 🔨 构建文达编译器...
call "build.bat"
if %errorlevel% neq 0 (
    echo [ERROR] ❌ 编译器构建失败
    pause
    exit /b 1
)
echo.

echo [STEP 2/3] 📝 生成文件关联...
call "生成文达文件关联.bat"
echo.

echo [STEP 3/3] ⚙️ 安装文件关联...
regedit /s "注册文达文件类型_动态.reg"

if %errorlevel% == 0 (
    echo [SUCCESS] 🎉 文达编译器安装完成！
    echo.
    echo [INFO] ✨ 现在你可以：
    echo   • 双击 .文达 文件直接编译
    echo   • 双击 .pg 文件直接编译
    echo   • 右键文件选择"用文达编译器编译"
    echo   • 命令行使用: 文达.bat your_program.文达
    echo.
    echo [INFO] 📁 测试文件：ai_tests\测试\中文语法测试.文达
    echo [INFO] 🗑️ 卸载方法：运行 卸载文达文件关联.bat
) else (
    echo [ERROR] ❌ 文件关联安装失败
)
goto :end

:build_only
echo [INFO] 🔨 仅构建编译器模式...
call "build.bat"
if %errorlevel% == 0 (
    echo [SUCCESS] ✅ 编译器构建完成！
    echo [INFO] 命令行使用: 文达.bat your_program.文达
    echo [INFO] 如需图形界面集成，请以管理员身份重新运行此脚本
) else (
    echo [ERROR] ❌ 编译器构建失败
)

:end
echo.
pause