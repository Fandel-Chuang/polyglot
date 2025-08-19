# polyglot/文达 测试文件集合

这个目录包含了所有用于测试polyglot编译器的测试文件，按功能和类型进行了分类。

## 📁 目录结构

### `basic_syntax/` - 基础语法测试
- `test_simple.pg` - 最简单的polyglot程序（? x = 42 语法 - 已废弃）
- `test_correct_syntax.pg` - 正确的变量声明语法（x: ? = 42）
- `test_simple_syntax.pg` - 基础语法测试
- `test_english.pg` - 英文版语法示例

### `测试/` - 中文版测试
- `中文语法测试.文达` - 中文版基础语法测试
- `学生管理系统.文达` - 学生管理系统示例
- `文字ＲＰＧ游戏.文达` - 文字RPG游戏示例
- `测试打印函数.文达` - 中文打印函数测试

### `semantic_tests/` - 语义分析测试
- `test_semantic_advanced.pg` - 高级语义分析测试
- `test_semantic_error.pg` - 语义错误检测测试
- `test_parser.pg` - 解析器功能测试

### `examples/` - 复杂示例和功能测试
- `test_complex.pg` - 复杂语法结构测试
- `test_builtin_print.pg` - 内置打印函数测试
- `test_new_syntax.pg` - 新语法特性测试
- `test_new_syntax_success.pg` - 成功的新语法测试

## 🚀 运行测试

从项目根目录运行：

```bash
# 编译编译器
cd compiler
g++ -std=c++17 -O2 *.cpp -o polyglot.exe

# 测试基础语法
polyglot.exe ../ai_tests/basic_syntax/test_correct_syntax.pg

# 测试中文版
polyglot.exe ../ai_tests/测试/中文语法测试.文达

# 测试语义分析
polyglot.exe ../ai_tests/semantic_tests/test_semantic_advanced.pg

# 测试复杂示例
polyglot.exe ../ai_tests/examples/test_complex.pg
```

## 📋 语法规范

### ✅ 支持的语法：
```polyglot
x: ? = 42          // 类型推导变量声明
y: i32 = 100       // 显式类型变量声明
<- result          // 返回语句
```

### ❌ 不支持的语法：
```polyglot
? x = 42           // 已移除的polyglot风格声明
```

## 🎯 测试状态

- ✅ `basic_syntax/` - 所有基础语法测试通过
- ⚠️ `测试/` - 中文版需要Unicode符号支持
- ✅ `semantic_tests/` - 语义分析功能正常
- 🔄 `examples/` - 复杂示例持续开发中