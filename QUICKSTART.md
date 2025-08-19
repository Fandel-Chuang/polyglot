# 🚀 polyglot 快速开始指南

这是 polyglot 编译器的快速开始指南，帮你在 5 分钟内运行起来！

## 📥 第一步：获取代码

```bash
git clone https://github.com/your-username/polyglot.git
cd polyglot
```

## ⚡ 第二步：一键构建

### Linux/macOS 用户：
```bash
# 自动检测环境并构建
chmod +x build.sh && ./build.sh
```

### Windows 用户：
```cmd
# 双击运行或在命令提示符中执行
build.bat
```

**构建脚本会自动：**
- ✅ 检测操作系统
- ✅ 查找 C++ 编译器 (g++/clang++/MSVC)
- ✅ 如果没有找到，尝试自动安装
- ✅ 检测 CMake（用于高级功能）
- ✅ 编译生成 `polyglot` 可执行文件

## 🧪 第三步：测试运行

```bash
# 运行测试程序
./build.sh test      # Linux/macOS
build.bat test       # Windows
```

## 🎯 第四步：编写你的第一个程序

创建 `hello.pg` 文件：
```rust
>> "iostream"

main() {
    ? greeting = "Hello, polyglot!"
    print(greeting)
    <- 0
}
```

编译并运行：
```bash
./polyglot hello.pg   # 会生成对应的 C++ 代码
```

## 📚 常用符号速查

| 符号 | 含义 | 示例 |
|------|------|------|
| `>>` | 导入 | `>> "stdio"` |
| `?` | 变量 | `? name = "Alice"` |
| `@` | 结构 | `@ Point { x: f, y: f }` |
| `&` | 实现 | `& Point { ... }` |
| `<-` | 返回 | `<- value` |
| `*` | 常量 | `* PI = 3.14` |

## 🆘 遇到问题？

### 构建失败？
```bash
# 检查错误信息，脚本会提供详细的安装建议
./build.sh          # 重新运行构建

# 查看详细帮助
./build.sh help     # Linux/macOS
build.bat help      # Windows
```

### 权限问题？
```bash
# Linux/macOS: 确保脚本有执行权限
chmod +x build.sh

# Windows: 以管理员身份运行命令提示符
```

### 编译器不存在？
构建脚本会尝试自动安装，如果失败：

**Linux:**
```bash
sudo apt install g++     # Ubuntu/Debian
sudo yum install gcc-c++  # CentOS/RHEL
```

**macOS:**
```bash
xcode-select --install    # 安装 Xcode 工具
brew install gcc          # 或使用 Homebrew
```

**Windows:**
- 安装 [Visual Studio](https://visualstudio.microsoft.com/) (推荐)
- 或安装 [MinGW-w64](https://www.mingw-w64.org/)

## 🎉 成功了！

如果看到类似下面的输出，说明一切正常：
```
[INFO] 🚀 polyglot 编译器构建系统
[INFO] 使用编译器: g++
[INFO] 已安装 CMake 版本: 3.22.1
[SUCCESS] ✅ 编译成功！
[SUCCESS] 生成的可执行文件: polyglot
```

## 📖 下一步

- 阅读 [README.md](README.md) 了解详细功能
- 查看 [示例程序](test.pg)
- 学习 [完整语法](polyglot完整符号语法示例.md)

---

**有问题？** 查看 [故障排除](README.md#故障排除) 或提交 [Issue](https://github.com/your-username/polyglot/issues)！