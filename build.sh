#!/bin/bash

# polyglot 编译器跨平台编译脚本
# 支持 Windows (MinGW/MSYS2), Linux, macOS

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_message() {
    echo -e "${2}$1${NC}"
}

print_error() {
    print_message "$1" "$RED"
}

print_success() {
    print_message "$1" "$GREEN"
}

print_info() {
    print_message "$1" "$BLUE"
}

print_warning() {
    print_message "$1" "$YELLOW"
}

# 检测操作系统
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    elif [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        OS="windows"
    else
        print_error "未支持的操作系统: $OSTYPE"
        exit 1
    fi
}

# 检测编译器
detect_compiler() {
    if command -v g++ >/dev/null 2>&1; then
        COMPILER="g++"
        print_info "使用编译器: g++"
    elif command -v clang++ >/dev/null 2>&1; then
        COMPILER="clang++"
        print_info "使用编译器: clang++"
    else
        print_error "未找到 C++ 编译器 (g++ 或 clang++)"
        print_info "正在尝试自动安装..."
        install_compiler_automatically

        # 重新检测
        if command -v g++ >/dev/null 2>&1; then
            COMPILER="g++"
            print_success "成功安装并使用编译器: g++"
        else
            print_error "自动安装失败，请手动安装编译器:"
            print_info "  Linux:   sudo apt install g++ 或 sudo yum install gcc-c++"
            print_info "  macOS:   xcode-select --install 或 brew install gcc"
            print_info "  Windows: 安装 MinGW-w64 或 MSYS2"
            exit 1
        fi
    fi
}

# 检测 CMake
detect_cmake() {
    if command -v cmake >/dev/null 2>&1; then
        CMAKE_VERSION=$(cmake --version | head -n 1 | cut -d ' ' -f 3)
        print_info "已安装 CMake 版本: $CMAKE_VERSION"
        CMAKE_AVAILABLE=true
    else
        print_warning "未找到 CMake"
        print_info "正在尝试自动安装 CMake..."
        install_cmake_automatically

        # 重新检测
        if command -v cmake >/dev/null 2>&1; then
            CMAKE_VERSION=$(cmake --version | head -n 1 | cut -d ' ' -f 3)
            print_success "成功安装 CMake 版本: $CMAKE_VERSION"
            CMAKE_AVAILABLE=true
        else
            print_warning "CMake 自动安装失败，将使用直接编译方式"
            CMAKE_AVAILABLE=false
        fi
    fi
}

# 自动安装编译器
install_compiler_automatically() {
    case $OS in
        "linux")
            if command -v apt >/dev/null 2>&1; then
                print_info "使用 apt 安装 g++..."
                sudo apt update && sudo apt install -y g++
            elif command -v yum >/dev/null 2>&1; then
                print_info "使用 yum 安装 gcc-c++..."
                sudo yum install -y gcc-c++
            elif command -v dnf >/dev/null 2>&1; then
                print_info "使用 dnf 安装 gcc-c++..."
                sudo dnf install -y gcc-c++
            elif command -v pacman >/dev/null 2>&1; then
                print_info "使用 pacman 安装 gcc..."
                sudo pacman -S --noconfirm gcc
            else
                print_warning "未识别的 Linux 发行版，请手动安装编译器"
            fi
            ;;
        "macos")
            if command -v brew >/dev/null 2>&1; then
                print_info "使用 Homebrew 安装 gcc..."
                brew install gcc
            else
                print_info "安装 Xcode 命令行工具..."
                xcode-select --install
            fi
            ;;
        "windows")
            print_warning "Windows 系统请使用 build.bat 脚本或手动安装编译器"
            ;;
    esac
}

# 自动安装 CMake
install_cmake_automatically() {
    case $OS in
        "linux")
            if command -v apt >/dev/null 2>&1; then
                print_info "使用 apt 安装 CMake..."
                sudo apt update && sudo apt install -y cmake
            elif command -v yum >/dev/null 2>&1; then
                print_info "使用 yum 安装 CMake..."
                sudo yum install -y cmake
            elif command -v dnf >/dev/null 2>&1; then
                print_info "使用 dnf 安装 CMake..."
                sudo dnf install -y cmake
            elif command -v pacman >/dev/null 2>&1; then
                print_info "使用 pacman 安装 CMake..."
                sudo pacman -S --noconfirm cmake
            elif command -v snap >/dev/null 2>&1; then
                print_info "使用 snap 安装 CMake..."
                sudo snap install cmake --classic
            else
                # 尝试使用 pip 安装
                if command -v pip3 >/dev/null 2>&1; then
                    print_info "使用 pip3 安装 CMake..."
                    pip3 install --user cmake
                    export PATH="$HOME/.local/bin:$PATH"
                elif command -v pip >/dev/null 2>&1; then
                    print_info "使用 pip 安装 CMake..."
                    pip install --user cmake
                    export PATH="$HOME/.local/bin:$PATH"
                else
                    print_warning "无法自动安装 CMake"
                fi
            fi
            ;;
        "macos")
            if command -v brew >/dev/null 2>&1; then
                print_info "使用 Homebrew 安装 CMake..."
                brew install cmake
            elif command -v port >/dev/null 2>&1; then
                print_info "使用 MacPorts 安装 CMake..."
                sudo port install cmake
            else
                print_info "正在下载并安装 CMake..."
                # 下载官方预编译版本
                curl -L -o cmake.tar.gz "https://github.com/Kitware/CMake/releases/latest/download/cmake-3.27.7-macos-universal.tar.gz"
                tar -xzf cmake.tar.gz
                sudo mv cmake-*/CMake.app/Contents/bin/* /usr/local/bin/
                rm -rf cmake* 2>/dev/null || true
            fi
            ;;
        "windows")
            print_info "Windows CMake 安装将在 build.bat 中处理"
            ;;
    esac
}

# 设置编译选项
set_compile_options() {
    # 基本编译选项 - 添加UTF-8支持
    CXXFLAGS="-std=c++17 -Wall -Wextra -O2 -g -finput-charset=UTF-8 -fexec-charset=UTF-8"

    # 源文件
    SRCDIR="compiler"
    SOURCES="$SRCDIR/main.cpp $SRCDIR/lexer.cpp $SRCDIR/parser.cpp $SRCDIR/semantic.cpp $SRCDIR/codegen.cpp"

    # 根据操作系统设置可执行文件名
    case $OS in
        "windows")
            TARGET="polyglot.exe"
            ;;
        "linux"|"macos")
            TARGET="polyglot"
            ;;
    esac
}

# 清理构建文件
clean_build() {
    print_info "🧹 清理构建文件..."

    # 删除目标文件
    if [ -f "$TARGET" ]; then
        rm "$TARGET"
        print_success "已删除 $TARGET"
    fi

    # 删除对象文件
    find compiler -name "*.o" -delete 2>/dev/null || true

    print_success "清理完成"
}

# 编译项目
compile_project() {
    print_info "🔨 开始编译 polyglot 编译器..."
    print_info "操作系统: $OS"
    print_info "编译器: $COMPILER"
    print_info "目标文件: $TARGET"

    # 创建构建目录
    if [ ! -d "build" ]; then
        print_info "📁 创建构建目录..."
        mkdir -p build
    fi

    # 执行编译
    print_info "编译命令: $COMPILER $CXXFLAGS $SOURCES -o $TARGET"

    if $COMPILER $CXXFLAGS $SOURCES -o $TARGET; then
        print_success "✅ 编译成功！"
        print_success "生成的可执行文件: $TARGET"

        # 显示文件信息
        if command -v ls >/dev/null 2>&1; then
            ls -lh "$TARGET"
        fi

        # 创建中文软链接
        print_info "🔗 创建中文软链接: 文达"
        if ln -sf "$TARGET" "文达" 2>/dev/null; then
            print_success "✅ 中文软链接创建成功: 文达"
        else
            print_warning "⚠️ 中文软链接创建失败"
        fi
    else
        print_error "❌ 编译失败"
        exit 1
    fi
}

# 运行测试
run_test() {
    if [ ! -f "test.pg" ]; then
        print_warning "警告: 未找到测试文件 test.pg"
        return
    fi

    print_info "🧪 运行测试..."

    if [ -f "$TARGET" ]; then
        ./"$TARGET" test.pg
    else
        print_error "错误: 未找到可执行文件 $TARGET"
        exit 1
    fi
}

# 安装编译器 (可选)
install_compiler() {
    case $OS in
        "linux")
            INSTALL_DIR="/usr/local/bin"
            ;;
        "macos")
            INSTALL_DIR="/usr/local/bin"
            ;;
        "windows")
            INSTALL_DIR="/usr/bin"  # MSYS2/MinGW
            ;;
    esac

    if [ -w "$INSTALL_DIR" ] || [ "$EUID" -eq 0 ]; then
        print_info "📦 安装编译器到 $INSTALL_DIR..."
        cp "$TARGET" "$INSTALL_DIR/"
        print_success "安装完成！现在可以在任何地方使用 'polyglot' 命令"
    else
        print_warning "权限不足，无法安装到 $INSTALL_DIR"
        print_info "请使用 sudo 运行: sudo $0 install"
    fi
}

# 显示帮助信息
show_help() {
    echo "polyglot 编译器构建脚本"
    echo ""
    echo "用法:"
    echo "  $0 [选项]"
    echo ""
    echo "选项:"
    echo "  build     编译项目 (默认)"
    echo "  clean     清理构建文件"
    echo "  test      编译并运行测试"
    echo "  install   安装编译器到系统路径"
    echo "  help      显示此帮助信息"
    echo ""
    echo "示例:"
    echo "  $0              # 编译项目"
    echo "  $0 clean        # 清理文件"
    echo "  $0 test         # 编译并测试"
    echo "  sudo $0 install # 安装到系统"
}

# 主函数
main() {
    print_info "🚀 polyglot 编译器构建系统"

    # 检测环境
    detect_os
    detect_compiler
    detect_cmake
    set_compile_options

    # 处理命令行参数
    case "${1:-build}" in
        "build")
            compile_project
            ;;
        "clean")
            clean_build
            ;;
        "test")
            compile_project
            run_test
            ;;
        "install")
            if [ ! -f "$TARGET" ]; then
                print_info "编译器不存在，先进行编译..."
                compile_project
            fi
            install_compiler
            ;;
        "help"|"-h"|"--help")
            show_help
            ;;
        *)
            print_error "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
}

# 运行主函数
main "$@"