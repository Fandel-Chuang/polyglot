#!/bin/bash

# ========================================
# Polyglot集成包管理器测试脚本
# Integrated Package Manager Test Script
# ========================================

set -e  # 遇到错误立即退出

echo "🚀 Polyglot集成包管理器测试"
echo "=========================================="

# 检查编译器是否存在
if ! command -v ./polyglot &> /dev/null && ! command -v polyglot &> /dev/null; then
    echo "❌ 找不到polyglot编译器"
    echo "   请先运行 ./build.sh 编译项目"
    exit 1
fi

# 设置编译器路径
COMPILER="./polyglot"
if [ ! -f "$COMPILER" ]; then
    COMPILER="polyglot"
fi

echo "✅ 使用编译器: $COMPILER"
echo

# 测试1: 基本帮助信息
echo "📋 测试1: 显示帮助信息"
echo "----------------------------------------"
$COMPILER --help
echo
echo "✅ 帮助信息测试完成"
echo

# 测试2: 依赖信息显示
echo "📋 测试2: 显示依赖信息"
echo "----------------------------------------"
$COMPILER --deps-info package_manager_demo.pg
echo
echo "✅ 依赖信息测试完成"
echo

# 测试3: 清理缓存
echo "🧹 测试3: 清理依赖缓存"
echo "----------------------------------------"
$COMPILER --clean-cache
echo
echo "✅ 缓存清理测试完成"
echo

# 测试4: 详细模式编译（跳过依赖以避免真实下载）
echo "🔍 测试4: 详细模式编译（跳过依赖）"
echo "----------------------------------------"
$COMPILER --verbose --no-deps package_manager_demo.pg
echo
echo "✅ 详细模式编译测试完成"
echo

# 测试5: 基本编译测试
echo "📦 测试5: 基本编译测试"
echo "----------------------------------------"
$COMPILER package_manager_demo.pg
echo
echo "✅ 基本编译测试完成"
echo

# 测试6: 测试简单的polyglot程序
echo "📝 测试6: 创建并编译简单程序"
echo "----------------------------------------"

cat > test_simple.pg << 'EOF'
>> "io/console"

main() {
    println("🎉 Polyglot集成包管理器工作正常！")
    println("================================")
    println("✅ 编译器集成成功")
    println("✅ 包管理器集成成功")
    println("✅ 所有功能正常")
    <- 0
}
EOF

echo "创建测试文件: test_simple.pg"
$COMPILER --no-deps test_simple.pg

echo
echo "✅ 简单程序编译测试完成"
echo

# 清理测试文件
echo "🧹 清理测试文件..."
rm -f test_simple.pg
rm -f test_simple_generated.cpp
rm -f test_simple
rm -f test_simple.exe
rm -f package_manager_demo_generated.cpp
rm -f package_manager_demo
rm -f package_manager_demo.exe

echo "✅ 测试文件清理完成"
echo

# 总结
echo "🎉 所有测试完成！"
echo "=========================================="
echo "✅ 帮助系统正常"
echo "✅ 依赖信息显示正常"
echo "✅ 缓存清理功能正常"
echo "✅ 详细模式编译正常"
echo "✅ 基本编译功能正常"
echo "✅ 简单程序编译正常"
echo
echo "🚀 Polyglot集成包管理器已准备就绪！"
echo
echo "📖 使用说明："
echo "   polyglot main.pg                    # 基本编译"
echo "   polyglot --update-deps main.pg      # 更新依赖后编译"
echo "   polyglot --no-deps main.pg          # 跳过依赖解析"
echo "   polyglot --verbose main.pg          # 详细输出"
echo "   polyglot --deps-info main.pg        # 显示依赖信息"
echo "   polyglot --clean-cache              # 清理缓存"
echo "   polyglot --help                     # 显示帮助"
echo