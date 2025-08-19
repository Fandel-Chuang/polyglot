# polyglot/文达 编程语言

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B17-orange.svg)

一个革命性的编程语言，支持符号化语法和中文编程，采用智能内存管理，零垃圾回收器设计。

## 🌟 特色功能

### � 双语言模式
- **polyglot** (英文版): 使用半角符号的现代化编程语言
- **文达** (中文版): 支持全角符号和中文标识符的本土化编程体验

### 🔧 核心特性
- **符号化语法**: 用直观符号替代传统关键字
- **智能内存管理**: 编译期循环引用检测 + 智能指针
- **三符号赋值系统**: `=`强引用、`~=`弱引用、`:=`值拷贝
- **外部可配置符号**: JSON配置文件动态定义符号映射
- **零垃圾回收**: 高性能内存管理，无运行时开销

## 🚀 快速开始

### 环境要求
- C++17 兼容的编译器 (g++, clang++, MSVC)
- 支持UTF-8编码的终端
- JSON配置文件支持

### 编译安装
```bash
# 克隆项目
git clone <repository-url>
cd polyglot

# 编译编译器
cd compiler
g++ -std=c++17 -O2 *.cpp -o polyglot.exe

# 测试安装
./polyglot.exe ../examples/hello_world.pg
```

## 📚 语法指南

### 基础语法对比

| 功能 | 英文版 (polyglot) | 中文版 (文达) |
|------|------------------|-------------|
| 模块导入 | `>> "iostream"` | `》》 "输入输出流"` |
| 变量声明 | `? name = "Alice"` | `？ 姓名 = "爱丽丝"` |
| 结构体定义 | `@ Player { ... }` | `＠ 玩家 ｛ ... ｝` |
| 方法实现 | `& Player { ... }` | `＆ 玩家 ｛ ... ｝` |
| 函数返回 | `<- value` | `＜－ 值` |
| 循环语句 | `^(condition) { ... }` | `＾（条件）｛ ... ｝` |

### 完整示例

#### 英文版 Hello World
```rust
>> "iostream"

main() {
    print("Hello, polyglot!")
    <- 0
}
```

#### 中文版 Hello World
```rust
》》 "输入输出流"

主函数（） ｛
    打印（"你好，文达！"）
    ＜－ 0
｝
```

### 游戏开发示例

#### 玩家类定义 (中文版)
```rust
》》 "图形库"

＠ 玩家 ｛
    姓名： 字符串，
    生命值： 浮点数，
    位置： 三维向量
｝

＆ 玩家 ｛
    玩家（姓名： 字符串， 初始生命值： 浮点数） ｛
        ＿．姓名 ＝ 姓名
        ＿．生命值 ＝ 初始生命值
        ＿．位置 ＝ 三维向量｛横： 0.0， 纵： 0.0， 高： 0.0｝
    ｝

    受到伤害（伤害量： 浮点数） ｛
        ＿．生命值 －＝ 伤害量
        （＿．生命值 ＜＝ 0.0）？ ｛
            打印（＿．姓名 ＋ " 已死亡！"）
        ｝
    ｝
｝
```

## 🔧 编译器架构

### 编译流程
1. **词法分析** - 支持Unicode和全角符号解析
2. **语法分析** - 生成抽象语法树 (AST)
3. **语义分析** - 类型检查和循环引用检测
4. **代码生成** - 输出优化的C++代码
5. **本地编译** - 生成高性能可执行文件

### 项目结构
```
polyglot/
├── compiler/              # 编译器核心
│   ├── lexer.cpp         # 词法分析器
│   ├── parser.cpp        # 语法分析器
│   ├── semantic.cpp      # 语义分析器
│   ├── codegen.cpp       # 代码生成器
│   ├── symbol_config.cpp # 符号配置管理
│   └── main.cpp          # 编译器入口
├── symbol_mapping.json   # 符号映射配置
├── examples/             # 示例代码
├── tests/               # 测试用例
└── docs/                # 文档
```

## ⚙️ 符号配置系统

polyglot支持通过JSON配置文件自定义符号映射，无需重新编译：

### symbol_mapping.json 示例
```json
{
  "full_width_symbols": {
    "（": "LEFT_PAREN",
    "）": "RIGHT_PAREN",
    "｛": "LEFT_BRACE",
    "｝": "RIGHT_BRACE",
    "？": "QUESTION",
    "＠": "STRUCT_DEF"
  },
  "chinese_keywords": {
    "整数": "TYPE_I32",
    "字符串": "TYPE_STRING",
    "真": "TRUE",
    "假": "FALSE"
  }
}
```

### 添加新符号
1. 编辑 `symbol_mapping.json`
2. 添加符号到相应类别
3. 重新运行编译器（无需重新编译）

## 🎮 游戏开发支持

polyglot特别适合游戏开发，提供：

- **组件化架构**: 接口和结构体组合设计
- **高性能内存管理**: 零GC，适合实时渲染
- **直观的语法**: 符号化语法提高开发效率
- **中文编程**: 降低中文开发者的语言门槛

## � 内存管理

### 三符号赋值系统
```rust
// 强引用 (默认)
? player1 = Player("Alice", 100.0)
? player2 = player1  // 引用计数 +1

// 弱引用 (避免循环引用)
? enemy ~= player1   // 不影响生命周期

// 值拷贝 (深拷贝)
? backup := player1  // 创建完整副本
```

### 循环引用自动检测
编译器在编译期检测并报告潜在的循环引用问题：
```
❌ 编译错误: 检测到循环引用
Player -> Enemy -> Weapon -> Player
建议: 使用弱引用 (~=) 打破循环
```

## 🔬 测试和示例

### 运行示例
```bash
# 英文版示例
./polyglot examples/game_demo.pg

# 中文版示例
./polyglot examples/游戏演示.文达

# 性能测试
./polyglot tests/performance_test.pg
```

### 测试覆盖
- ✅ 词法分析测试 (Unicode + ASCII)
- ✅ 语法分析测试 (双语言模式)
- ✅ 语义分析测试 (类型检查)
- ✅ 内存管理测试 (循环引用检测)
- ✅ 代码生成测试 (C++输出验证)

## 🐛 问题排查

### 常见问题

**Q: 编译时出现Unicode字符错误？**
A: 确保终端支持UTF-8编码，并检查`symbol_mapping.json`中是否定义了该符号

**Q: 如何添加自定义符号？**
A: 编辑`symbol_mapping.json`，添加新的符号映射，无需重新编译

**Q: 循环引用检测太严格？**
A: 使用弱引用`~=`或ID引用来打破循环，参考文档中的最佳实践

**Q: 生成的C++代码性能如何？**
A: polyglot生成高度优化的C++代码，性能接近手写C++

## � 贡献指南

欢迎贡献代码！请遵循以下步骤：

1. Fork 项目仓库
2. 创建功能分支: `git checkout -b feature/amazing-feature`
3. 提交更改: `git commit -m 'Add amazing feature'`
4. 推送分支: `git push origin feature/amazing-feature`
5. 创建 Pull Request

### 开发环境设置
```bash
# 安装依赖
sudo apt-get install g++ cmake

# 运行测试
make test

# 代码格式化
clang-format -i compiler/*.cpp compiler/*.h
```

## 📜 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 🔗 相关链接

- [语法参考](docs/syntax-reference.md)
- [API文档](docs/api-reference.md)
- [游戏开发教程](docs/game-development.md)
- [性能基准测试](docs/benchmarks.md)

## 👥 贡献者

感谢所有为polyglot/文达做出贡献的开发者！

## 📊 项目状态

- 🟢 词法分析器: 完成
- 🟢 语法分析器: 完成
- 🟢 语义分析器: 完成
- 🟢 代码生成器: 完成
- 🟢 符号配置系统: 完成
- 🟡 标准库: 开发中
- 🟡 IDE插件: 计划中
- 🔴 包管理器: 未开始

---

**polyglot/文达** - 让编程更直观，让创意无国界 🌍✨
