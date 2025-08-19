@echo off
chcp 65001 >nul 2>&1
echo 🚀 构建达文编译器快捷方式
echo ============================
echo.
call "%~dp0build.bat" %*