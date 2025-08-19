@echo off
chcp 65001 >nul 2>&1
echo 🚀 生成文达文件关联注册表
echo ==========================

REM 获取当前路径（去掉末尾反斜杠）
set "CURRENT_PATH=%~dp0"
if "%CURRENT_PATH:~-1%"=="\" set "CURRENT_PATH=%CURRENT_PATH:~0,-1%"

set "WENDA_BAT=%CURRENT_PATH%\文达.bat"

REM 转换路径中的反斜杠为双反斜杠（注册表格式）
set "REG_PATH=%WENDA_BAT:\=\\%"

echo [INFO] 当前文达.bat路径: %WENDA_BAT%
echo [INFO] 注册表格式路径: %REG_PATH%
echo.

echo [INFO] 生成注册表文件...

REM 创建注册表文件
echo Windows Registry Editor Version 5.00 > 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo ; 注册 .文达 文件扩展名 >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\.文达] >> 注册文达文件类型_动态.reg
echo @="WenDaFile" >> 注册文达文件类型_动态.reg
echo "Content Type"="text/plain" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo ; 注册 .pg 文件扩展名 >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\.pg] >> 注册文达文件类型_动态.reg
echo @="PolyglotFile" >> 注册文达文件类型_动态.reg
echo "Content Type"="text/plain" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo ; 定义文达文件类型 >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\WenDaFile] >> 注册文达文件类型_动态.reg
echo @="文达源代码文件" >> 注册文达文件类型_动态.reg
echo "EditFlags"=dword:00000000 >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\WenDaFile\DefaultIcon] >> 注册文达文件类型_动态.reg
echo @="%%SystemRoot%%\\System32\\notepad.exe,0" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\WenDaFile\shell] >> 注册文达文件类型_动态.reg
echo @="compile" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo ; 编译选项 >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\WenDaFile\shell\compile] >> 注册文达文件类型_动态.reg
echo @="用文达编译器编译" >> 注册文达文件类型_动态.reg
echo "Icon"="%%SystemRoot%%\\System32\\cmd.exe,0" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\WenDaFile\shell\compile\command] >> 注册文达文件类型_动态.reg
echo @="\"%REG_PATH%\" \"%%1\"" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo ; 编辑选项 >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\WenDaFile\shell\edit] >> 注册文达文件类型_动态.reg
echo @="编辑" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\WenDaFile\shell\edit\command] >> 注册文达文件类型_动态.reg
echo @="notepad.exe \"%%1\"" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo ; 定义Polyglot文件类型 >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\PolyglotFile] >> 注册文达文件类型_动态.reg
echo @="Polyglot源代码文件" >> 注册文达文件类型_动态.reg
echo "EditFlags"=dword:00000000 >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\PolyglotFile\DefaultIcon] >> 注册文达文件类型_动态.reg
echo @="%%SystemRoot%%\\System32\\notepad.exe,0" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\PolyglotFile\shell] >> 注册文达文件类型_动态.reg
echo @="compile" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo ; 编译选项 >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\PolyglotFile\shell\compile] >> 注册文达文件类型_动态.reg
echo @="用Polyglot编译器编译" >> 注册文达文件类型_动态.reg
echo "Icon"="%%SystemRoot%%\\System32\\cmd.exe,0" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\PolyglotFile\shell\compile\command] >> 注册文达文件类型_动态.reg
echo @="\"%REG_PATH%\" \"%%1\"" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo ; 编辑选项 >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\PolyglotFile\shell\edit] >> 注册文达文件类型_动态.reg
echo @="编辑" >> 注册文达文件类型_动态.reg
echo. >> 注册文达文件类型_动态.reg
echo [HKEY_CLASSES_ROOT\PolyglotFile\shell\edit\command] >> 注册文达文件类型_动态.reg
echo @="notepad.exe \"%%1\"" >> 注册文达文件类型_动态.reg

echo [SUCCESS] ✅ 注册表文件生成成功: 注册文达文件类型_动态.reg
echo [INFO] 现在可以运行 安装文达文件关联.bat 进行安装

pause