#!/bin/bash

echo "🎯 polyglot/文达编译器测试套件"
echo "========================================"

COMPILER="compiler/polyglot_correct.exe"

echo ""
echo "📋 1. 基础语法测试"
echo "========================================"
$COMPILER ai_tests/basic_syntax/test_correct_syntax.pg
echo ""

echo "⚠️  2. 错误语法测试 (预期失败)"
echo "========================================"
$COMPILER ai_tests/basic_syntax/test_simple.pg
echo ""

echo "🔍 3. 语义分析测试"
echo "========================================"
if [ -f "ai_tests/semantic_tests/test_semantic_advanced.pg" ]; then
    $COMPILER ai_tests/semantic_tests/test_semantic_advanced.pg
else
    echo "⚡ 找不到语义测试文件"
fi
echo ""

echo "📊 4. 复杂示例测试"
echo "========================================"
if [ -f "ai_tests/examples/test_complex.pg" ]; then
    $COMPILER ai_tests/examples/test_complex.pg
else
    echo "⚡ 找不到复杂示例文件"
fi

echo ""
echo "🎉 测试完成！"