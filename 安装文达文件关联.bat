@echo off
chcp 65001 >nul 2>&1
echo 🚀 文达编译器文件关联安装程序
echo ================================
echo.

echo [INFO] 正在注册 .文达 和 .pg 文件类型...
echo [INFO] 这将允许你：
echo   • 双击 .文达 文件直接编译
echo   • 右键菜单中显示"用文达编译器编译"选项
echo   • 在文件管理器中显示正确的文件图标
echo.

REM 检查管理员权限
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] ⚠️ 需要管理员权限来修改注册表
    echo [INFO] 请右键以管理员身份运行此脚本
    pause
    exit /b 1
)

echo [INFO] ✅ 已获得管理员权限

REM 检查动态注册表文件是否存在
if not exist "注册文达文件类型_动态.reg" (
    echo [INFO] 🔄 动态注册表文件不存在，正在生成...
    call "生成文达文件关联.bat"
    echo.
)

REM 导入注册表文件
echo [INFO] 📝 导入文件关联到注册表...
regedit /s "注册文达文件类型_动态.reg"

if %errorlevel% == 0 (
    echo [SUCCESS] ✅ 文件关联注册成功！
    echo.
    echo [INFO] 🎉 安装完成！现在你可以：
    echo   1. 双击任何 .文达 文件来编译
    echo   2. 右键 .文达/.pg 文件选择"用文达编译器编译"
    echo   3. 在资源管理器中看到正确的文件类型描述
    echo.
    echo [INFO] 📁 测试文件位置：ai_tests\测试\
    echo [INFO] 🔧 如需卸载，运行：卸载文达文件关联.bat
) else (
    echo [ERROR] ❌ 文件关联注册失败
    echo [INFO] 请检查注册表文件是否存在
)

echo.
pause