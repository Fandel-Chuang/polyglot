@echo off
chcp 65001 >nul 2>&1
echo 🗑️ 文达编译器文件关联卸载程序
echo ==================================
echo.

echo [INFO] 正在移除 .文达 和 .pg 文件关联...
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

REM 删除文件关联
echo [INFO] 🗑️ 删除注册表项...
reg delete "HKEY_CLASSES_ROOT\.文达" /f >nul 2>&1
reg delete "HKEY_CLASSES_ROOT\.pg" /f >nul 2>&1
reg delete "HKEY_CLASSES_ROOT\WenDaFile" /f >nul 2>&1
reg delete "HKEY_CLASSES_ROOT\PolyglotFile" /f >nul 2>&1

echo [SUCCESS] ✅ 文件关联卸载完成！
echo.
echo [INFO] 📝 已移除的关联：
echo   • .文达 文件关联
echo   • .pg 文件关联
echo   • 右键菜单项
echo   • 文件图标关联
echo.
echo [INFO] 🔄 重启资源管理器后生效

echo.
pause